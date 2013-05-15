/*
 * meego-handset-sms - Meego Handset SMS application
 *
 * Copyright (c) 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include <QDBusReply>
#include <QDebug>
#include <QTime> // for debug message timing
#include <QDesktopServices>
#include <QTextCodec>

#include <MMessageBox>

#include <qmailnamespace.h>
#include <qmailserviceconfiguration.h>
#include <qmailaccountkey.h>

#include "backend.h"
#include "util.h"
#include "dbustypes.h"

#define TELEPHONY_SERVICE "org.ofono"
#define TELEPHONY_MGR_INTERFACE TELEPHONY_SERVICE ".Manager"
#define TELEPHONY_MSGMGR_INTERFACE TELEPHONY_SERVICE ".MessageManager"
#define TELEPHONY_SMSHIST_INTERFACE TELEPHONY_SERVICE ".SmsHistory"

#define DIALER_SERVICE "com.meego.dialer"
#define DIALER_PATH "/com/meego/dialer"
#define DIALER_INTERFACE "com.meego.dialer"

Backend::Backend() :
    mManagerInterface(0),
    mMessageManagerInterface(0),
    mDialerInterface(0),
    mSeasidemodel(0),
    mMessageServerProcess(0),
    mMailStore(0)
{
    QTime startupTimer;
    startupTimer.start();
    QTime totalStartupTimer;
    totalStartupTimer.start();

    registerMyDataTypes();

    ////
    // connect to MessageManager for sending messages
    mManagerInterface = new ManagerInterface(TELEPHONY_SERVICE, "/", QDBusConnection::systemBus(), this);
    if (!mManagerInterface->isValid()) {
        qCritical("unable to connect to " TELEPHONY_MGR_INTERFACE);
    } else {
        QDBusPendingReply<OfonoPathList> reply = mManagerInterface->GetModems();
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)), this, SLOT(getModemsFinished(QDBusPendingCallWatcher*)));
    }

    ////
    // connect to Dialer for initiating calls

    mDialerInterface = new DialerInterface(DIALER_SERVICE, DIALER_PATH, QDBusConnection::sessionBus(), this);
    if (!mDialerInterface->isValid()) {
        qCritical("unable to connect to " DIALER_INTERFACE);
    }

    qDebug() << "Backend startup:" << startupTimer.elapsed() << "for initial dbus setup";
    startupTimer.restart();

    ////
    // set up phone->name and phone->avatar lookup

    mSeasidemodel = new SeasideSyncModel();
    int i;
    for (i = 0; i < mSeasidemodel->rowCount(); i++) {
        QStringList phonelist = mSeasidemodel->data(mSeasidemodel->index(i,Seaside::ColumnPhoneNumbers), Seaside::DataRole).toStringList();
        QString name = mSeasidemodel->data(mSeasidemodel->index(i,Seaside::ColumnFirstName), Seaside::DataRole).toString() + " " +
            mSeasidemodel->data(mSeasidemodel->index(i,Seaside::ColumnLastName), Seaside::DataRole).toString();
        QString avatar = mSeasidemodel->data(mSeasidemodel->index(i,Seaside::ColumnAvatar), Seaside::DataRole).toString();
        int j;
        for (j = 0; j < phonelist.size(); j++) {
            mNameindex.insert(stripPhone(phonelist[j]), name);
            mAvatarindex.insert(stripPhone(phonelist[j]), avatar);
        }
    }

    qDebug() << "Backend startup:" << startupTimer.elapsed() << "for seaside";
    startupTimer.restart();

    // start messageserver if it isn't already running
    if (!isMessageServerRunning()) {
        if (!startMessageServer()) {
            qCritical("Could not start messageserver");
        }
    }

    qDebug() << "Backend startup:" << startupTimer.elapsed() << "for starting messageserver";
    startupTimer.restart();

    mMailStore = QMailStore::instance();

    qDebug() << "Backend startup:" << startupTimer.elapsed() << "for initiating QMailStore";
    startupTimer.restart();

    setupAccount();

    qDebug() << "Backend startup:" << startupTimer.elapsed() << "for setting up account";
    startupTimer.restart();

    connect(mMailStore, SIGNAL(messagesAdded(const QMailMessageIdList&)), this, SLOT(messagesAdded(const QMailMessageIdList&)));
    connect(mMailStore, SIGNAL(messagesRemoved(const QMailMessageIdList&)), this, SLOT(messagesRemoved(const QMailMessageIdList&)));
    connect(mMailStore, SIGNAL(messagesUpdated(const QMailMessageIdList&)), this, SLOT(messagesUpdated(const QMailMessageIdList&)));

    qDebug() << "Backend startup total:" << totalStartupTimer.elapsed();
}

Backend::~Backend()
{}

SeasideSyncModel *Backend::seasidemodel()
{
    return mSeasidemodel;
}

QString Backend::getName(QString phone)
{
    QString stripped = stripPhone(phone);
    if (mNameindex.contains(stripped)) {
        return mNameindex[stripped];
    } else {
        return phone;
    }
}

QPixmap Backend::getAvatar(QString phone)
{
    QString stripped = stripPhone(phone);

    const QPixmap *photo;
    if (mAvatarindex.contains(stripped)) {
        //qDebug() << "avatar:" << mAvatarindex[stripped];
        photo = MTheme::pixmap(mAvatarindex[stripped], QSize(56, 56));
    } else {
        //qDebug() << "avatar not found";
        photo = MTheme::pixmap("icon-m-content-avatar-placeholder", QSize(56, 56));
    }

    // create alpha mask
    QImage mask(56, 56, QImage::Format_ARGB32_Premultiplied);
    QPainter maskpainter(&mask);
    //maskpainter.setRenderHint(QPainter::Antialiasing, true);
    maskpainter.setCompositionMode(QPainter::CompositionMode_Source);
    maskpainter.fillRect(mask.rect(), Qt::transparent);
    maskpainter.setPen(QPen(Qt::transparent));
    maskpainter.setBrush(QBrush(Qt::white));
    maskpainter.drawRoundedRect(1.0, 1.0, 53.0, 53.0, 5.0, 5.0);

    // scale/crop photo
    qreal x, y, w, h;
    if (photo->width() > photo->height()) {
        x = (photo->width() - photo->height()) / 2.0;
        w = photo->height();
        y = 0.0;
        h = photo->height();
    } else {
        y = (photo->height() - photo->width()) / 2.0;
        h = photo->width();
        x = 0.0;
        w = photo->width();
    }
    QImage tmp(56, 56, QImage::Format_ARGB32_Premultiplied);
    QPainter tmppainter(&tmp);
    //tmppainter.setRenderHint(QPainter::Antialiasing, true);
    tmppainter.setCompositionMode(QPainter::CompositionMode_Source);
    tmppainter.drawPixmap(tmp.rect(), *photo, QRectF(x, y, w, h));

    // mask photo
    tmppainter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    tmppainter.drawImage(tmp.rect(), mask, mask.rect());

    // create final image
    QImage dst(56, 56, QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&dst);
    //painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(dst.rect(), Qt::transparent);

    // draw rounded rect background
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    QPen pen(QColor(0xB0, 0xB0, 0xB0));
    QBrush brush(Qt::white);
    pen.setWidthF(1.0);
    painter.setPen(pen);
    painter.setBrush(brush);
    painter.drawRoundedRect(0.0, 0.0, 55.0, 55.0, 5.0, 5.0);

    // draw masked photo
    painter.drawImage(dst.rect(), tmp, tmp.rect());


    return QPixmap::fromImage(dst);
}

QString Backend::toUnicodeMessage(const QString& aMsg)
{
    // Convert the string to locale specific unicode representation
    QTextCodec* codec = QTextCodec::codecForLocale();
    return codec->toUnicode(aMsg.toAscii());
}

QList<SmsData> Backend::getSmsHistory(QString phone, bool excludeDrafts)
{
    QList<SmsData> messages;
    QMailMessageKey key = QMailMessageKey::messageType(QMailMessage::Sms);
    if (! phone.isEmpty()) {
        key &= QMailMessageKey::sender(phone);
    }
    if (excludeDrafts) {
        key &= QMailMessageKey::status(QMailMessage::Draft,QMailDataComparator::Excludes);
    }

    QMailMessageIdList ids = QMailStore::instance()->queryMessages(key);
    foreach (QMailMessageId id, ids) {
        const QMailMessage qmsg = QMailMessage(id);
        SmsData dat = qmsg;
        dat.message = toUnicodeMessage(dat.message);
        dat.sender  = stripPhone(dat.sender);
        if (!mMessageIdMap.contains(id)) {
            mMessageIdMap.insert(id, dat.id);

        }
        messages.append(dat);
    }
    return messages;
}

MessageManagerInterface *Backend::getMessageManager()
{
    return mMessageManagerInterface;
}

void Backend::send(const QString phone, const QString message, QMailMessageId qid)
{
    if (!mMessageManagerInterface || !mMessageManagerInterface->isValid()) {
        qCritical("not connected to ofono");
        //% "Error: not connected to ofono"
        MMessageBox message(qtTrId("backend_send_err"));
        message.exec();
        return;
    }
    if (qid.isValid()) {
        removeDrafts(phone);
    }

    QString stripped = stripPhone(phone);
    qDebug() << "Sending phone:" << stripped << ", message:" << message;
    QDBusPendingReply<void> reply = mMessageManagerInterface->SendMessage(stripped, message);

    // Watcher is deleted in the slot
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)), this, SLOT(genericCallHandler(QDBusPendingCallWatcher*)));
}

void Backend::call(const QString number)
{
    /*
    if (!mDialerInterface || !mDialerInterface->isValid()) {
        qCritical("not connected to dialer");
        MMessageBox message("Error: not connected to dialer");
        message.exec();
        return;
    }
    */
    QDBusPendingReply<void> reply = mDialerInterface->call(number);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)), this, SLOT(genericCallHandler(QDBusPendingCallWatcher*)));
}

