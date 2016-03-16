/****************************************************************************
**
** Copyright (C) 2006 Trolltech ASA. All rights reserved.
**
** This file is part of the documentation of Qt. It was originally
** published as part of Qt Quarterly.
**
** This file may be used under the terms of the GNU General Public License
** version 2.0 as published by the Free Software Foundation or under the
** terms of the Qt Commercial License Agreement. The respective license
** texts for these are provided with the open source and commercial
** editions of Qt.
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "fileedit.h"
#include <QHBoxLayout>
#include <QToolButton>
#include <QFileDialog>
#include <QFocusEvent>
#include <QSettings>

FileEdit::FileEdit(QWidget *parent, bool save)
    : QWidget(parent), _save(save)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    theLineEdit = new QLineEdit(this);
    theLineEdit->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    QToolButton *button = new QToolButton(this);
    button->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred));
    button->setText(QLatin1String("..."));
    layout->addWidget(theLineEdit);
    layout->addWidget(button);
    theLineEdit->setFocusProxy(this);
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_InputMethodEnabled);
    connect(theLineEdit, SIGNAL(textEdited(const QString &)),
                this, SIGNAL(filePathChanged(const QString &)));
    connect(button, SIGNAL(clicked()),
                this, SLOT(buttonClicked()));
}

void FileEdit::buttonClicked()
{
    QSettings settings;
    QString folderStr = theLineEdit->text();
    if(folderStr.isEmpty() || folderStr.isNull())
        folderStr = settings.value("FileEditFolder",QDir::homePath()).toString();
    QString filePath;
    if(!_save)
    {
        filePath = QFileDialog::getOpenFileName(this, "Choose a file", folderStr, theFilter);
    }
    else filePath = QFileDialog::getSaveFileName(this, "Choose a file", folderStr, theFilter);
    if (filePath.isNull())
        return;
    theLineEdit->setText(filePath);
    settings.setValue("FileEditFolder", filePath);
    emit filePathChanged(filePath);
}

void FileEdit::focusInEvent(QFocusEvent *e)
{
    theLineEdit->event(e);
    Qt::FocusReason reason = e->reason();
    if (reason == Qt::OtherFocusReason) {
        theLineEdit->selectAll();
    }
    QWidget::focusInEvent(e);
}

void FileEdit::focusOutEvent(QFocusEvent *e)
{
    theLineEdit->event(e);
    QWidget::focusOutEvent(e);
}

void FileEdit::keyPressEvent(QKeyEvent *e)
{
    theLineEdit->event(e);
}

void FileEdit::keyReleaseEvent(QKeyEvent *e)
{
    theLineEdit->event(e);
}
