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

#ifndef SMSINIT_H
#define SMSINIT_H

#include <QCoreApplication>

class SmsInit : public QCoreApplication 
{
    Q_OBJECT
public:
        SmsInit(int, char**);
public slots:
        void setupAccount();
private:
        bool isMessageServerRunning();
        bool startMessageServer();
};

#endif // SMSINIT_H
