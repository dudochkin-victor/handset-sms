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

#ifndef SMSSERVICE_H
#define SMSSERVICE_H

#include <QList>
#include <QDBusInterface>

#include <qmailmessage.h>

#include <QContactManager>

#include <MNotification>

#include <qmailmessageservice.h>

#include "smshistoryinterface.h"
#include "managerinterface.h"
#include "messagemanagerinterface.h"
#include "cellbroadcastinterface.h"

using namespace QtMobility;

class SmsService : public QMailMessageService
{
    Q_OBJECT

public:
    SmsService(const QMailAccountId &accountId);
    ~SmsService();

    virtual QString service() const;
    virtual QMailAccountId accountId() const;
    virtual bool available() const;

    virtual bool hasSource() const;
    virtual QMailMessageSource &source() const;
public slots:
    virtual bool cancelOperation(QMailServiceAction::Status::ErrorCode, const QString&);
private slots:
    void messagesUpdated(const QMailMessageIdList&);
    void messagesRemoved(const QMailMessageIdList&);
    void getModemsFinished(QDBusPendingCallWatcher*);
    void smsHistoryChanged();
    void smsStatusChanged(const QString&, ushort, int);
    void getSmsHistoryFinished(QDBusPendingCallWatcher*);
    void showImmediateMessage(const QString&, const QVariantMap&);
    void emergencyBroadcast(const QString&, const QVariantMap&);
    void incomingBroadcast(const QString&, ushort);
private:
    class Source;
    friend class Source;

    QContactManager *mContactManager;
    QHash<QMailMessageId,MNotification> mNotifications;
    QHash<QString,ushort> mStatusBacklog;
    SmsHistoryInterface *mSmsHistoryInterface;
    ManagerInterface *mManagerInterface;
    CellBroadcastInterface *mCellBroadcastInterface;
    MessageManagerInterface *mMessageManagerInterface;
    QMailAccountId mAccountId;
    Source *mSource;
};

class SmsServicePlugin : public QMailMessageServicePlugin
{
    Q_OBJECT

public:
    SmsServicePlugin();

    virtual QString key() const;
    virtual bool supports(QMailMessageServiceFactory::ServiceType type) const;
    virtual bool supports(QMailMessage::MessageType type) const;
    virtual QMailMessageService *createService(const QMailAccountId &id);
    virtual QMailMessageServiceConfigurator *createServiceConfigurator();
};

#endif  // SMSSERVICE_H
