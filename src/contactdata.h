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

#ifndef CONTACTDATA_H
#define CONTACTDATA_H

#include <QMetaType>
#include <QString>
#include <QStringList>


class ContactData
{
public:
    ContactData() {}
    ContactData(const ContactData&);

    QString name;
    QString firstname;
    QString lastname;
    QString nickname;
    QString phone;
    QStringList contexts;
    QStringList subtypes;
};

Q_DECLARE_METATYPE(ContactData)

QString displayPhoneType(const ContactData & contact);

#endif // CONTACTDATA_H
