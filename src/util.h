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

#ifndef TIMEUTIL_H
#define TIMEUTIL_H

#include <QDateTime>

QString formatTimestamp(const QDateTime&);

QString stripPhone(const QString&);

bool isValidPhone(const QString&);

#endif // TIMEUTIL_H
