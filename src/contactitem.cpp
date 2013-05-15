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

#include <QGraphicsGridLayout>
#include <QGraphicsSceneEvent>
#include <MCancelEvent>
#include <MWidgetCreator>

#include "contactitem.h"
#include "util.h"

M_REGISTER_WIDGET(ContactItem)

ContactItem::ContactItem(MWidget *parent)
    :MStylableWidget(parent),
    mPressed(false)
{
    setObjectName("contactItem");
    QGraphicsGridLayout *layout = new QGraphicsGridLayout();
    setLayout(layout);

    mAvatar = new MImageWidget(new QPixmap());
    mAvatar->setObjectName("contactItemAvatar");
    mAvatar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    layout->addItem(mAvatar, 0, 0, 2, 1, Qt::AlignTop);

    mNameLabel = new MLabel("");
    mNameLabel->setTextElide(true);
    mNameLabel->setObjectName("contactItemName");
    mNameLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    layout->addItem(mNameLabel, 0, 1);

    mPhoneLabel = new MLabel("");
    mPhoneLabel->setObjectName("contactItemPhone");
    layout->addItem(mPhoneLabel, 1, 1);
}

void ContactItem::setName(QString name)
{
    mNameLabel->setText(name);
}

void ContactItem::setPhone(QString phone)
{
    mAvatar->setPixmap(SmsApplication::instance()->backend()->getAvatar(stripPhone(phone)));
    mPhone = phone;
    generatePhoneLabel();
}

void ContactItem::setSubtypes(QStringList subtypes)
{
    mSubtypes = subtypes;
    generatePhoneLabel();
}

void ContactItem::generatePhoneLabel()
{
    mPhoneLabel->setText(mPhone + " (" + mSubtypes.join(", ") + ")");
}

void ContactItem::mousePressEvent(QGraphicsSceneMouseEvent *ev)
{
    mPressed = true;
    style().setModePressed();
    update();
    ev->accept();
}

void ContactItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *ev)
{
    if (mPressed) {
        mPressed = false;
        style().setModeDefault();
        update();
        ev->accept();
        emit clicked();
    } else {
        ev->ignore();
    }
}

void ContactItem::cancelEvent(MCancelEvent *ev)
{
    mPressed = false;
    style().setModeDefault();
    update();
    ev->accept();
}
