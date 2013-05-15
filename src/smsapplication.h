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

#ifndef SMSAPPLICATION_H
#define SMSAPPLICATION_H

#include <MApplication>
#include <MApplicationWindow>

#include "backend.h"
#include "inboxpage.h"
#include "dialogpage.h"

class SmsApplication : public MApplication
{
    Q_OBJECT

public:
        SmsApplication(int &argc, char **argv);
        static SmsApplication *instance();
        Backend *backend();
public slots:
        void showinboxpage();
        void showdialogpage(QString phone="", QString message="");
        void shownewmessagepage();
        void deleteThread(QString);
        void deleteAllThreads();
        void openSettings();
        void cleanExit();
private:
        MApplicationWindow *mWindow;
        Backend *mBackend;
        InboxPage *mInboxpage;
        DialogPage *mDialogpage;
};

#endif // SMSAPPLICATION_H
