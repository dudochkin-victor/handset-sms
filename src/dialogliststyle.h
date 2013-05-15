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

#ifndef DIALOGLISTSTYLE_H
#define DIALOGLISTSTYLE_H

#include <MWidgetStyle>

class M_EXPORT DialogListStyle : public MWidgetStyle
{
    Q_OBJECT
    M_STYLE(DialogListStyle)
};

class M_EXPORT DialogListStyleContainer: public MWidgetStyleContainer
{
    M_STYLE_CONTAINER(DialogListStyle)
};

#endif // DIALOGLISTSTYLE_H
