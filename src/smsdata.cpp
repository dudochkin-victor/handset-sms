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

#include "smsdata.h"

SmsData::SmsData(const QMailMessage &qmessage)
{
    id = qmessage.customField("id");
    status = PENDING;
    if (qmessage.customField("status") == "SENT") {
        status = SENT;
    } else if (qmessage.customField("status") == "FAILED") {
        status = FAILED;
    } else if (qmessage.customField("status") == "DELIVERED") {
        status = DELIVERED;
    } else if (qmessage.customField("status") == "DELIVER_FAILED") {
        status = DELIVER_FAILED;
    }

    if (qmessage.status() & QMailMessage::Incoming) {
        calltype = INCOMING;
    } else {
        calltype = OUTGOING;
    }
    sender = qmessage.from().toString();
    message = qmessage.body().data();
    datetime = qmessage.date().toLocalTime();
    if (qmessage.status() & QMailMessage::Draft) {
        draft = true;
    } else {
        draft = false;
    }
    if (qmessage.status() & QMailMessage::Read) {
        unread = false;
    } else {
        unread = true;
    }
}

SmsData::SmsData(const SmsData &other)
{
    id = other.id;
    status = other.status;
    calltype = other.calltype;
    sender = other.sender;
    message = other.message;
    datetime = other.datetime;
    draft = other.draft;
    unread = other.unread;
}

bool SmsData::operator<(const SmsData &other) const
{
    // new messages > draft messages > read messages
    // messages of same class sorted by date
    if (!unread && other.unread) {
        return true;
    } else if (unread && !other.unread) {
        return false;
    } else if (!draft && other.draft) {
        return true;
    } else if (draft && !other.draft) {
        return false;
    } else {
        return datetime < other.datetime;
    }
}

bool SmsData::operator==(const SmsData &other) const
{
    return sender == other.sender;
}
