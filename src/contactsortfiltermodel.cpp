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

#include <QDebug>

#include "contactsortfiltermodel.h"
#include "contactitemmodel.h"
#include "contactdata.h"
#include "util.h"

ContactSortFilterModel::ContactSortFilterModel()
{}

ContactSortFilterModel::~ContactSortFilterModel()
{}

bool ContactSortFilterModel::filterAcceptsRow(int source_row,
                                              const QModelIndex &source_parent) const
{
    // or phone number substrings
    Q_UNUSED(source_parent);

    const ContactItemModel *model = dynamic_cast<const ContactItemModel*>(sourceModel());
    const ContactData dat = model->data(model->index(source_row, 0)).value<ContactData>();

    bool isAccepted = false;

    // Assume pattern is the start of a name (first or last).  Match
    // at the beginning of word boundaries

    if (!mSearchText.isEmpty())
    {
        const QString namePattern = ("\\b" + QRegExp::escape(mSearchText));
        const QRegExp nameRex(namePattern, Qt::CaseInsensitive);
        const bool nameMatches = dat.name.contains(nameRex);
        qDebug() << "Name:" << dat.name
                 << ", search:" << mSearchText << ", result:" << nameMatches;

        QString contextPattern = mContext;
        contextPattern.remove(QRegExp("^!! "));
        contextPattern = ("\\b" + QRegExp::escape(contextPattern));
        const QRegExp contextRex(contextPattern, Qt::CaseInsensitive);
        QString context = displayPhoneType(dat);
        context.remove(QRegExp("^!! "));

        const bool contextMatches = context.contains(contextRex);
        qDebug() << "Contexts:" << context
                 << ", search:" << contextPattern << ", result:" << contextMatches;

        isAccepted = nameMatches && contextMatches;
    }

    if (!isAccepted) {

        // Assume pattern is a phone number fragment

        const QString phonePattern = stripPhone(mSearchText);
        if (!phonePattern.isEmpty())
        {
            const QRegExp rex(phonePattern, Qt::CaseInsensitive, QRegExp::FixedString);
            const QString phone = stripPhone(dat.phone);
            isAccepted = phone.contains(rex);

            qDebug() << "Phone:" << dat.phone
                     << ", search:" << mSearchText << ", result:" << isAccepted;
        }
    }

    return isAccepted;
}

bool ContactSortFilterModel::lessThan(const QModelIndex &left,
                                      const QModelIndex &right) const
{
    int cmp = QString::localeAwareCompare(left.data().value<ContactData>().name,
                                          right.data().value<ContactData>().name);
//    if (cmp == 0) {
//        cmp = QString::localeAwareCompare(left.data().value<ContactData>().displayPhoneType(),
//                                          right.data().value<ContactData>().displayPhoneType());
//    }
    return (cmp < 0);
}

void ContactSortFilterModel::setContext(const QString & s) {
    mContext = s.trimmed();
}

void ContactSortFilterModel::setSearchText(const QString & s) {
    mSearchText = s.trimmed();
}
