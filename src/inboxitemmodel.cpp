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


#include "inboxitemmodel.h"

InboxItemModel::InboxItemModel(Backend *backend) :
    mBackend(backend)
{
    SmsData dat;
    foreach (dat, mBackend->getSmsHistory()) {
        bool found = false;
        int i;
        for (i = 0; i < mModel.size(); i++) {
            if (mModel[i].first().sender == dat.sender) {
                mModel[i].insertSorted(dat);
                found = true;
                break;
            }
        }
        if (!found) {
            mModel.append(SmsThread(dat));
        }
    }
    qSort(mModel);

    connect(mBackend, SIGNAL(smsAdded(QList<SmsData>)), this, SLOT(sms_added(QList<SmsData>)));
    connect(mBackend, SIGNAL(smsModified(QList<SmsData>)), this, SLOT(sms_modified(QList<SmsData>)));
    connect(mBackend, SIGNAL(smsRemoved(QList<QString>)), this, SLOT(sms_removed(QList<QString>)));
}

void InboxItemModel::sms_added(QList<SmsData> messages)
{
    SmsData dat;
    foreach (dat, messages) {
        int i;
        bool found = false;
        for (i = 0; i < mModel.size(); i++) {
            if (mModel[i].first().sender == dat.sender) {
                mModel[i].insertSorted(dat);
                emit dataChanged(index(i), index(i));
                found = true;
                break;
            }
        }
        if (!found) {
            beginInsertRows(QModelIndex(), mModel.size(), mModel.size());
            mModel.append(SmsThread(dat));
            endInsertRows();
        }
    }
    emit layoutAboutToBeChanged();
    qSort(mModel);
    emit layoutChanged();
}

void InboxItemModel::sms_modified(QList<SmsData> messages)
{
    SmsData dat;
    foreach (dat, messages) {
        int i, j;
        for (i = 0; i < mModel.size(); i++) {
            if (mModel[i].first().sender == dat.sender) {
                for (j = 0; j < mModel[i].size(); j++) {
                    if (mModel[i][j].id == dat.id) {
                        mModel[i].removeAt(j);
                        mModel[i].insertSorted(dat);
                        emit dataChanged(index(i), index(i));
                        break;
                    }
                }
            }
        }
    }
    emit layoutAboutToBeChanged();
    qSort(mModel);
    emit layoutChanged();
}

void InboxItemModel::sms_removed(QList<QString> ids)
{
    QString id;
    foreach (id, ids) {
        int i, j;
        for (i = 0; i < mModel.size(); i++) {
            for (j = 0; j < mModel[i].size(); j++) {
                if (mModel[i][j].id == id) {
                    // removing one msg or whole thread?
                    if (mModel[i].size() > 1) {
                        mModel[i].removeAt(j);
                        emit dataChanged(index(i), index(i));
                    } else {
                        beginRemoveRows(QModelIndex(), i, i);
                        mModel.removeAt(i);
                        endRemoveRows();
                    }
                    break;
                }
            }
        }
    }
    emit layoutAboutToBeChanged();
    qSort(mModel);
    emit layoutChanged();
}

int InboxItemModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mModel.size();
}

QVariant InboxItemModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        if (index.isValid() && index.row() < mModel.size()) {
            return QVariant::fromValue<SmsThread>(mModel.at(index.row()));
        }
    }
    return QVariant();
}

