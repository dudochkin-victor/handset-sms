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

#include <MMessageBox>

#include "smsapplication.h"
#include "util.h"
#include "settingspage.h"

SmsApplication::SmsApplication(int &argc, char **argv)
    : MApplication(argc, argv)
{
    mWindow = new MApplicationWindow();
    mWindow->show();
    mBackend = new Backend();
    mInboxpage = new InboxPage(mBackend);
    connect(mInboxpage, SIGNAL(startDialog(QString)),
            this, SLOT(showdialogpage(QString)));
    connect(mInboxpage, SIGNAL(startNewMessage()),
            this, SLOT(shownewmessagepage()));
    connect(mInboxpage, SIGNAL(deleteAllThreads()),
            this, SLOT(deleteAllThreads()));
    connect(mInboxpage, SIGNAL(openSettings()),
            this, SLOT(openSettings()));
    connect(this, SIGNAL(aboutToQuit()),
            this, SLOT(cleanExit()));
}

SmsApplication *SmsApplication::instance()
{
    return qobject_cast<SmsApplication*>(MApplication::instance());
}

Backend *SmsApplication::backend()
{
    return mBackend;
}

void SmsApplication::showinboxpage()
{
    if (!mWindow->currentPage()) {
        mInboxpage->appear();
    } else if (mWindow->currentPage() == mDialogpage) {
        mDialogpage->dismiss();
    }
}

void SmsApplication::showdialogpage(QString phone, QString message)
{
    if (mWindow->currentPage() && mWindow->currentPage() == mDialogpage) {
        mDialogpage->dismiss();
    }
    mDialogpage = new DialogPage(mBackend, phone, message);
    connect(mDialogpage, SIGNAL(backButtonClicked()),
            this, SLOT(showinboxpage()));
    connect(mDialogpage, SIGNAL(deleteThread(QString)),
            this, SLOT(deleteThread(QString)));
    mDialogpage->appear(MSceneWindow::DestroyWhenDismissed);
}

void SmsApplication::shownewmessagepage()
{
    if (mWindow->currentPage() && mWindow->currentPage() == mDialogpage) {
        mDialogpage->dismiss();
    }
    mDialogpage = new DialogPage(mBackend);
    connect(mDialogpage, SIGNAL(backButtonClicked()),
            this, SLOT(showinboxpage()));
    connect(mDialogpage, SIGNAL(deleteThread(QString)),
            this, SLOT(deleteThread(QString)));
    mDialogpage->appear(MSceneWindow::DestroyWhenDismissed);
}

void SmsApplication::openSettings()
{
    SettingsPage *settings = new SettingsPage(mBackend);
    settings->appear(MSceneWindow::DestroyWhenDismissed);
}

void SmsApplication::deleteThread(QString phone)
{
    mBackend->deleteThread(phone);
}

void SmsApplication::deleteAllThreads()
{
    mBackend->deleteAllThreads();
}

void SmsApplication::cleanExit()
{
    //Save the text as Draft if any.
    if (mWindow->currentPage() == mDialogpage) {
        delete mDialogpage;
    }
}
