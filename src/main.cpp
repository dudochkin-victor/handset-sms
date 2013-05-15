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

#include <MApplicationWindow>

#include "smsapplication.h"
#include "smsdbusadaptor.h"

int main(int argc, char** argv)
{
    SmsApplication *app = new SmsApplication(argc, argv);

    new SmsDBusAdaptor(app);
    QDBusConnection::sessionBus().registerObject("/", app);
    QDBusConnection::sessionBus().registerService("com.meego.sms");

    app->showinboxpage();

    return app->exec();
}
