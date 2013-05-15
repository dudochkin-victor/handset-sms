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

#include <QTimer>

#include "smsinit.h"

int main(int argc, char** argv)
{
    SmsInit *app = new SmsInit(argc, argv);

    QTimer::singleShot(0, app, SLOT(setupAccount()));

    return app->exec();
}

