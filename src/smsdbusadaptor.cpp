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

#include "smsdbusadaptor.h"

SmsDBusAdaptor::SmsDBusAdaptor(SmsApplication *app) :
    QDBusAbstractAdaptor(app), mApp(app) 
{}

void SmsDBusAdaptor::showinboxpage()
{
    mApp->showinboxpage();
}

void SmsDBusAdaptor::showdialogpage(QString phone, QString message)
{
    mApp->showdialogpage(phone, message);
}
