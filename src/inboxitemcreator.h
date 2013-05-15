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

#include <mabstractcellcreator.h>

#include "inboxitem.h"

class InboxItemCreator : public MAbstractCellCreator<InboxItem>
{
public:
    InboxItemCreator();
    void updateCell(const QModelIndex &index, MWidget *cell) const;
};
