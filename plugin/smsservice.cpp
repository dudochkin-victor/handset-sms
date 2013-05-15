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
#include <QtPlugin>
#include <QDebug>

#include <QContactDetailFilter>
#include <QContactPhoneNumber>

#include <qmailstore.h>
#include <qmailmessageservice.h>

#include <seasidesyncmodel.h>

#include "../src/smsdata.h"
#include "../src/dbustypes.h"
#include "smsservice.h"
#include "smssettings.h"

#define TELEPHONY_SERVICE "org.ofono"
#define TELEPHONY_MGR_INTERFACE TELEPHONY_SERVICE ".Manager"
#define TELEPHONY_MSGMGR_INTERFACE TELEPHONY_SERVICE ".MessageManager"
#define TELEPHONY_SMSHIST_INTERFACE TELEPHONY_SERVICE ".SmsHistory"
#define TELEPHONY_CBS_INTERFACE TELEPHONY_SERVICE ".CellBroadcast"

namespace { const QString serviceKey("sms"); }

class SmsService::Source : public QMailMessageSource
{
    Q_OBJECT

public:
    Source(SmsService *service)
        : QMailMessageSource(service)
    {}
public slots:
private:
};

class SmsConfigurator : public QMailMessageServiceConfigurator
{
public:
    SmsConfigurator() {}
    ~SmsConfigurator() {}

    virtual QString service() const;
    virtual QString displayName() const;

    virtual QMailMessageServiceEditor *createEditor(QMailMessageServiceFactory::ServiceType type);
};

QString SmsConfigurator::service() const
{
    return serviceKey;
}

QString SmsConfigurator::displayName() const
{
    return "SMS";
}

QMailMessageServiceEditor *SmsConfigurator::createEditor(QMailMessageServiceFactory::ServiceType type)
{
    if (type == QMailMessageServiceFactory::Source)
        return new SmsSettings;
    else
        return 0;
}

SmsService::SmsService(const QMailAccountId &accountId)
    : QMailMessageService(),
    mSmsHistoryInterface(0),
    mManagerInterface(0),
    mCellBroadcastInterface(0),
    mAccountId(accountId),
    mSource(new Source(this))
{
    //qDebug("SmsService constructor");
    registerMyDataTypes();

    // connect to QtContacts
    SeasideSyncModel *syncmodel = new SeasideSyncModel();
    mContactManager = syncmodel->manager();

    // listen for removed/updated messages
    connect(QMailStore::instance(), SIGNAL(messagesUpdated(const QMailMessageIdList&)), this, SLOT(messagesUpdated(const QMailMessageIdList&)));
    connect(QMailStore::instance(), SIGNAL(messagesRemoved(const QMailMessageIdList&)), this, SLOT(messagesRemoved(const QMailMessageIdList&)));

    // connect to SmsHistory
    mSmsHistoryInterface = new SmsHistoryInterface(TELEPHONY_SERVICE, "/", QDBusConnection::systemBus(), this);
    if (!mSmsHistoryInterface->isValid()) {
        qCritical("unable to connect to " TELEPHONY_SMSHIST_INTERFACE);
    } else {
        connect(mSmsHistoryInterface, SIGNAL(SmsHistoryChanged(uint)), this, SLOT(smsHistoryChanged()));
        connect(mSmsHistoryInterface, SIGNAL(SmsStatusChanged(const QString&, ushort, int)), this, SLOT(smsStatusChanged(const QString&, ushort, int)));
    }

    // connect to Manager
    QString modempath;
    mManagerInterface = new ManagerInterface(TELEPHONY_SERVICE, "/", QDBusConnection::systemBus(), this);
    if (!mManagerInterface->isValid()) {
        qCritical("unable to connect to " TELEPHONY_MGR_INTERFACE);
    } else {
        QDBusPendingReply<OfonoPathList> reply = mManagerInterface->GetModems();
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)), this, SLOT(getModemsFinished(QDBusPendingCallWatcher*)));
    }

    // check for new messages on startup
    QTimer::singleShot(0, this, SLOT(smsHistoryChanged()));
}

SmsService::~SmsService()
{
    //qDebug("SmsService destructor");
    delete mSource;
}

QString SmsService::service() const
{
    return serviceKey;
}

QMailAccountId SmsService::accountId() const
{
    return mAccountId;
}

bool SmsService::available() const
{
    return true;
}

bool SmsService::cancelOperation(QMailServiceAction::Status::ErrorCode code, const QString &text)
{
    Q_UNUSED(code);
    Q_UNUSED(text);
    return true;
}

bool SmsService::hasSource() const
{
    return true;
}

QMailMessageSource &SmsService::source() const
{
    return *mSource;
}

