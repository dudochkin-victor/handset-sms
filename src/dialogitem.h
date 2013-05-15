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

#ifndef DIALOGITEM_H
#define DIALOGITEM_H

#include <MStylableWidget>
#include <QGraphicsGridLayout>
#include <MContainer>
#include <MLabel>
#include <MImageWidget>

#include <QDateTime>

#include "dialogitemstyle.h"
#include "smsapplication.h"
#include "smsdata.h"
#include "highlighter.h"


class M_EXPORT DialogItem : public MStylableWidget
{
    Q_OBJECT;

public:
        enum PositionType {
            POSITION_FIRST = 0,
            POSITION_MIDDLE
        };

        DialogItem(MWidget *parent = NULL);
        DialogItem(SmsData&, MWidget *parent = NULL);
        virtual ~DialogItem() {}

        void setSender(QString);
        void setMessage(QString);
        void setDatetime(QDateTime);
        void setCalltype(SmsData::Calltype);
        void setStatus(SmsData::Status);
        void setPosition(PositionType);

        virtual void timerEvent(QTimerEvent*);

protected:
        virtual void mousePressEvent(QGraphicsSceneMouseEvent*);
        virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent*);
        virtual void cancelEvent(MCancelEvent*);
private:
        void init();

signals:
        void clicked();

private slots:
        void callClicked();
        void forwardActionClicked();
        void contactDetailsClicked();
        void clearThreadClicked();
        void deleteThreadClicked();
        void phoneLinkPressed(QString);
        void urlLinkPressed(QString);

private:
        QGraphicsGridLayout *mLayout;
        MStylableWidget *mChatbubble;
        MLabel *mTimestamp;
        MLabel *mMessageLabel;
        Highlighter mLinks;

        QDateTime mDatetime;
        QString mSender;
        QString mMessage;
        SmsData::Calltype mCalltype;
        SmsData::Status mStatus;
        PositionType mPosition;
        bool mPressed;

        void createContent();

        M_STYLABLE_WIDGET(DialogItemStyle);
};

#endif // DIALOGITEM_H
