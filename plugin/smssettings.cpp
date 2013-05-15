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

#include "smssettings.h"
#include "smsconfiguration.h"

namespace { const QString serviceKey("sms"); }

SmsSettings::SmsSettings()
    : QMailMessageServiceEditor()
{
    //mButton = new QPushButton("test", this);
}

void SmsSettings::displayConfiguration(const QMailAccount &account, const QMailAccountConfiguration &config)
{
    Q_UNUSED(account);
    Q_UNUSED(config);
}

bool SmsSettings::updateAccount(QMailAccount *account, QMailAccountConfiguration *config)
{
    if (!config->services().contains(serviceKey))
        config->addServiceConfiguration(serviceKey);
    SmsConfigurationEditor smsConfig(config);
    smsConfig.setVersion(100);
    smsConfig.setType(QMailServiceConfiguration::Source);

    account->setStatus(QMailAccount::CanCreateFolders, false);
    account->setStatus(QMailAccount::CanRetrieve, true);
    return true;
}
