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

#include <QGraphicsLinearLayout>

#include <MAction>
#include <MLayout>
#include <MLinearLayoutPolicy>
#include <MButton>
#include <MList>
#include <MDialog>
#include <MMessageBox>
#include <MCompleter>

#include <MWidgetCreator>

#include "inboxpage.h"
#include "inboxitemcreator.h"

M_REGISTER_WIDGET(InboxPage);

InboxPage::InboxPage() :
    mBackend(0), mModel(0)
{}

InboxPage::InboxPage(Backend *backend) :
    mBackend(backend), mModel(0)
{
    //% "SMS"
    setTitle(qtTrId("inboxpage_title"));
    setPannable(false);

    mLayout = new MLayout;

    // Page actions
    //% "New Message"
    MAction *actionNewMessage = new MAction("icon-newMessage-inactive", qtTrId("inboxpage_action_newmessage"), this);
    actionNewMessage->setLocation(MAction::ApplicationMenuLocation);
    addAction(actionNewMessage);
    connect(actionNewMessage, SIGNAL(triggered()), this, SIGNAL(startNewMessage()));

    //% "Delete all messages"
    MAction *actionDeleteAll = new MAction(qtTrId("inboxpage_action_deleteall"), this);
    actionDeleteAll->setLocation(MAction::ApplicationMenuLocation);
    addAction(actionDeleteAll);
    connect(actionDeleteAll, SIGNAL(triggered()), this, SIGNAL(deleteAllThreads()));

    //% "Settings"
    MAction *actionSettings = new MAction(qtTrId("inboxpage_action_settings"), this);
    actionSettings->setLocation(MAction::ApplicationMenuLocation);
    addAction(actionSettings);
    connect(actionSettings, SIGNAL(triggered()), this, SIGNAL(openSettings()));
    /////
    // Policy 1: no messages
    /////
    mEmptyPolicy = new MLinearLayoutPolicy(mLayout, Qt::Vertical);

    //% "There are no messages"
    MLabel *emptyLabel = new MLabel(QString("<b>%1</b>").arg(qtTrId("inboxpage_nomessages")));
    emptyLabel->setAlignment(Qt::AlignCenter);
    emptyLabel->setObjectName("inboxEmptyMessage");
    mEmptyPolicy->addItem(emptyLabel);

    //% "Send a new message"
    MButton *newMessageButton = new MButton(qtTrId("inboxpage_button_newmessage"));
    newMessageButton->setObjectName("inboxNewmessageButton");
    newMessageButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mEmptyPolicy->addItem(newMessageButton);
    connect(newMessageButton, SIGNAL(clicked()), this, SIGNAL(startNewMessage()));

    /////
    // Policy 2: normal display with messages
    /////
    mNormalPolicy = new MLinearLayoutPolicy(mLayout, Qt::Vertical);

    mInboxViewport = new MPannableViewport();
    {
        MList *list = new MList();
        list->setCellCreator(new InboxItemCreator());
        mModel = new InboxItemModel(mBackend);
        list->setItemModel(mModel);
        mInboxViewport->setObjectName("inboxViewport");
        mInboxViewport->setWidget(list);
        connect(list, SIGNAL(itemClicked(const QModelIndex&)), this, SLOT(inboxItemClicked(const QModelIndex&)));
    }
    mNormalPolicy->addItem(mInboxViewport);

    connect(mModel, SIGNAL(rowsInserted(const QModelIndex&, int, int)), this, SLOT(rowsInserted()));
    connect(mModel, SIGNAL(rowsRemoved(const QModelIndex&, int, int)), this, SLOT(rowsRemoved()));


    rowsInserted();

    centralWidget()->setLayout(mLayout);
}

InboxPage::~InboxPage()
{
    delete mBackend;
    delete mModel;
}

void InboxPage::createContent()
{}


void InboxPage::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    MApplicationPage::resizeEvent(event);
    resizeViewport();
}

void InboxPage::resizeViewport()
{
    qreal inboxViewportHeight = size().height()
        - centralWidget()->geometry().top();

    mInboxViewport->setMinimumHeight(inboxViewportHeight);
    mInboxViewport->setMaximumHeight(inboxViewportHeight);
    mInboxViewport->setPreferredHeight(inboxViewportHeight);
}

void InboxPage::rowsInserted()
{
    if (mModel->rowCount() > 0) {
        mLayout->setPolicy(mNormalPolicy);
    }
    this->mInboxViewport->setPosition(QPointF());
}

void InboxPage::rowsRemoved()
{
    if (0 == mModel->rowCount()) {
        mLayout->setPolicy(mEmptyPolicy);
    }

}

void InboxPage::inboxItemClicked(const QModelIndex &index)
{
    QVariant data = index.data(Qt::DisplayRole);
    SmsThread rowData = data.value<SmsThread>();
    emit startDialog(rowData.first().sender);
}
