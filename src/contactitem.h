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

#ifndef CONTACTITEM_H
#define CONTACTITEM_H

#include <MStylableWidget>
#include <MLayout>
#include <MLabel>
#include <MImageWidget>

#include "contactitemstyle.h"
#include "smsapplication.h"

class M_EXPORT ContactItem : public MStylableWidget
{
    Q_OBJECT
public:
        ContactItem(MWidget *parent = NULL);
        virtual ~ContactItem() {}

        void setName(QString name);
        void setPhone(QString phone);
        void setSubtypes(QStringList subtypes);

protected:
        virtual void mousePressEvent(QGraphicsSceneMouseEvent*);
        virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent*);
        virtual void cancelEvent(MCancelEvent*);

signals:
        void clicked();

private:
        MLayout *mLayout;
        MImageWidget *mAvatar;
        MLabel *mNameLabel;
        MLabel *mPhoneLabel;

        bool mPressed;
        QString mPhone;
        QStringList mSubtypes;

        void generatePhoneLabel();

        M_STYLABLE_WIDGET(ContactItemStyle)
};

#endif // CONTACTITEM_H
