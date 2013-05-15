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

#ifndef CONTACTITEMSTYLE_H
#define CONTACTITEMSTYLE_H

#include <MWidgetStyle>

class M_EXPORT ContactItemStyle : public MWidgetStyle
{
    Q_OBJECT
    M_STYLE(ContactItemStyle)

};

class M_EXPORT ContactItemStyleContainer : public MWidgetStyleContainer
{
    M_STYLE_CONTAINER(ContactItemStyle)
    M_STYLE_MODE(Pressed)
};

#endif // CONTACTITEMSTYLE_H
