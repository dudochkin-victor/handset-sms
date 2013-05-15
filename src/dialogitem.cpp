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

#include <QGraphicsSceneEvent>
#include <QRegExp>

#include <MCancelEvent>
#include <MWidgetCreator>
#include <MAction>
#include <MMessageBox>

#include "smsapplication.h"
#include "dialogitem.h"
#include "util.h"
#include "highlighter.h"

M_REGISTER_WIDGET(DialogItem)

DialogItem::DialogItem(MWidget *parent)
    :MStylableWidget(parent)
{
    init();
}

DialogItem::DialogItem(SmsData &dat, MWidget *parent)
    :MStylableWidget(parent)
{
    init();

    mPosition = DialogItem::POSITION_FIRST;
    mSender = dat.sender;
    mMessage = dat.message;
    mDatetime = dat.datetime;
    mCalltype = dat.calltype;
    if (mCalltype == SmsData::INCOMING) {
        setObjectName("dialogItemIncoming");
    } else {
        setObjectName("dialogItem");
    }
    mStatus = dat.status;
    createContent();
}

void DialogItem::init()
{
    setObjectName("dialogItem");
    mLayout = new QGraphicsGridLayout();
    setLayout(mLayout);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    mChatbubble = new MStylableWidget();
    QGraphicsGridLayout *bubbleLayout = new QGraphicsGridLayout();
    mChatbubble->setLayout(bubbleLayout);
    mChatbubble->setObjectName("dialogChatbubble");
    mLayout->addItem(mChatbubble, 0, 0, Qt::AlignBottom);

    mMessageLabel = new MLabel("");
    mMessageLabel->setWordWrap(true);
    mMessageLabel->setObjectName("dialogItemMessage");
    mLinks.enable(mMessageLabel);

    bubbleLayout->addItem(mMessageLabel, 0, 0);

    mTimestamp = new MLabel("");
    mTimestamp->setObjectName("dialogItemTimestamp");
    bubbleLayout->addItem(mTimestamp, 1, 0);

    //% "Forward"
    MAction *forwardaction = new MAction(qtTrId("dialogitem_action_forward"), this);
    forwardaction->setLocation(MAction::ObjectMenuLocation);
    addAction(forwardaction);
    connect(forwardaction, SIGNAL(triggered()), this, SLOT(forwardActionClicked()));

    //% "Call"
    MAction *callaction = new MAction(qtTrId("dialogitem_action_call"), this);
    callaction->setLocation(MAction::ObjectMenuLocation);
    addAction(callaction);
    connect(callaction, SIGNAL(triggered()), this, SLOT(callClicked()));

    //% "Contact details"
    MAction *detailaction = new MAction(qtTrId("dialogitem_action_details"), this);
    detailaction->setLocation(MAction::ObjectMenuLocation);
    addAction(detailaction);
    connect(detailaction, SIGNAL(triggered()), this, SLOT(contactDetailsClicked()));

    //% "Clear thread"
    MAction *clearaction = new MAction(qtTrId("dialogitem_action_clear"), this);
    clearaction->setLocation(MAction::ObjectMenuLocation);
    addAction(clearaction);
    connect(clearaction, SIGNAL(triggered()), this, SLOT(clearThreadClicked()));

    //% "Delete this thread"
    MAction *deleteaction = new MAction(qtTrId("dialogitem_action_delete"), this);
    deleteaction->setLocation(MAction::ObjectMenuLocation);
    addAction(deleteaction);
    connect(deleteaction, SIGNAL(triggered()), this, SLOT(deleteThreadClicked()));
    // timestamp update timer
    //startTimer(30000); // 30 seconds
}

void DialogItem::setSender(QString sender)
{
    mSender = sender;
    createContent();
}

void DialogItem::setMessage(QString message)
{
    mMessage = message;
    createContent();
}

void DialogItem::setDatetime(QDateTime datetime)
{
    mDatetime = datetime;
    createContent();
}

void DialogItem::setCalltype(SmsData::Calltype calltype)
{
    mCalltype = calltype;
    if (mCalltype == SmsData::INCOMING) {
        setObjectName("dialogItemIncoming");
    } else {
        setObjectName("dialogItem");
    }
    createContent();
}

