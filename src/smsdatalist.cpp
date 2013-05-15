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


#include "smsdatalist.h"

QDBusArgument &operator<<(QDBusArgument &arg, const SmsDataList &list)
{
    SmsData dat;
    arg.beginArray(list.size());
    foreach(dat, list) {
        arg.beginStructure();
        arg << dat.id << dat.calltype << dat.status << dat.sender << dat.message << dat.datetime.toTime_t() << dat.datetime.toTime_t();
        arg.endStructure();
    }
    arg.endArray();
    return arg;
}

const QDBusArgument &operator>>(const QDBusArgument &arg, SmsDataList &list)
{
    arg.beginArray();
    while (!arg.atEnd()) {
        SmsData dat;
        unsigned short calltype, status;
        int starttime, endtime;
        arg.beginStructure();
        arg >> dat.id;
        arg >> calltype;
        dat.calltype = static_cast<SmsData::Calltype>(calltype);
        arg >> status;
        dat.status = static_cast<SmsData::Status>(status);
        arg >> dat.sender >> dat.message;
        arg >> starttime;
        arg >> endtime; // not currently used
        dat.datetime = QDateTime::fromTime_t(starttime);
        if (dat.calltype == SmsData::INCOMING)
            dat.unread = true;
        else
            dat.unread = false;
        list.append(dat);
        arg.endStructure();
    }
    arg.endArray();
    return arg;
}
