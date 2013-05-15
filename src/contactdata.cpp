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

#include "contactdata.h"
#include <QContactPhoneNumber>

ContactData::ContactData(const ContactData &other)
{
    name = other.name;
    firstname = other.firstname;
    lastname = other.lastname;
    nickname = other.nickname;
    phone = other.phone;
    contexts = other.contexts;
    subtypes = other.subtypes;
}

QString displayPhoneType(const ContactData & contact)
{
    if (contact.subtypes.contains(QtMobility::QContactPhoneNumber::SubTypeMobile)) {
        //% "Mobile"
        return qtTrId("contactdata_mobile_phonetype");
    }
    else {
        if (contact.contexts.contains(QtMobility::QContactPhoneNumber::ContextHome)) {
            //% "Home"
            return qtTrId("contactdata_home_phonetype");
        }
        else if (contact.contexts.contains(QtMobility::QContactPhoneNumber::ContextWork)) {
            //% "Work"
            return qtTrId("contactdata_work_phonetype");
        }
        else {
            return "";
        }
    }
}
