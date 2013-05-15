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

#include <MWidgetCreator>

#include "dialoglist.h"

M_REGISTER_WIDGET(DialogList)

DialogList::DialogList(MWidget *parent)
    :MStylableWidget(parent)
{
    setObjectName("dialogList");
    mLayout = new MLayout();
    mPolicy = new MLinearLayoutPolicy(mLayout, Qt::Vertical);
    mLayout->setPolicy(mPolicy);
    setLayout(mLayout);
}

void DialogList::addItem(QGraphicsLayoutItem *item)
{
    mPolicy->addItem(item);
}

void DialogList::insertItem(int index, QGraphicsLayoutItem *item)
{
    mPolicy->insertItem(index, item);
}

void DialogList::removeAt(int index)
{
    mPolicy->removeAt(index);
}
