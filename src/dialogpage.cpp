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
#include <QGraphicsSceneResizeEvent>
#include <QTimer>

#include <MLayout>
#include <MButton>
#include <MAction>
#include <MSceneManager>
#include <MMessageBox>
#include <MPositionIndicator>

#include <MWidgetCreator>
#include <MTextEditView>

#include "dialogpage.h"
#include "dialogitem.h"
#include "contactitemcreator.h"
#include "util.h"

#define SMS_LIMIT 160

DialogPage::DialogPage(Backend *backend, QString phone, QString message)
    : mBackend(backend),
      mOverlay(new MOverlay())
{
    qDebug() << __FUNCTION__ << "(phone:" << phone << ", message:'" << message << "')";
    mPhone = phone;

    setPannable(false);

    setEscapeMode(MApplicationPageModel::EscapeManualBack);

    // overlay for SMS character limit counter
    mOverlay->setObjectName("dialogOverlay");
    mCountdownlabel = new MLabel(mOverlay.data());
    mCountdownlabel->setObjectName("countdownText");

    mLayout = new MLayout;

    phone = stripPhone(phone);

    /////
    // Policy 1: new message mode (contact search bar, contacts, text entry bar)
    /////
    mNewMessagePolicy = new MLinearLayoutPolicy(mLayout, Qt::Vertical);

    mContactModel = new ContactItemModel(mBackend);
    mContactFilterModel = new ContactSortFilterModel();
    mContactFilterModel->setSourceModel(mContactModel);

    mContactSearchBar = new MStylableWidget();
    {
        mCompleter = new MCompleter(QStringList());
        mContactSearchBar->setObjectName("contactSearchBar");
        QGraphicsLinearLayout *topLayout = new QGraphicsLinearLayout(Qt::Vertical);
        mContactSearchBar->setLayout(topLayout);
        MStylableWidget *entryBox = new MStylableWidget();
        entryBox->setObjectName("entryBox");
        QGraphicsLinearLayout *entryLayout = new QGraphicsLinearLayout(Qt::Horizontal);
        entryBox->setLayout(entryLayout);
        //% "To"
        MLabel *toLabel = new MLabel(qtTrId("dialogpage_to"));
        entryLayout->addItem(toLabel);

        mAddressEntry = new MTextEdit(MTextEditModel::SingleLine);
        //% "Enter a contact name or number"
        mAddressEntry->setPrompt(qtTrId("dialogpage_address_prompt"));
        mAddressEntry->setObjectName("textEntry");
        mAddressEntry->setCompleter(mCompleter);

        //connect(mCompleter, SIGNAL(startCompleting(const QString&)), this, SLOT(customizedComplete(const QString&)));
        //connect(mAddressEntry, SIGNAL(textChanged()), this, SLOT(maybeClearedComplete()));
        connect(mAddressEntry, SIGNAL(cursorPositionChanged()), this, SLOT(parseAddressEntry()));

        //MButton *clear = new MButton();
        //clear->setObjectName("clearButton");
        entryLayout->addItem(mAddressEntry);
        //entryLayout->addItem(clear);

        topLayout->addItem(entryBox);

        //connect(clear, SIGNAL(clicked()), mAddressEntry, SLOT(clear()));
    }

    mContactsViewport = new MPannableViewport();
    {
        MList *contactsList = new MList();
        contactsList->setCellCreator(new ContactItemCreator());
        contactsList->setItemModel(mContactFilterModel);
        mContactsViewport->setObjectName("contactsViewport");
        mContactsViewport->setWidget(contactsList);
        mContactsViewport->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        connect(contactsList, SIGNAL(itemClicked(const QModelIndex&)), this, SLOT(contactItemClicked(const QModelIndex&)));
    }

    mTextEntryBar = new MStylableWidget();
    {
        mTextEntryBar->setObjectName("textEntryBar");
        QGraphicsLinearLayout *bottomLayout = new QGraphicsLinearLayout(Qt::Vertical);
        mTextEntryBar->setLayout(bottomLayout);
        MStylableWidget *entryBox = new MStylableWidget();
        entryBox->setObjectName("entryBox");
        QGraphicsLinearLayout *entryLayout = new QGraphicsLinearLayout(Qt::Horizontal);
        entryBox->setLayout(entryLayout);
        mEntry = new MTextEdit(MTextEditModel::MultiLine,
                QString());
        //% "Tap here to start typing"
        mEntry->setPrompt(qtTrId("dialogpage_textentry_prompt"));
        mEntry->setObjectName("textEntry");
        if (!message.isEmpty()) {
            mEntry->setText(message);
            qDebug() << "Set message body to given text:" << mEntry->text();
        } else if (!phone.isEmpty()) {
            mEntry->setText(mBackend->getDraft(phone, mMsgId));
            qDebug() << "Set message body to draft: " << mEntry->text();
        }

        //MButton *clear = new MButton();
        //clear->setObjectName("clearButton");
        MButton *send = new MButton();
        send->setViewType(MButton::iconType);
        send->setIconID("icon-m-toolbar-send");
        send->setMaximumWidth(50);
        send->setMaximumHeight(50);
        //MStylableWidget *separator = new MStylableWidget();
        //separator->setObjectName("textSeparator");
        send->setObjectName("sendButton");
        entryLayout->addItem(mEntry);
        //entryLayout->addItem(clear);
        //entryLayout->addItem(separator);
        entryLayout->addItem(send);

        bottomLayout->addItem(entryBox);

        connect(mEntry, SIGNAL(textChanged()), this, SLOT(textChanged()));
        //connect(clear, SIGNAL(clicked()), mEntry, SLOT(clear()));
        connect(mEntry, SIGNAL(returnPressed()), this, SLOT(sendClicked()));
        connect(send, SIGNAL(clicked()), this, SLOT(sendClicked()));
    }

    mNewMessagePolicy->addItem(mContactSearchBar);
    mNewMessagePolicy->addItem(mContactsViewport);
    mNewMessagePolicy->addItem(mTextEntryBar);

    /////
    // Policy 2: conversation mode (contact name bar, dialog, text entry bar)
    /////
    mConversationPolicy = new MLinearLayoutPolicy(mLayout, Qt::Vertical);

    mDialogViewport = new MPannableViewport();
    {
        mDialogList = new DialogList();
        mDialogViewport->setObjectName("dialogViewport");
        mDialogViewport->setWidget(mDialogList);
        mDialogViewport->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

    mConversationPolicy->addItem(mDialogViewport);
    mConversationPolicy->addItem(mTextEntryBar);


    centralWidget()->setLayout(mLayout);

    if (!phone.isEmpty()) {
        setDialogMode(phone);
        QTimer::singleShot(200, this, SLOT(scrollToBottom()));
    } else {
        //% "New message"
        setTitle(qtTrId("newmessagepage_title"));
        mLayout->setPolicy(mNewMessagePolicy);
    }

    parseAddressEntry();
    QtConcurrent::run(mBackend, &Backend::markRead, mPhone);
}

DialogPage::~DialogPage()
{
    qDebug(__FUNCTION__);

    const QString message = mEntry->text().trimmed();
    if (message.isEmpty()) return;

    if (mMsgId.isValid()) {
        mBackend->saveDraft(mMsgId, mEntry->text());
    }
    else if (!mPhone.isEmpty()) {
        mBackend->saveDraft(mPhone, mEntry->text());
    }
    else {
        normalizePhones();

        foreach (QString phone, mCompletedAddresses) {
            const bool valid = isValidPhone(phone);
            qDebug() << "phone:" << phone << " ,valid:" << valid;
            if (!valid) continue;
            mBackend->saveDraft(phone, mEntry->text());
        }
    }

}

void DialogPage::createContent()
{}

void DialogPage::setDialogMode(QString phone)
{
    qDebug() << __FUNCTION__ << "(" << phone << ")";

    mPhone = phone;
    // page actions
    /*
    MAction *actionDelete= new MAction("Delete this thread", this);
    actionDelete->setLocation(MAction::ApplicationMenuLocation);
    addAction(actionDelete);
    connect(actionDelete, SIGNAL(triggered()), this, SLOT(deleteActionTriggered()));
    */

    //mName->setText(SmsApplication::instance()->backend()->getName(mPhone));
    const QString name =  SmsApplication::instance()->backend()->getName(mPhone);
    setTitle(name);
    qDebug() << "Setting title to: " << name;
    //mAvatar->setPixmap(SmsApplication::instance()->backend()->getAvatar(mPhone));

    // populate dialog from history
    SmsData dat;
    //beginInsertRows(QModelIndex(), 0, mModel.size()-1);
    const bool excludeDrafts = true;
    foreach (dat, mBackend->getSmsHistory(mPhone, excludeDrafts)) {
        if (mPhone == dat.sender && !dat.draft) {
            mModel.append(dat);
        }
    }
    qSort(mModel);
    int i;
    for (i = 0; i < mModel.size(); i++) {
        DialogItem *item = new DialogItem(mModel[i]);
        if (i > 0 && mModel[i-1].calltype == mModel[i].calltype)
            item->setPosition(DialogItem::POSITION_MIDDLE);
        mDialogList->addItem(item);
    }

    connect(mBackend, SIGNAL(smsAdded(QList<SmsData>)), this, SLOT(sms_added(QList<SmsData>)));
    connect(mBackend, SIGNAL(smsModified(QList<SmsData>)), this, SLOT(sms_modified(QList<SmsData>)));
    connect(mBackend, SIGNAL(smsRemoved(QList<QString>)), this, SLOT(sms_removed(QList<QString>)));

    mLayout->setPolicy(mConversationPolicy);
}

void DialogPage::deleteActionTriggered()
{
    emit deleteThread(mPhone);
}

void DialogPage::textChanged()
{
    int chars = mEntry->text().size();
    if (chars > SMS_LIMIT - 6) {
        mOverlay->appear();
        //% "%1/%2"
        mCountdownlabel->setText(qtTrId("dialogpage_remainchars_counter").arg(SMS_LIMIT - (chars%SMS_LIMIT)).arg(chars/SMS_LIMIT + 1));
    } else {
        mOverlay->disappear();
    }
}


void DialogPage::normalizePhones ()
{
    for (int i = 0; i < mEnteredAddresses.size(); ++i) {
        if (i >= mCompletedAddresses.size()) {
            mCompletedAddresses << mEnteredAddresses.at(i).trimmed();
        }
        else {
            QString & phone = mCompletedAddresses[i]; // reference to element
            if (phone.isEmpty()) {
                phone = mEnteredAddresses.at(i).trimmed();  // change list element
            }
        }
    }

    mCompletedAddresses.removeAll("");
}


void DialogPage::sendClicked()
{
    const QString & message = mEntry->text().trimmed();

    if (message.isEmpty()) {
        qDebug() << "No text to send";
        //% "Error: No text to send"
        MMessageBox message(qtTrId("dialogpage_missing_message"));
        message.exec();
        return;
    }

    if (!mPhone.isEmpty()) {
        qDebug() << __FUNCTION__ << " - message:" << message << ", mPhone:" << mPhone
                 << "mMsgId:" << mMsgId;

        mBackend->send(mPhone, message, mMsgId);
        mEntry->clear();
        return;
    }

    normalizePhones();

    qDebug() << __FUNCTION__ << " - message:" << message << ", mPhone:" << mPhone
             << "mCompletedAddresses:" << mCompletedAddresses.join(",");

    // Check all addresses before sending anything

    if (mCompletedAddresses.isEmpty()) {
        qDebug("Error: No phone numbers given");
        //% "Error: No phone numbers given"
        MMessageBox message(qtTrId("dialogpage_missing_phone"));
        message.exec();
        return;
    }

    foreach (QString phone, mCompletedAddresses) {
        if (! isValidPhone(phone)) {
            qDebug() << "Error: phone number is not valid: " << phone;
            //% "Error: phone number is not valid: %1"
            MMessageBox message(qtTrId("dialogpage_invalid_phone").arg(phone));
            message.exec();
            return;
        }
    }

    // Send, one per recipient phone

    foreach (QString phone, mCompletedAddresses) {
        mBackend->send(phone, message);
    }

    if (1 == mCompletedAddresses.size()) {
        setDialogMode(mCompletedAddresses.front());
        mEntry->clear();
    }
    else {
        // Just send a message to multiple people.
        // Go back to inbox
        this->disappear();
    }
}

void DialogPage::dialogUpdated()
{
    QTimer::singleShot(50, this, SLOT(scrollToBottom()));
}

void DialogPage::scrollToBottom()
{
    // No need to scroll to bottom if the list is not covering the viewport area.

    //qreal size = mDialogViewport->positionIndicator()->viewportSize().height();
    qreal range = mDialogViewport->positionIndicator()->range().height();
    //if(size < range) {
        mDialogViewport->setPosition(QPointF(0.0, range));
    //}
}

void DialogPage::contactItemClicked(const QModelIndex &index)
{
    QVariant data = index.data(Qt::DisplayRole);
    ContactData rowData = data.value<ContactData>();

    // Save selected completion
    while (mCompletedAddresses.size() <= mAddressIndex) {
        mCompletedAddresses << QString();
    }
    mCompletedAddresses[mAddressIndex] = rowData.phone;

    // TODO: check that mEnteredAddresses[] and mAddressIndex are sane
    while (mEnteredAddresses.size() <= mAddressIndex) {
        mEnteredAddresses << QString();
    }

    QString displayType = displayPhoneType(rowData);
    QString displayName = rowData.name;
    if (! displayType.isEmpty()) {
        displayName += " (" + displayType + ")";
    }

    //mEnteredAddresses[mAddressIndex] = rowData->name + " (" + rowData->phone + ")";
    mEnteredAddresses[mAddressIndex] = displayName;

    // Make sure there is a comma at the end of the address entry text
    if (! mEnteredAddresses.last().trimmed().isEmpty()) {
        mEnteredAddresses << QString();
    }

    mAddressEntry->setText(mEnteredAddresses.join(", "));
    //mAddressEntry->setFocus();

    //customizedComplete(rowData.phone);
    //mAddressEntry->setText(rowData.phone);
}

void DialogPage::parseAddressEntry()
{
    // Split text entry into comma-separated recipient addresses
    const QString & addressText = mAddressEntry->text();
    const int cursorPos = mAddressEntry->cursorPosition();
    mEnteredAddresses = addressText.split(",");

    // In case the user backspaced
    while (mCompletedAddresses.size() > mEnteredAddresses.size()) {
        mCompletedAddresses.removeLast();
    }
    for (int i = 0; i < mEnteredAddresses.size(); ++i) {
        if (i >= mCompletedAddresses.size()) {
            mCompletedAddresses << "";
        }
        else if (mEnteredAddresses.at(i).trimmed().isEmpty()) {
            mCompletedAddresses[i].clear();
        }
    }

    qDebug() << "Split '" << addressText << "' into: " << mEnteredAddresses.join(" | ");

    QString left = addressText.left(cursorPos);
    mAddressIndex = left.count(',');

    const QString currAddress = mEnteredAddresses.empty() ? "" : mEnteredAddresses[mAddressIndex].trimmed();

    // Address could be in the form "Person (Context)" such as "Ben Drucker (Work)".
    // This pattern splits up the first part from the (optional) second part
    //                      (1     (2  (3   3) 2)1)
    QRegExp contextPattern("([^(]+)(\\(([^)]+)\\)?)?");
    contextPattern.indexIn(currAddress);
    mContactFilterModel->setSearchText(contextPattern.cap(1));  // first regex subpattern
    mContactFilterModel->setContext(contextPattern.cap(3));     // third regex subpattern

    mContactFilterModel->setFilterRegExp("");  // TODO: This the best way to trigger the filter?
}

void DialogPage::sms_added(QList<SmsData> messages)
{
    bool addedMessages = false;
    foreach (SmsData dat, messages) {
        // filter by sender/receiver
        if (mPhone == dat.sender && !dat.draft) {
            qDebug() << "Adding message id:" << dat.id << ", phone: " << dat.sender << ", message:" << dat.message;
            int i = mModel.size();
            mModel.insert(i, dat);
            DialogItem *item = new DialogItem(dat);
            if (i > 0 && mModel[i-1].calltype == dat.calltype)
                item->setPosition(DialogItem::POSITION_MIDDLE);
            mDialogList->addItem(item);
            addedMessages = true;
        }
    }
    if (addedMessages) {
        dialogUpdated();
        mBackend->markRead(mPhone);
    }
}

void DialogPage::sms_modified(QList<SmsData> messages)
{
    SmsData dat;
    foreach (dat, messages) {
        int i;
        for (i = 0; i < mModel.size(); i++) {
            if (mModel[i].id == dat.id) {
                qDebug() << "Changing message id:" << dat.id << ", phone: " << dat.sender << ", message:" << dat.message;
                DialogItem *item = new DialogItem(dat);
                if (i > 0 && mModel[i-1].calltype == dat.calltype)
                    item->setPosition(DialogItem::POSITION_MIDDLE);
                mModel[i] = dat;
                mDialogList->removeAt(i);
                mDialogList->insertItem(i, item);
                break;
            }
        }
    }
}

void DialogPage::sms_removed(QList<QString> ids)
{
    QString id;
    foreach (id, ids) {
        int i;
        for (i = 0; i < mModel.size(); i++) {
            if (mModel[i].id == id) {
                qDebug() << "Removing message id:" << id;
                mModel.removeAt(i);
                mDialogList->removeAt(i);
                break;
            }
        }
    }
}
