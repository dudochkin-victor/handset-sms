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

#ifndef DIALOGITEMSTYLE_H
#define DIALOGITEMSTYLE_H

#include <MWidgetStyle>

class M_EXPORT DialogItemStyle : public MWidgetStyle
{
    Q_OBJECT
    M_STYLE(DialogItemStyle)
};

class M_EXPORT DialogItemStyleContainer: public MWidgetStyleContainer
{
    M_STYLE_CONTAINER(DialogItemStyle)
    M_STYLE_MODE(Indented)
};

#endif // DIALOGITEMSTYLE_H
