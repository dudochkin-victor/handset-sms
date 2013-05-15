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

#include <QContactManager>
#include <QContactName>
#include <QContactNickname>
#include <QContactPhoneNumber>

#include "contactitemmodel.h"

ContactItemModel::ContactItemModel(Backend *backend) :
    mBackend(backend)
{
    QContactManager *manager = mBackend->seasidemodel()->manager();
    QList<QContactSortOrder> orders;
    QContactSortOrder firstnameorder;
    firstnameorder.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldFirstName);
    QContactSortOrder lastnameorder;
    lastnameorder.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldLastName);
    orders << firstnameorder << lastnameorder;
    int i, j;
    QList<QContact> contacts = manager->contacts(orders);
    ContactData dat;
    for (i = 0; i < contacts.size(); i++) {
        QContactDetail name = contacts[i].detail(QContactName::DefinitionName);
        QContactDetail nickname = contacts[i].detail(QContactNickname::DefinitionName);
        QList<QContactDetail> phones = contacts[i].details(QContactPhoneNumber::DefinitionName);
        for (j = 0; j < phones.size(); j++) {
            dat.name = contacts[i].displayLabel();
            dat.firstname = static_cast<QContactName>(name).firstName();
            dat.lastname = static_cast<QContactName>(name).lastName();
            dat.nickname = static_cast<QContactNickname>(nickname).nickname();
            dat.phone = static_cast<QContactPhoneNumber>(phones[j]).number();
            dat.contexts = static_cast<QContactPhoneNumber>(phones[j]).contexts();
            dat.subtypes = static_cast<QContactPhoneNumber>(phones[j]).subTypes();
            mModel.append(dat);
        }
    }

    connect(manager, SIGNAL(contactsAdded(const QList<QContactLocalId>&)),
                this, SLOT(addContacts(const QList<QContactLocalId>&)));
    connect(manager, SIGNAL(contactsChanged(const QList<QContactLocalId>&)),
                this, SLOT(changeContacts(const QList<QContactLocalId>&)));
    connect(manager, SIGNAL(contactsRemoved(const QList<QContactLocalId>&)),
                this, SLOT(removeContacts(const QList<QContactLocalId>&)));
}

int ContactItemModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mModel.size();
}

QVariant ContactItemModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        if (index.isValid() && index.row() < mModel.size()) {
            return QVariant::fromValue<ContactData>(mModel.at(index.row()));
        }
    }
    return QVariant();
}

void ContactItemModel::addContacts(const QList<QContactLocalId> &contactIds)
{
    Q_UNUSED(contactIds);
    qDebug("addContacts");
    // FIXME
}

void ContactItemModel::changeContacts(const QList<QContactLocalId> &contactIds)
{
    Q_UNUSED(contactIds);
    qDebug("changeContacts");
    // FIXME
}

void ContactItemModel::removeContacts(const QList<QContactLocalId> &contactIds)
{
    Q_UNUSED(contactIds);
    qDebug("removeContacts");
    // FIXME
}
