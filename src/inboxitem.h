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

#ifndef INBOXITEM_H
#define INBOXITEM_H

#include <QGraphicsLinearLayout>

#include <MStylableWidget>
#include <MLabel>
#include <MImageWidget>

#include <QDateTime>

#include "inboxitemstyle.h"
#include "smsapplication.h"
#include "highlighter.h"

class M_EXPORT InboxItem : public MStylableWidget
{
    Q_OBJECT
public:
        InboxItem(MWidget *parent = NULL);
        virtual ~InboxItem() {}

        void setData(QString sender,
                     QString message,
                     QDateTime datetime,
                     SmsData::Calltype calltype,
                     bool isDraft,
                     int newCount);

protected:
        virtual void mousePressEvent(QGraphicsSceneMouseEvent*);
        virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent*);
        virtual void cancelEvent(MCancelEvent*);

signals:
        void clicked();

private slots:
        void callClicked();
        void contactDetailsClicked();
        void deleteThreadClicked();

private:
        QGraphicsLinearLayout *mTimestamplayout;
        MImageWidget *mUserpic;
        MImageWidget *mSmsicon;
        MLabel *mSenderLabel;
        MLabel *mTimestamp;
        MLabel *mMessageLabel;
        Highlighter mLinks;

        QDateTime mDatetime;
        QString mSender;
        QString mMessage;
        int mNewMsgCount;
        bool mDraft;
        bool mPressed;
        SmsData::Calltype mCalltype;

        void createContent();

        M_STYLABLE_WIDGET(InboxItemStyle)
};

#endif // INBOXITEM_H
