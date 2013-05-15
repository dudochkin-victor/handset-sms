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

#ifndef INBOXITEMSTYLE_H
#define INBOXITEMSTYLE_H

#include <MWidgetStyle>

class M_EXPORT InboxItemStyle : public MWidgetStyle
{
    Q_OBJECT
    M_STYLE(InboxItemStyle)

};

class M_EXPORT InboxItemStyleContainer : public MWidgetStyleContainer
{
    M_STYLE_CONTAINER(InboxItemStyle)
    M_STYLE_MODE(Read)
    M_STYLE_MODE(Pressed)
};

#endif // INBOXITEMSTYLE_H
