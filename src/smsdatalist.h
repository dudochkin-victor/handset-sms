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

#ifndef SMSDATALIST_H
#define SMSDATALIST_H

#include <QMetaType>
#include <QList>
#include <QtDBus>

#include "smsdata.h"

typedef QList<SmsData> SmsDataList;

Q_DECLARE_METATYPE(SmsDataList)

QDBusArgument &operator<<(QDBusArgument&, const SmsDataList&);
const QDBusArgument &operator>>(const QDBusArgument&, SmsDataList&);

inline void registerMyDataTypes() {
    qDBusRegisterMetaType<SmsDataList>();
}

#endif // SMSDATALIST_H
