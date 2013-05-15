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

#ifndef INBOXITEMMODEL_H
#define INBOXITEMMODEL_H

#include <QAbstractListModel>

#include "backend.h"
#include "smsthread.h"

class InboxItemModel : public QAbstractListModel
{
    Q_OBJECT
public:
    InboxItemModel(Backend*);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
public slots:
    void sms_added(QList<SmsData>);
    void sms_modified(QList<SmsData>);
    void sms_removed(QList<QString>);
private:
    Backend *mBackend;
    QList<SmsThread> mModel;
};

#endif // INBOXITEMMODEL_H
