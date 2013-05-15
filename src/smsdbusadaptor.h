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

#ifndef SMSDBUSADAPTOR_H
#define SMSDBUSADAPTOR_H

#include <QDBusAbstractAdaptor>
#include "smsapplication.h"

class SmsDBusAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.meego.sms")

public:
    SmsDBusAdaptor(SmsApplication *app);

public slots:
    void showinboxpage();
    void showdialogpage(QString phone="", QString message="");
private:
    SmsApplication *mApp;

};

#endif // SMSDBUSADAPTOR_H
