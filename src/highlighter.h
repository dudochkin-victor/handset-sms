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

#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <MLabel>
#include <QDebug>

class Highlighter : public QObject
{
    Q_OBJECT;

public:
    Highlighter (): QObject() {}
    ~Highlighter () {}

    void enable (MLabel * pLabel);

public slots:
    void phoneLinkPressed (QString phone);
    void urlLinkPressed (QString url);
};


#endif // HIGHLIGHTER_H
