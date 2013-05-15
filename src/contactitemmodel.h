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

#ifndef CONTACTITEMMODEL_H
#define CONTACTITEMMODEL_H

#include <QAbstractListModel>

#include "backend.h"
#include "contactdata.h"

class ContactItemModel : public QAbstractListModel
{
    Q_OBJECT
public:
    ContactItemModel(Backend*);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
public slots:
    void addContacts(const QList<QContactLocalId> &contactIds);
    void changeContacts(const QList<QContactLocalId> &contactIds);
    void removeContacts(const QList<QContactLocalId> &contactIds);
private:
    Backend *mBackend;
    QList<ContactData> mModel;
};

#endif // CONTACTITEMMODEL_H
