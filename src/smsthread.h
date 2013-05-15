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

#ifndef SMSTHREAD_H
#define SMSTHREAD_H

#include <QMetaType>
#include <QList>

#include "smsdata.h"

class SmsThread : public QList<SmsData>
{
public:
    SmsThread();
    SmsThread(const SmsData&);
    void insertSorted(const SmsData &dat);
    int newMsgCount();
    bool operator<(const SmsThread &other) const;
};

Q_DECLARE_METATYPE(SmsThread)

#endif // SMSTHREAD_H