void Backend::openUrl(const QString url)
{
    QDesktopServices::openUrl(QUrl(url, QUrl::TolerantMode));
}

void Backend::getModemsFinished(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<OfonoPathList> reply = *call;
    if (reply.isError()) {
        qCritical() << reply.error().name() << reply.error().message();
    } else {
        if (!reply.value().size()) {
            qCritical("no modems found");
        } else {
            mMessageManagerInterface = new MessageManagerInterface(TELEPHONY_SERVICE, reply.value().first().path.path(), QDBusConnection::systemBus(), this);
            if (!mMessageManagerInterface->isValid()) {
                qCritical("unable to connect to " TELEPHONY_MSGMGR_INTERFACE);
            }
        }
    }
    delete call;
}

void Backend::genericCallHandler(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<void> reply = *call;
    if (reply.isError()) {
        qCritical() << reply.error().name() << reply.error().message();
    }
    delete call;
}

void Backend::saveDraft(QMailMessageId qid, const QString message)
{
    qDebug() << __FUNCTION__ << "(qid: " << qid << ", message:" << "'" << message << "'')";

    QMailMessage msg = QMailMessage(qid);
    msg.setBody(QMailMessageBody::fromData(message, QMailMessageContentType("text/plain"), QMailMessageBody::NoEncoding));
    msg.setDate(QMailTimeStamp(QDateTime::currentDateTime()));
    if (!mMailStore->updateMessage(&msg)) {
        qWarning("saveDraft: addMessage failed: %d", mMailStore->lastError());
    }
}

