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
#include <QGraphicsLinearLayout>
#include <QGraphicsSceneEvent>
#include <MCancelEvent>
#include <MWidgetCreator>
#include <MAction>
#include <MMessageBox>

#include "inboxitem.h"
#include "util.h"

M_REGISTER_WIDGET(InboxItem);

namespace {
    const QString COLOR_NEWMSG("#30ace0");
    const QString COLOR_DRAFTMSG("#f25f5a");
}

InboxItem::InboxItem(MWidget *parent)
    :MStylableWidget(parent),
    mTimestamplayout(0),
    mSmsicon(0),
    mNewMsgCount(0),
    mDraft(false),
    mPressed(false)
{
    setObjectName("inboxItem");
    QGraphicsGridLayout *layout = new QGraphicsGridLayout();
    setLayout(layout);

    mUserpic = new MImageWidget(new QPixmap());
    mUserpic->setObjectName("inboxItemUserpic");
    mUserpic->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    layout->addItem(mUserpic, 0, 0, 3, 1, Qt::AlignTop);

    mSenderLabel = new MLabel("");
    mSenderLabel->setTextElide(true);
    mSenderLabel->setObjectName("inboxItemSender");
    mSenderLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    layout->addItem(mSenderLabel, 0, 1);

    mMessageLabel = new MLabel("");
    mMessageLabel->setTextElide(true);
    mMessageLabel->setObjectName("inboxItemMessage");
    mLinks.enable(mMessageLabel);
    layout->addItem(mMessageLabel, 1, 1);

    mTimestamplayout = new QGraphicsLinearLayout();
    layout->addItem(mTimestamplayout, 2, 1);

    mTimestamp = new MLabel("");
    mTimestamp->setObjectName("inboxItemTimestamp");
    mTimestamplayout->addItem(mTimestamp);

    //% "Call"
    MAction *callaction = new MAction(qtTrId("inboxitem_action_call"), this);
    callaction->setLocation(MAction::ObjectMenuLocation);
    addAction(callaction);
    connect(callaction, SIGNAL(triggered()), this, SLOT(callClicked()));

    //% "Contact details"
    MAction *detailaction = new MAction(qtTrId("inboxitem_action_details"), this);
    detailaction->setLocation(MAction::ObjectMenuLocation);
    addAction(detailaction);
    connect(detailaction, SIGNAL(triggered()), this, SLOT(contactDetailsClicked()));

    //% "Delete this thread"
    MAction *deleteaction = new MAction(qtTrId("inboxitem_action_delete"), this);
    deleteaction->setLocation(MAction::ObjectMenuLocation);
    addAction(deleteaction);
    connect(deleteaction, SIGNAL(triggered()), this, SLOT(deleteThreadClicked()));
}

void InboxItem::callClicked()
{
    SmsApplication::instance()->backend()->call(mSender);
}

void InboxItem::contactDetailsClicked()
{
    // TODO: launch seaside here
    //% "TODO: launch contacts here"
    MMessageBox message(qtTrId("inboxitem_todo_contacts"));
    message.exec();
}

void InboxItem::deleteThreadClicked()
{
    SmsApplication::instance()->deleteThread(mSender);
}

void InboxItem::setData(QString sender,
                        QString message,
                        QDateTime datetime,
                        SmsData::Calltype calltype,
                        bool isDraft,
                        int count)
{
    mSender = sender;
    mSenderLabel->setText(QString("<b>%1</b>").arg(SmsApplication::instance()->backend()->getName(sender)));
    mUserpic->setPixmap(SmsApplication::instance()->backend()->getAvatar(sender));
    mMessage = message;
    mNewMsgCount = count;
    mDraft = isDraft;
    mCalltype = calltype;
    mDatetime = datetime;
    createContent();
}

void InboxItem::createContent()
{
    // show or hide new msg icon
    if (mNewMsgCount > 0) {
        if (!mSmsicon) {
            mSmsicon = new MImageWidget("icon-s-sms");
            mSmsicon->setObjectName("inboxItemSmsicon");
            mSmsicon->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            mTimestamplayout->insertItem(0, mSmsicon);
        }
    } else if (mSmsicon) {
        mTimestamplayout->removeItem(mSmsicon);
        delete mSmsicon;
        mSmsicon = 0;
    }

    // set timestamp line according to status
    if (mNewMsgCount > 0) {
        mMessageLabel->setText(QString("<b>%1</b>").arg(mMessage));
        //% "%Ln SMS"
        mTimestamp->setText(QString("<b><font color=\"%1\">%2</font></b>").arg(COLOR_NEWMSG).arg(qtTrId("inboxitem_unread_counter", mNewMsgCount)));
    } else if (mDraft) {
        mMessageLabel->setText(mMessage);
        //% "Draft"
        QString drafttext = qtTrId("inboxitem_draft");
        //% "this message has not been sent"
        QString drafttext2 = qtTrId("inboxitem_draft2");
        mTimestamp->setText(QString("[<font color=\"%1\">%2<font>] %3").arg(COLOR_DRAFTMSG).arg(drafttext).arg(drafttext2));
    } else {
        mMessageLabel->setText(mMessage);
        if (mCalltype == SmsData::INCOMING) {
            //% "Received %1"
            mTimestamp->setText(qtTrId("inboxitem_received").arg(formatTimestamp(mDatetime)));
        } else {
            //% "You sent this %1"
            mTimestamp->setText(qtTrId("inboxitem_sent").arg(formatTimestamp(mDatetime)));
        }
        mTimestamp->setObjectName("inboxItemTimestamp");
    }
}

void InboxItem::mousePressEvent(QGraphicsSceneMouseEvent *ev)
{
    mPressed = true;
    style().setModePressed();
    update();
    ev->accept();
    //qDebug("mousePressEvent");
}

void InboxItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *ev)
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
    //qDebug("mouseReleaseEvent");
}

void InboxItem::cancelEvent(MCancelEvent *ev)
{
    mPressed = false;
    style().setModeDefault();
    update();
    ev->accept();
    //qDebug("cancelEvent");
}
