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

#ifndef SMSDATA_H
#define SMSDATA_H

#include <QMetaType>
#include <QString>
#include <QDateTime>

#include <qmailmessage.h>

class SmsData
{
public:
    SmsData() {}
    SmsData(const QMailMessage&);
    SmsData(const SmsData&);
    bool operator<(const SmsData &other) const;
    bool operator==(const SmsData &other) const;

    enum Status {
        PENDING = 0,
        FAILED,
        SENT,
        DELIVERED,
        DELIVER_FAILED
    };

    enum Calltype {
        OUTGOING = 0,
        INCOMING,
    };

    QString id; // SMS id from sms history
    Status status;
    Calltype calltype;
    QString sender;
    QString message;
    QDateTime datetime;
    bool draft;
    bool unread;
};

Q_DECLARE_METATYPE(SmsData)

#endif // SMSDATA_H
