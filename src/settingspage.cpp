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

#include <MLabel>
#include <MMessageBox>
#include <QGraphicsGridLayout>

#include "settingspage.h"

SettingsPage::SettingsPage(Backend *backend)
{
    mMessageManagerInterface = backend->getMessageManager();
}

SettingsPage::~SettingsPage()
{
    // save MessageManager properties
    if (mMessageManagerInterface && mMessageManagerInterface->isValid()) {
        mMessageManagerInterface->SetProperty("UseDeliveryReports", QDBusVariant(mDeliveryButton->isChecked()));
        mMessageManagerInterface->SetProperty("ServiceCenterAddress", QDBusVariant(mSmscEdit->text()));
    }
}

void SettingsPage::createContent()
{
    //% "SMS"
    setTitle(qtTrId("settingspage_title"));

    QGraphicsGridLayout *layout = new QGraphicsGridLayout();
    centralWidget()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    //% "Settings"
    MLabel *titlelabel = new MLabel(qtTrId("settingspage_settings"));
    layout->addItem(titlelabel, 0, 0, 1, 2);

    //% "Delivery reports"
    MLabel *deliverylabel = new MLabel(qtTrId("settingspage_toggle_deliveryreports"));
    layout->addItem(deliverylabel, 1, 0);

    mDeliveryButton = new MButton();
    mDeliveryButton->setViewType(MButton::switchType);
    mDeliveryButton->setCheckable(true);
    layout->addItem(mDeliveryButton, 1, 1);

    //% "Service center"
    MLabel *smsclabel = new MLabel(qtTrId("settingspage_entry_servicecenter"));
    layout->addItem(smsclabel, 2, 0);

    mSmscEdit = new MTextEdit(MTextEditModel::SingleLine);
    mSmscEdit->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    layout->addItem(mSmscEdit, 2, 1);

    layout->setColumnStretchFactor(0, 0);

    centralWidget()->setLayout(layout);

    // get MessageManager properties
    if (mMessageManagerInterface && mMessageManagerInterface->isValid()) {
        QDBusPendingReply<QVariantMap> reply = mMessageManagerInterface->GetProperties();
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)), this, SLOT(getPropertiesFinished(QDBusPendingCallWatcher*)));
    } else {
        qCritical("not connected to ofono");
        //% "Error: not connected to ofono"
        MMessageBox message(qtTrId("settings_err"));
        message.exec();
    }
}


void SettingsPage::getPropertiesFinished(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<QVariantMap> reply = *call;
    if (reply.isError()) {
        qCritical() << reply.error().name() << reply.error().message();
    } else {
        mDeliveryButton->setChecked(reply.value().value("UseDeliveryReports").value<bool>());
        mSmscEdit->setText(reply.value().value("ServiceCenterAddress").value<QString>());
    }
    delete call;
}