void SmsService::messagesUpdated(const QMailMessageIdList &ids)
{
    QMailMessageId id;
    foreach (id, ids) {
        if (mNotifications.contains(id) &&
                (QMailMessage(id).status() & QMailMessage::Read)) {
            if (!mNotifications[id].remove()) {
                qWarning("could not remove notification");
            }
            mNotifications.remove(id);
        }
    }
}

void SmsService::messagesRemoved(const QMailMessageIdList &ids)
{
    QMailMessageId id;
    foreach (id, ids) {
        if (mNotifications.contains(id)) {
            if (!mNotifications[id].remove()) {
                qWarning("could not remove notification");
            }
            mNotifications.remove(id);
        }
    }
}

void SmsService::getModemsFinished(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<OfonoPathList> reply = *call;
    if (reply.isError()) {
        qCritical() << reply.error().name() << reply.error().message();
    } else {
        // connect to CellBroadcast
        if (!reply.value().size()) {
            qCritical("no modems found");
        } else {
            mCellBroadcastInterface = new CellBroadcastInterface(TELEPHONY_SERVICE, reply.value().first().path.path(), QDBusConnection::systemBus(), this);
            if (!mCellBroadcastInterface->isValid()) {
                qCritical("unable to connect to " TELEPHONY_CBS_INTERFACE);
            } else {
                qDebug("Connecting cell broadcast signals");
                connect(mCellBroadcastInterface, SIGNAL(EmergencyBroadcast(const QString&, const QVariantMap&)), this, SLOT(emergencyBroadcast(const QString&, const QVariantMap&)));
                connect(mCellBroadcastInterface, SIGNAL(IncomingBroadcast(const QString&, ushort)), this, SLOT(incomingBroadcast(const QString&, ushort)));
            }

            mMessageManagerInterface = new MessageManagerInterface(TELEPHONY_SERVICE, reply.value().first().path.path(), QDBusConnection::systemBus(), this);
            if (!mMessageManagerInterface->isValid()) {
                qCritical("unable to connect to " TELEPHONY_MSGMGR_INTERFACE);
            } else {
                qDebug("Connecting message manager signals");
                connect(mMessageManagerInterface, SIGNAL(ImmediateMessage(const QString&, const QVariantMap&)), this, SLOT(showImmediateMessage(const QString&, const QVariantMap&)));
            }
        }
    }
    delete call;
}

void SmsService::smsHistoryChanged()
{
    qDebug("smsHistoryChanged");
    QDBusPendingReply<QMailMessageList> reply = mSmsHistoryInterface->GetSmsHistory();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)), this, SLOT(getSmsHistoryFinished(QDBusPendingCallWatcher*)));
}

void SmsService::smsStatusChanged(const QString &msgid, ushort status, int endtime)
{
    Q_UNUSED(endtime);

    QMailMessageKey filter = QMailMessageKey::customField("id", msgid);
    QMailMessageIdList ids = QMailStore::instance()->queryMessages(filter);
    if (ids.isEmpty()) {
        qDebug() << "smsStatusChanged: could not find msgid" << msgid;
        mStatusBacklog.insert(msgid, status);
    }
    QMailMessageId id;
    foreach (id, ids) {
        QMailMessage *msg = new QMailMessage(id);
        switch (status) {
            case SmsData::PENDING:
                msg->setCustomField("status", "PENDING");
                break;
            case SmsData::SENT:
                msg->setCustomField("status", "SENT");
                break;
            case SmsData::DELIVERED:
                msg->setCustomField("status", "DELIVERED");
                break;
            case SmsData::DELIVER_FAILED:
                msg->setCustomField("status", "DELIVER_FAILED");
                break;
            case SmsData::FAILED:
                msg->setCustomField("status", "FAILED");
                break;
            default:
                break;
        }
        if (!QMailStore::instance()->updateMessage(msg)) {
            qCritical("updateMessage failed: %d", QMailStore::instance()->lastError());
        }
    }
}