void DialogItem::setStatus(SmsData::Status status)
{
    mStatus = status;
    createContent();
}

void DialogItem::setPosition(PositionType position)
{
    mPosition = position;
    createContent();
}

void DialogItem::createContent()
{
    QString text;
    if (mPosition == POSITION_FIRST) {
        if (mCalltype == SmsData::INCOMING) {
            text = QString("<b>%1:</b> ").arg(SmsApplication::instance()->backend()->getName(mSender));
        } else {
            //% "Me"
            text = QString("<b>%1:</b> ").arg(qtTrId("dialogitem_me"));
        }
    }
    text += mMessage;
    mMessageLabel->setText(text);
    mMessageLabel->setObjectName("dialogItemMessage");

    switch (mStatus) {
        case SmsData::SENT:
            if (mCalltype == SmsData::INCOMING) {
                //% "Received %1"
                mTimestamp->setText(qtTrId("dialogitem_received").arg(formatTimestamp(mDatetime)));
            } else {
                //% "Sent %1"
                mTimestamp->setText(qtTrId("dialogitem_sent").arg(formatTimestamp(mDatetime)));
            }
            mTimestamp->setObjectName("dialogItemTimestamp");
            break;
        case SmsData::DELIVERED:
            //% "Delivered %1"
            mTimestamp->setText(qtTrId("dialogitem_delivered").arg(formatTimestamp(mDatetime)));
            mTimestamp->setObjectName("dialogItemTimestamp");
            break;
        case SmsData::PENDING:
            //% "Sending..."
            mTimestamp->setText(QString("<b>%1</b>").arg(qtTrId("dialogitem_sending")));
            mTimestamp->setObjectName("dialogItemSending");
            break;
        case SmsData::DELIVER_FAILED:
            //% "Sorry, your message could not be delivered!"
            mTimestamp->setText(QString("<b>%1</b>").arg(qtTrId("dialogitem_deliverfailed")));
            mTimestamp->setObjectName("dialogItemFailed");
            break;
        case SmsData::FAILED:
        default:
            //% "Sorry, your message failed to send!"
            mTimestamp->setText(QString("<b>%1</b>").arg(qtTrId("dialogitem_sendfailed")));
            mTimestamp->setObjectName("dialogItemFailed");
            break;
    }

}

void DialogItem::forwardActionClicked()
{
    SmsApplication::instance()->showdialogpage("", mMessage);
}

void DialogItem::callClicked()
{
    SmsApplication::instance()->backend()->call(mSender);
}

void DialogItem::contactDetailsClicked()
{
    // TODO: launch seaside here
    //% "TODO: launch contacts here"
    MMessageBox message(qtTrId("dialogitem_todo_contacts"));
    message.exec();
}

void DialogItem::clearThreadClicked()
{
    SmsApplication::instance()->backend()->deleteThread(mSender);
}

void DialogItem::deleteThreadClicked()
{
    SmsApplication::instance()->backend()->deleteThread(mSender);
    SmsApplication::instance()->showinboxpage();
}

void DialogItem::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
}

void DialogItem::mousePressEvent(QGraphicsSceneMouseEvent *ev)
{
    mPressed = true;
    style().setModePressed();
    update();
    ev->accept();
    //qDebug("mousePressEvent");
}

void DialogItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *ev)
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

void DialogItem::cancelEvent(MCancelEvent *ev)
{
    mPressed = false;
    style().setModeDefault();
    update();
    ev->accept();
    //qDebug("cancelEvent");
}

void DialogItem::phoneLinkPressed(QString phone)
{
    qDebug() << "phoneLinkPressed" << phone;
    //% "Call %1"
    MDialog *dialog = new MDialog(qtTrId("dialogitem_hyperlink_call").arg(phone), M::OkButton | M::CancelButton);
    if (dialog->exec() == M::OkButton) {
        SmsApplication::instance()->backend()->call(phone);
    }
}

void DialogItem::urlLinkPressed(QString url)
{
    qDebug() << "urlLinkPressed" << url;
    //% "Open %1"
    MDialog *dialog = new MDialog(qtTrId("dialogitem_hyperlink_openurl").arg(url), M::OkButton | M::CancelButton);
    if (dialog->exec() == M::OkButton) {
        SmsApplication::instance()->backend()->openUrl(url);
    }
}
