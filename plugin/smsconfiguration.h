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

#ifndef SMSCONFIGURATION_H
#define SMSCONFIGURATION_H

#include <qmailserviceconfiguration.h>

class PLUGIN_EXPORT SmsConfiguration : public QMailServiceConfiguration
{
public:
    explicit SmsConfiguration(const QMailAccountConfiguration &config);
    explicit SmsConfiguration(const QMailAccountConfiguration::ServiceConfiguration &svcCfg);
};

class PLUGIN_EXPORT SmsConfigurationEditor : public SmsConfiguration
{
public:
    explicit SmsConfigurationEditor(QMailAccountConfiguration *config);
};

#endif
