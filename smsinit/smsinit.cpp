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

#include <QProcess>

#include <qmailstore.h>
#include <qmailnamespace.h>
#include <qmailserviceconfiguration.h>

#include "smsinit.h"

SmsInit::SmsInit(int argc, char **argv) :
    QCoreApplication(argc, argv)
{}

void SmsInit::setupAccount()
{
    QMailStore *mailstore = QMailStore::instance();
    if (!startMessageServer()) {
        qCritical("Could not start messageserver");
    }

    QMailAccountIdList accountids = mailstore->queryAccounts();
    QMailAccountId accountid;
    bool found = false;
    foreach (accountid, accountids) {
        QMailAccount account = QMailAccount(accountid);
        if (account.messageSources().contains("sms", Qt::CaseInsensitive)) {
            found = true;
            break;
        }
    }
    if (!found) {
        qDebug("no sms account found, creating account");
        QMailAccount account;
        account.setName("sms account");
        account.setMessageType(QMailMessage::Sms);
        account.setStatus(QMailAccount::MessageSource, true);
        account.setStatus(QMailAccount::CanRetrieve, true);
        account.setStatus(QMailAccount::Enabled, true);
        QMailAccountConfiguration config;
        config.addServiceConfiguration("qmfstoragemanager");
        QMailAccountConfiguration::ServiceConfiguration &srvcCfg(config.serviceConfiguration("qmfstoragemanager"));
        srvcCfg.setValue("version", "101");
        srvcCfg.setValue("servicetype", "storage");
        srvcCfg.setValue("basePath", "");
        config.addServiceConfiguration("sms");
        QMailAccountConfiguration::ServiceConfiguration *svccfg = &config.serviceConfiguration("sms");
        if (!svccfg) {
            qCritical("setupAccount: serviceConfiguration failed: %d", mailstore->lastError());
            return;
        }
        svccfg->setValue("servicetype","source");
        svccfg->setValue("version","100");
        if (!mailstore->addAccount(&account, &config)) {
            qCritical("setupAccount: addAccount failed: %d", mailstore->lastError());
        }
    }
    quit();
}

bool SmsInit::isMessageServerRunning()
{
    QString lockfile = "messageserver-instance.lock";
    int id = QMail::fileLock(lockfile);
    if (id == -1) {
        return true;
    }
    QMail::fileUnlock(id);
    return false;
}

bool SmsInit::startMessageServer()
{
    // start messageserver if it isn't already running
    if (!isMessageServerRunning()) {
        QProcess *messageserverprocess = new QProcess(this);

        messageserverprocess->startDetached(QMail::messageServerPath() + "/messageserver");
        return messageserverprocess->waitForStarted();
    }
    return true;
}

