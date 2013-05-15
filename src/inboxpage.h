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

#ifndef INBOXPAGE_H
#define INBOXPAGE_H

#include <MApplicationPage>
#include <QModelIndex>
#include <MLayout>
#include <MLinearLayoutPolicy>
#include <MStylableWidget>
#include <MPannableViewport>

#include "backend.h"
#include "inboxitemmodel.h"
#include "dialogpage.h"

class InboxPage : public MApplicationPage
{
    Q_OBJECT
public:
        InboxPage();
        InboxPage(Backend*);
        virtual ~InboxPage();
        virtual void createContent();
signals:
        void startDialog(QString);
        void startNewMessage();
        void deleteAllThreads();
        void openSettings();
private slots:
        void resizeViewport();
        void inboxItemClicked(const QModelIndex&);
        void rowsRemoved();
        void rowsInserted();
protected:
        void resizeEvent(QGraphicsSceneResizeEvent *event);
private:
        Backend *mBackend;
        InboxItemModel *mModel;

        MPannableViewport *mInboxViewport;
        MLayout *mLayout;
        MLinearLayoutPolicy *mEmptyPolicy,
                              *mNormalPolicy;
};

#endif // INBOXPAGE_H
