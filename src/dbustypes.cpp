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


#include "dbustypes.h"
#include "smsdata.h"

QDBusArgument &operator<<(QDBusArgument &arg, const OfonoPath &path)
{
    arg.beginStructure();
    arg << path.path << path.properties;
    arg.endStructure();
    return arg;
}

const QDBusArgument &operator>>(const QDBusArgument &arg, OfonoPath &path)
{
    arg.beginStructure();
    arg >> path.path >> path.properties;
    arg.endStructure();
    return arg;
}

QDBusArgument &operator<<(QDBusArgument &arg, const QMailMessage &msg)
{
    Q_UNUSED(msg);
    // currently unused
    arg.beginStructure();
    arg.endStructure();
    return arg;
}

const QDBusArgument &operator>>(const QDBusArgument &arg, QMailMessage &msg)
{
    QString id;
    unsigned short calltype, status;
    QString sender, message;
    int starttime, endtime;

    arg.beginStructure();
    msg.setMessageType(QMailMessage::Sms);
    arg >> id;
    msg.setCustomField("id", id);
    arg >> calltype;
    if (calltype == SmsData::INCOMING) {
        msg.setStatus(QMailMessage::Incoming, true);
        msg.setStatus(QMailMessage::Outgoing, false);
    } else {
        msg.setStatus(QMailMessage::Incoming, false);
        msg.setStatus(QMailMessage::Outgoing, true);
    }
    arg >> status;
    switch (status) {
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
        default:
            msg.setCustomField("status", "FAILED");
            break;
    }
    arg >> sender;
    msg.setFrom(QMailAddress(sender));
    arg >> message;
    //msg.setSubject(message);
    msg.setBody(QMailMessageBody::fromData(message, QMailMessageContentType("text/plain"), QMailMessageBody::NoEncoding));
    arg >> starttime;

    if(calltype == SmsData::INCOMING)
        msg.setDate(QMailTimeStamp(QDateTime::currentDateTime()));
    else
        msg.setDate(QMailTimeStamp(QDateTime::fromTime_t(starttime)));

    arg >> endtime; // not currently used
    if (calltype == SmsData::INCOMING)
        msg.setStatus(QMailMessage::Read, false);
    else
        msg.setStatus(QMailMessage::Read, true);
    arg.endStructure();
    return arg;
}