void Backend::saveDraft(const QString phone, const QString message)
{
    qDebug() << __FUNCTION__ << "(phone: " << phone << ", message:" << "'" << message << "'')";

    const QString stripped = stripPhone(phone);
    QMailMessage msg = QMailMessage();
    msg.setMessageType(QMailMessage::Sms);
    msg.setParentAccountId(mAccountId);
    msg.setParentFolderId(QMailFolder::LocalStorageFolderId);
    msg.setStatus(QMailMessage::Incoming, false);
    msg.setStatus(QMailMessage::Outgoing, true);
    msg.setStatus(QMailMessage::Read, true);
    msg.setStatus(QMailMessage::Draft, true);
    msg.setFrom(QMailAddress(stripped));
    msg.setBody(QMailMessageBody::fromData(message, QMailMessageContentType("text/plain"), QMailMessageBody::NoEncoding));
    msg.setDate(QMailTimeStamp(QDateTime::currentDateTime()));
    if (!mMailStore->addMessage(&msg)) {
        qWarning("saveDraft: addMessage failed: %d", mMailStore->lastError());
    }
}


void Backend::removeDrafts(QString phone)
{
    const QString stripped = stripPhone(phone);
    QMailMessageIdList messageIds = mMailStore->queryMessages(
            QMailMessageKey::messageType(QMailMessage::Sms) &
            QMailMessageKey::sender(stripped) &
            QMailMessageKey::status(QMailMessage::Draft));

    foreach (QMailMessageId id, messageIds) {
        if (!mMailStore->removeMessage(id)) {
            qWarning() << __FUNCTION__ << ": removeMessage failed: " << mMailStore->lastError();
        }
        else {
            qDebug() << "Removed draft " << id;
        }
    }
}


QString Backend::getDraft(const QString phone, QMailMessageId & qid)
{
    const QString stripped = stripPhone(phone);
    QString message;
    QMailMessageIdList messageIds = mMailStore->queryMessages(
            QMailMessageKey::messageType(QMailMessage::Sms) &
            QMailMessageKey::sender(stripped) &
            QMailMessageKey::status(QMailMessage::Draft));
    if (!messageIds.isEmpty()) {
        qid = messageIds.first();
        qDebug() << __FUNCTION__ << ": found" << messageIds.size() << "drafts that match" << phone
                 << ".  First id is" << qid << ".";
        message = toUnicodeMessage(QMailMessage(qid).body().data());
        qDebug() << "Message is" << message;
    }
    return message;
}

