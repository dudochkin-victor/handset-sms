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

#include "highlighter.h"
#include "smsapplication.h"
#include <QRegExp>
#include <MLabelHighlighter>
#include <MMessageBox>

namespace {


// TODO: Ak! these patterns are too hairy!  And they are not well internationalized!

const QString phoneStr(
    "(\\b|\\+)"                               // Optionial "+" to start
    "("                                       // either (
      "(\\d{7,})"                             //   seven or more digits
      "|"                                     //     or
      "([\\s-]?(\\(\\d+\\)|\\d+(?!\\d))){2,}" //   Two or more phone number parts [### or (###)] separated by exactly one space or dash
    ")"                                       // )
    "\\b"                                     // End at word boundary
    );
QRegExp phonePattern(phoneStr, Qt::CaseInsensitive, QRegExp::RegExp2);

const QString urlStr(
    "("
      "((ftp|https?)://)"          // scheme://
      "([\\w.-_]+(:[-@ ]+)?@)?"     // user:passwd@
      "([\\w-_]+\\.)+([a-z]{2,})"   // domain
    ") | ("
      "((ftp|https?)://)?"           // scheme://
      "([\\w.-_]+(:[-@ ]+)?@)?"     // user:passwd@
      "([\\w-_]+\\.)+(com|net|org|edu|gov|info|co|ly|gl)"   // domain
    ")"
    "(:\\d+)?"                     // :port
    "(/[- /]+)?"                  // /path
    "(\\?[- #]+)?"                // query
    "(#[\\w]+)?"                  // #fragment
    );
QRegExp urlPattern(urlStr, Qt::CaseInsensitive);


} //namespace


void Highlighter::phoneLinkPressed (QString phone)
{
    qDebug() << __FUNCTION__ << phone;
    //% "Call %1"
    MDialog *dialog = new MDialog(qtTrId("label_hyperlink_call").arg(phone), M::OkButton | M::CancelButton);
    if (dialog->exec() == M::OkButton) {
        SmsApplication::instance()->backend()->call(phone);
    }
}


void Highlighter::urlLinkPressed (QString url)
{
    qDebug() << __FUNCTION__ << url;
    //% "Open %1"
    MDialog *dialog = new MDialog(qtTrId("label_hyperlink_openurl").arg(url), M::OkButton | M::CancelButton);
    if (dialog->exec() == M::OkButton) {
        SmsApplication::instance()->backend()->openUrl(url);
    }
}


void Highlighter::enable (MLabel * pLabel)
{
    if (!pLabel) {
        return;
    }

    MCommonLabelHighlighter * pPhoneHighlighter = new MCommonLabelHighlighter(phonePattern);
    MCommonLabelHighlighter * pUrlHighlighter = new MCommonLabelHighlighter(urlPattern);

    pLabel->addHighlighter(pPhoneHighlighter);
    pLabel->addHighlighter(pUrlHighlighter);

    connect(pPhoneHighlighter, SIGNAL(clicked(QString)), this, SLOT(phoneLinkPressed(QString)));
    connect(pUrlHighlighter, SIGNAL(clicked(QString)), this, SLOT(urlLinkPressed(QString)));
}

