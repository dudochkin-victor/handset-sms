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

#include "smsthread.h"

SmsThread::SmsThread()
{}

SmsThread::SmsThread(const SmsData &dat)
{
    prepend(dat);
}

void SmsThread::insertSorted(const SmsData &dat)
{
    // FIXME: a heap would be more efficient than keeping a sorted list,
    // but Qt doesnt seem to provide one
    prepend(dat);
    qSort(this->begin(), this->end(), qGreater<SmsData>());
}

int SmsThread::newMsgCount()
{
    int count = 0;
    SmsData dat;
    foreach (dat, *this) {
        if (dat.unread) count++;
    }
    return count;
}

bool SmsThread::operator<(const SmsThread &other) const
{
    // threads shouldn't be empty in practice
    if (isEmpty() || other.isEmpty()) {
        qWarning("comparing empty threads");
        return false;
    }
    else
        return other.first() < first();
}
