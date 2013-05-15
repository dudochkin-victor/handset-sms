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

#ifndef BACKEND_H
#define BACKEND_H

#include <QProcess>
#include <QString>
#include <QMap>
#include <QList>
#include <QDBusInterface>
#include <QDBusPendingCallWatcher>
#include <seaside.h>
#include <seasidesyncmodel.h>

#include <qmailstore.h>
#include <qmailid.h>

#include "smsdata.h"
#include "managerinterface.h"
#include "messagemanagerinterface.h"
#include "dialerinterface.h"

class Backend : public QObject
{
    Q_OBJECT
public:
        Backend();
        ~Backend();
        SeasideSyncModel *seasidemodel();
        QString getName(QString phone);
        QPixmap getAvatar(QString phone);
        void saveDraft(const QString recipient, const QString message);
        void saveDraft(QMailMessageId qid, const QString message);
        void removeDrafts(QString phone);

        QString getDraft(QString recipient, QMailMessageId & draftId);
        QList<SmsData> getSmsHistory(QString phone = "", bool excludeDrafts = false);
        MessageManagerInterface *getMessageManager();
signals:
        void smsAdded(QList<SmsData>);
        void smsModified(QList<SmsData>);
        void smsRemoved(QList<QString>);

public slots:
        void send(const QString recipient, const QString message, QMailMessageId qid = QMailMessageId());
        void call(const QString number);
        void openUrl(const QString url);
        void markRead(QString phone);
        void deleteThread(QString phone);
        void deleteAllThreads();
private slots:
        void getModemsFinished(QDBusPendingCallWatcher*);
        void genericCallHandler(QDBusPendingCallWatcher*);
        void messagesAdded(const QMailMessageIdList&);
        void messagesRemoved(const QMailMessageIdList&);
        void messagesUpdated(const QMailMessageIdList&);

private:
        bool isMessageServerRunning();
        bool startMessageServer();
        void setupAccount();
        QString toUnicodeMessage(const QString& );
        ManagerInterface *mManagerInterface;
        MessageManagerInterface *mMessageManagerInterface;
        DialerInterface *mDialerInterface;
        SeasideSyncModel *mSeasidemodel;
        QMap<QString,QString> mNameindex;
        QMap<QString,QString> mAvatarindex;
        QProcess *mMessageServerProcess;
        QMailStore *mMailStore;
        QMailAccountId mAccountId;
        // map QMailMessageIds to SmsHistory ids,
        // since the latter will not be retrievable from the
        // QMailMessage once we get a messagesRemoved signal
        // from QMailStore
        QHash<QMailMessageId,QString> mMessageIdMap;
};

#endif // BACKEND_H