void Backend::markRead(QString phone)
{
    const QString stripped = stripPhone(phone);
    QMailMessageId id;
    QMailMessageIdList ids = QMailStore::instance()->queryMessages(
            QMailMessageKey::messageType(QMailMessage::Sms) &
                QMailMessageKey::status(QMailMessage::Draft, QMailDataComparator::Excludes) &
                (QMailMessageKey::sender(stripped) |
                QMailMessageKey::recipients(stripped))
            );
    foreach (id, ids) {
        QMailMessage message = QMailMessage(id);
        if (!(message.status() & QMailMessage::Read)) {
            message.setStatus(QMailMessage::Read, true);
            if (!mMailStore->updateMessage(&message)) {
                qWarning("markRead: updateMessage failed: %d", mMailStore->lastError());
            }
        }
    }
}

void Backend::deleteThread(QString phone)
{
    const QString stripped = stripPhone(phone);
    if (!mMailStore->removeMessages(
                QMailMessageKey::messageType(QMailMessage::Sms) & (
                    QMailMessageKey::sender(stripped) |
                    QMailMessageKey::recipients(stripped)
                    )
                )
       ) {
        qWarning("deleteThread: removeMessages failed: %d", mMailStore->lastError());
    }
}

void Backend::deleteAllThreads()
{
    if (!mMailStore->removeMessages(QMailMessageKey::messageType(QMailMessage::Sms))) {
        qWarning("deleteAllThreads: removeMessages failed: %d", mMailStore->lastError());
    }
}


void Backend::messagesAdded(const QMailMessageIdList &ids)
{
    QList<SmsData> messages;
    QMailMessageId id;
    foreach (id, ids) {
        if (!mMessageIdMap.contains(id)) {
            SmsData dat = QMailMessage(id);
            dat.sender  = stripPhone(dat.sender);
	    dat.message = toUnicodeMessage(dat.message);
            mMessageIdMap.insert(id, dat.id);
            messages.append(dat);
        }
    }
    if (messages.size())
        emit smsAdded(messages);
}

void Backend::messagesRemoved(const QMailMessageIdList &ids)
{
    QList<QString> messageids;
    QMailMessageId id;
    foreach (id, ids) {
        if (mMessageIdMap.contains(id)) {
            messageids.append(mMessageIdMap[id]);
            mMessageIdMap.remove(id);
        }
    }
    if (messageids.size())
        emit smsRemoved(messageids);
}

void Backend::messagesUpdated(const QMailMessageIdList &ids)
{
    QList<SmsData> messages;
    QMailMessageId id;
    foreach (id, ids) {
        if (mMessageIdMap.contains(id)) {
            SmsData dat = QMailMessage(id);
            dat.sender  = stripPhone(dat.sender);
            dat.message = toUnicodeMessage(dat.message);
            messages.append(dat);
        }
    }
    if (messages.size())
        emit smsModified(messages);
}

bool Backend::isMessageServerRunning()
{
    QString lockfile = "messageserver-instance.lock";
    int id = QMail::fileLock(lockfile);
    if (id == -1) {
        return true;
    }
    QMail::fileUnlock(id);
    return false;
}

bool Backend::startMessageServer()
{
    if (mMessageServerProcess) {
        delete mMessageServerProcess;
    }
    mMessageServerProcess = new QProcess(this);

    mMessageServerProcess->startDetached(QMail::messageServerPath() + "/messageserver");
    return mMessageServerProcess->waitForStarted();
}

void Backend::setupAccount()
{
    mAccountId = QMailAccountId();

    // Do we already have a SMS account?

    const QMailAccountKey key = QMailAccountKey::messageType(QMailMessage::Sms);
    const QMailAccountIdList accountList = mMailStore->queryAccounts(key);
    if (! accountList.isEmpty()) {
        mAccountId = accountList.first();
        return;
    }

    // No SMS account, create one

    qDebug("no sms account found, creating account");
    QMailAccount account;
    account.setName("sms account");
    account.setMessageType(QMailMessage::Sms);
    account.setStatus(QMailAccount::MessageSource, true);
    account.setStatus(QMailAccount::CanRetrieve, true);
    account.setStatus(QMailAccount::Enabled, true);
    QMailAccountConfiguration config;
    config.addServiceConfiguration("qmfstoragemanager");
    QMailAccountConfiguration::ServiceConfiguration &srvcCfg(config.serviceConfiguration("qmfstoragemanager"));
    srvcCfg.setValue("version", "101");
    srvcCfg.setValue("servicetype", "storage");
    srvcCfg.setValue("basePath", "");
    config.addServiceConfiguration("sms");
    QMailAccountConfiguration::ServiceConfiguration *svccfg = &config.serviceConfiguration("sms");
    if (!svccfg) {
        qCritical("setupAccount: serviceConfiguration failed: %d", mMailStore->lastError());
        return;
    }
    svccfg->setValue("servicetype","source");
    svccfg->setValue("version","100");
    if (mMailStore->addAccount(&account, &config)) {
        mAccountId = account.id();
        return;
    }

    // No SMS account and can't create one.

    qCritical("setupAccount: addAccount failed: %d", mMailStore->lastError());
}

