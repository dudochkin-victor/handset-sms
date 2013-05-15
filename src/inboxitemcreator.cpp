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

#include "inboxitemcreator.h"
#include "inboxitem.h"
#include "smsthread.h"

InboxItemCreator::InboxItemCreator()
{}

void InboxItemCreator::updateCell(const QModelIndex &index, MWidget *cell) const
{
    if (!index.isValid() || !cell)
        return;
    InboxItem *item = qobject_cast<InboxItem *>(cell);
    QVariant data = index.data(Qt::DisplayRole);
    SmsThread rowData = data.value<SmsThread>();
    SmsData &first = rowData.first();
    item->setData(first.sender,
                  first.message,
                  first.datetime,
                  first.calltype,
                  first.draft,
                  rowData.newMsgCount());
}
