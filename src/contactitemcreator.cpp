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

#include "contactitemcreator.h"
#include "contactdata.h"

ContactItemCreator::ContactItemCreator()
{}

void ContactItemCreator::updateCell(const QModelIndex &index, MWidget *cell) const
{
    if (!index.isValid() || !cell)
        return;
    ContactItem *item = qobject_cast<ContactItem*>(cell);
    QVariant data = index.data(Qt::DisplayRole);
    ContactData dat = data.value<ContactData>();
    //item->setPixmap(QPixmap(THEMEDIR"images/sms-default-photo-small.png"));
    //item->setPixmap(QPixmap("/usr/share/themes/base/m/seaside/images/" + dat.avatar));
    item->setName(dat.name);
    item->setPhone(dat.phone);
    item->setSubtypes(dat.subtypes);
}
