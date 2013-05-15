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

#ifndef SMSSETTINGS_H
#define SMSSETTINGS_H

#include <QPushButton>

#include <qmailmessageservice.h>

class SmsSettings : public QMailMessageServiceEditor
{
    Q_OBJECT

public:
    SmsSettings();

    void displayConfiguration(const QMailAccount &account, const QMailAccountConfiguration &config);
    bool updateAccount(QMailAccount *account, QMailAccountConfiguration *config);
private:
    //QPushButton *mButton;
};

#endif  // SMSSETTINGS_H
