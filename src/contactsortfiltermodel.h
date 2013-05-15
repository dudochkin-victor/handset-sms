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

#ifndef CONTACTSORTFILTERMODEL_H
#define CONTACTSORTFILTERMODEL_H

#include <QSortFilterProxyModel>

class ContactSortFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    ContactSortFilterModel();
    virtual ~ContactSortFilterModel();

    void setContext(const QString &);
    void setSearchText(const QString &);

protected:
    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
    virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

private:
    QString mContext;
    QString mSearchText;
};

#endif // CONTACTSORTFILTERMODEL_H
