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

#include <QRegExp>
#include "util.h"

QString formatTimestamp(const QDateTime &datetime)
{
    // FIXME: this function probably requires more extensive i18n
    QDateTime now = QDateTime::currentDateTime();

    if (datetime > now) {
        qWarning("timestamp is in the future");
        //% "M/dd/yy 'at' h:mmap"
        return datetime.toString(qtTrId("util_timestamp_future"));
    }

    if (datetime.date() == now.date()) {
        //% "'today at' h:mmap"
        return datetime.toString(qtTrId("util_timestamp_today"));
    } else if (datetime.date().addDays(1) == now.date()) {
        //% "'yesterday at' h:mmap"
        return datetime.toString(qtTrId("util_timestamp_yesterday"));
    } else if (datetime.date().addDays(7) > now.date()) {
        //% "dddd 'at' h:mmap"
        return datetime.toString(qtTrId("util_timestamp_thisweek"));
    } else {
        //% "M/dd/yy 'at' h:mmap"
        return datetime.toString(qtTrId("util_timestamp_default"));
    }
}

QString stripPhone(const QString &phone)
{
    QString copy = phone;
    copy.replace(QRegExp("[^0-9+*#]"), "");
    return copy;
}

bool isValidPhone(const QString &phone)
{
    return phone.contains(QRegExp("[0-9]")) && !(phone.contains(QRegExp("[^0-9+*#\\(\\) -]")));
}