void SmsService::getSmsHistoryFinished(QDBusPendingCallWatcher *call)
{
    qDebug("getSmsHistoryFinished");
    QDBusPendingReply<QMailMessageList> reply = *call;
    if (reply.isError()) {
        qCritical() << reply.error().name() << reply.error().message();
    } else {
        QMailMessageList list = reply.argumentAt<0>();
        qDebug() << "got" << list.size() << "messages";
        QMutableListIterator<QMailMessage> i(list);
        while (i.hasNext()) {
            QMailMessage &msg = i.next();
            QString msgid = msg.customField("id");
            if (mStatusBacklog.contains(msgid)) {
                qDebug() << "updating from status backlog:" << msgid << mStatusBacklog[msgid];
                switch (mStatusBacklog.take(msgid)) {
                    case SmsData::PENDING:
                        msg.setCustomField("status", "PENDING");
                        break;
                    case SmsData::SENT:
                        msg.setCustomField("status", "SENT");
                        break;
                    case SmsData::DELIVERED:
                        msg.setCustomField("status", "DELIVERED");
                        break;
                    case SmsData::DELIVER_FAILED:
                        msg.setCustomField("status", "DELIVER_FAILED");
                        break;
                    case SmsData::FAILED:
                        msg.setCustomField("status", "FAILED");
                        break;
                    default:
                        break;
                }
            }
            msg.setParentAccountId(mAccountId);
            msg.setParentFolderId(QMailFolder::LocalStorageFolderId);
            if (!QMailStore::instance()->addMessage(&msg)) {
                qCritical("addMessage failed: %d", QMailStore::instance()->lastError());
            }
            // create notification
            if (!(msg.status() & QMailMessage::Read)) {
                QString phone = msg.from().toString();
                QString name = phone;
                // lookup contact name
                QContactDetailFilter filter;
                filter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName);
                filter.setMatchFlags(QContactFilter::MatchPhoneNumber);
                filter.setValue(phone);
                QList<QContact> contacts = mContactManager->contacts(filter);
                if (!contacts.isEmpty()) {
                    name = contacts.first().displayLabel();
                }
                MNotification notification(MNotification::MessageArrivedEvent, name, msg.body().data());
                notification.setImage("icon-s-sms");
                QList<QVariant> arguments;
                arguments.append(QVariant(msg.from().toString()));
                arguments.append(QVariant(QString()));
                MRemoteAction action("com.meego.sms", "/", "com.meego.sms", "showdialogpage", arguments);
                notification.setAction(action);
                notification.publish();
                mNotifications.insert(msg.id(),notification);
            }
        }
        mSmsHistoryInterface->SetSmsHistoryRead();
    }
    delete call;
}

void SmsService::showImmediateMessage(const QString &message, const QVariantMap &properties)
{
    Q_UNUSED(properties);
    qDebug() << "Immediate message: " << message;

    // create notification
    // % "Message Received"
    MNotification notification(MNotification::MessageArrivedEvent, qtTrId("backend_immediate_message"), message);
    notification.setImage("icon-s-sms");
    notification.publish();
}


void SmsService::emergencyBroadcast(const QString &message, const QVariantMap &properties)
{
    qDebug() << "Emergency broadcast message: " << message;

    // create notification
    QString type;
    if (properties.contains("EmergencyType")) {
        // FIXME: need to test correct way to extract the type
        //type = properties["EmergencyType"].toString();
        properties["EmergencyType"].value<QDBusArgument>() >> type;
    }

    if (type == "Earthquake") {
        // % "Earthquake"
        type = qtTrId("backend_broadcast_earthquake");
    } else if (type == "Tsunami") {
        // % "Tsunami"
        type = qtTrId("backend_broadcast_tsunami");
    } else if (type == "Earthquake+Tsunami") {
        // % "Earthquake+Tsunami"
        type = qtTrId("backend_broadcast_earthquaketsunami");
    } else if (type == "Other") {
        // % "Other"
        type = qtTrId("backend_broadcast_other");
    }

    // % "Emergency Broadcast Message"
    MNotification notification(MNotification::MessageArrivedEvent, qtTrId("backend_emergency_broadcast"), type + ": " + message);
    notification.setImage("icon-s-sms");
    notification.publish();
}

void SmsService::incomingBroadcast(const QString &message, ushort channel)
{
    // create notification
    // % "Broadcast Message (%1)"
    MNotification notification(MNotification::MessageArrivedEvent, qtTrId("backend_broadcast").arg(channel), message);
    notification.setImage("icon-s-sms");
    notification.publish();
}

Q_EXPORT_PLUGIN2(sms,SmsServicePlugin)

SmsServicePlugin::SmsServicePlugin() : QMailMessageServicePlugin()
{}

QString SmsServicePlugin::key() const
{
    return serviceKey;
}

bool SmsServicePlugin::supports(QMailMessageServiceFactory::ServiceType type) const
{
    return (type == QMailMessageServiceFactory::Source);
}

bool SmsServicePlugin::supports(QMailMessage::MessageType type) const
{
    //qDebug("SmsServicePlugin::supports()");
    return (type == QMailMessage::Sms);
}

QMailMessageService *SmsServicePlugin::createService(const QMailAccountId &id)
{
    //qDebug("SmsServicePlugin::createService()");
    return new SmsService(id);
}

QMailMessageServiceConfigurator *SmsServicePlugin::createServiceConfigurator()
{
    return new SmsConfigurator();
}

#include "smsservice.moc"
