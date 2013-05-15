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

#ifndef DIALOGLIST_H
#define DIALOGLIST_H

#include <MStylableWidget>
#include <MLayout>
#include <MLinearLayoutPolicy>

#include "dialogliststyle.h"


class M_EXPORT DialogList : public MStylableWidget
{
    Q_OBJECT
public:
        DialogList(MWidget *parent = NULL);
        virtual ~DialogList() {}

        void addItem(QGraphicsLayoutItem *item);
        void insertItem(int index, QGraphicsLayoutItem *item);
        void removeAt(int index);

private:
        MLayout *mLayout;
        MLinearLayoutPolicy *mPolicy;

        M_STYLABLE_WIDGET(DialogListStyle)
};

#endif // DIALOGLIST_H

