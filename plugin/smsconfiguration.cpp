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


#include "smsconfiguration.h"

SmsConfiguration::SmsConfiguration(const QMailAccountConfiguration &config)
    : QMailServiceConfiguration(config, "sms")
{
}

SmsConfiguration::SmsConfiguration(const QMailAccountConfiguration::ServiceConfiguration &svcCfg)
    : QMailServiceConfiguration(svcCfg)
{
}

SmsConfigurationEditor::SmsConfigurationEditor(QMailAccountConfiguration *config)
    : SmsConfiguration(*config)
{
}

