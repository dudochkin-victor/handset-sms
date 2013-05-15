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

#ifndef DIALOGPAGE_H
#define DIALOGPAGE_H

#include <MApplicationPage>
#include <MLinearLayoutPolicy>
#include <MPannableViewport>
#include <MList>
#include <MLabel>
#include <MTextEdit>
#include <QList>
#include <QString>
#include <QStringList>
#include <QGraphicsLinearLayout>
#include <MImageWidget>
#include <MOverlay>
#include <MCompleter>
#include <MStylableWidget>

#include "dialoglist.h"
#include "contactitemmodel.h"
#include "contactsortfiltermodel.h"

class DialogPage : public MApplicationPage
{
    Q_OBJECT
public:

    // The dialog page can start in several modes:
    // phone      message
    // ---------  ---------
    // empty      empty      new draft
    // empty      non-empty  forwarded message
    // non-empty  empty      conituation of a thread (perhaps with a message in draft)
    // non-empty  non-empty  dbus call from another application

    DialogPage(Backend *backend, QString phone="", QString message="");

    virtual ~DialogPage();
    virtual void createContent();

signals:
        void deleteThread(QString);
private slots:
        void setDialogMode(QString phone);
        void deleteActionTriggered();
        void textChanged();
        void sendClicked();
        void dialogUpdated();
        void scrollToBottom();
        void contactItemClicked(const QModelIndex&);
        void parseAddressEntry();

        void sms_added(QList<SmsData>);
        void sms_modified(QList<SmsData>);
        void sms_removed(QList<QString>);
private:
        Backend *mBackend;
        QList<SmsData> mModel;
        ContactItemModel *mContactModel;
        ContactSortFilterModel *mContactFilterModel;
        MCompleter *mCompleter;

        MLayout *mLayout;
        MLinearLayoutPolicy *mNewMessagePolicy,
                              *mConversationPolicy;

        QMailMessageId mMsgId;
        QString mPhone;
        QStringList mCompletedAddresses;
        QStringList mEnteredAddresses;
        int mAddressIndex;

        MTextEdit *mAddressEntry;
        DialogList *mDialogList;
        MTextEdit *mEntry;
        QScopedPointer<MOverlay> mOverlay;
        MLabel *mCountdownlabel;
        MStylableWidget *mContactSearchBar;
        MStylableWidget *mTextEntryBar;
        MPannableViewport *mContactsViewport;
        MPannableViewport *mDialogViewport;

        void normalizePhones ();
};


#endif // DIALOGPAGE_H
