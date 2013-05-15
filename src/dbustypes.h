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

#ifndef DBUSTYPES_H
#define DBUSTYPES_H

#include <QMetaType>
#include <QList>
#include <QtDBus>

#include <qmailmessage.h>

struct OfonoPath
{
    QDBusObjectPath     path;
    QVariantMap         properties;
};

Q_DECLARE_METATYPE(OfonoPath)

QDBusArgument &operator<<(QDBusArgument&, const OfonoPath&);
const QDBusArgument &operator>>(const QDBusArgument&, OfonoPath&);

typedef QList<OfonoPath> OfonoPathList;

Q_DECLARE_METATYPE(OfonoPathList)

QDBusArgument &operator<<(QDBusArgument&, const QMailMessage&);
const QDBusArgument &operator>>(const QDBusArgument&, QMailMessage&);

inline void registerMyDataTypes() {
    qDBusRegisterMetaType<OfonoPath>();
    qDBusRegisterMetaType<OfonoPathList>();
    qDBusRegisterMetaType<QMailMessage>();
    qDBusRegisterMetaType<QMailMessageList>();
}

#endif // DBUSTYPES_H
