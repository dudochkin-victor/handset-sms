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

#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include <MApplicationPage>
#include <MButton>
#include <MTextEdit>

#include "backend.h"

class SettingsPage : public MApplicationPage
{
    Q_OBJECT
public:
        SettingsPage(Backend*);
        virtual ~SettingsPage();
        virtual void createContent();
private slots:
        void getPropertiesFinished(QDBusPendingCallWatcher*);
private:
        MButton *mDeliveryButton;
        MTextEdit *mSmscEdit;
        MessageManagerInterface *mMessageManagerInterface;
};

#endif // SETTINGSPAGE_H
