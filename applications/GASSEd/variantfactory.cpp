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

#include "variantfactory.h"
#include "variantmanager.h"
#include "fileedit.h"
#include "CustomTypes.h"

VariantFactory::~VariantFactory()
{
    QList<QWidget *> editors = theEditorToProperty.keys();
    QListIterator<QWidget *> it(editors);
    while (it.hasNext())
        delete it.next();
}

void VariantFactory::connectPropertyManager(QtVariantPropertyManager *manager)
{
    connect(manager, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
                this, SLOT(slotPropertyChanged(QtProperty *, const QVariant &)));
    connect(manager, SIGNAL(attributeChanged(QtProperty *, const QString &, const QVariant &)),
                this, SLOT(slotPropertyAttributeChanged(QtProperty *, const QString &, const QVariant &)));
    QtVariantEditorFactory::connectPropertyManager(manager);
}

QWidget *VariantFactory::createEditor(QtVariantPropertyManager *manager,
        QtProperty *property, QWidget *parent)
{
    FileEdit *editor = NULL;
    if (manager->propertyType(property) == filePathTypeId()) editor = new FileEdit(parent);
    if (manager->propertyType(property) == newFileTypeId()) editor = new FileEdit(parent, true);
    if (editor) {
        editor->setFilePath(manager->value(property).toString());
        editor->setFilter(manager->attributeValue(property, QLatin1String("filter")).toString());
        theCreatedEditors[property].append(editor);
        theEditorToProperty[editor] = property;

        connect(editor, SIGNAL(filePathChanged(const QString &)),
                this, SLOT(slotSetValue(const QString &)));
        connect(editor, SIGNAL(destroyed(QObject *)),
                this, SLOT(slotEditorDestroyed(QObject *)));
        return editor;
    }
    return QtVariantEditorFactory::createEditor(manager, property, parent);
}

void VariantFactory::disconnectPropertyManager(QtVariantPropertyManager *manager)
{
    disconnect(manager, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
                this, SLOT(slotPropertyChanged(QtProperty *, const QVariant &)));
    disconnect(manager, SIGNAL(attributeChanged(QtProperty *, const QString &, const QVariant &)),
                this, SLOT(slotPropertyAttributeChanged(QtProperty *, const QString &, const QVariant &)));
    QtVariantEditorFactory::disconnectPropertyManager(manager);
}

void VariantFactory::slotPropertyChanged(QtProperty *property,
                const QVariant &value)
{
    if (!theCreatedEditors.contains(property))
        return;

    QList<QWidget *> editors = theCreatedEditors[property];
    QListIterator<QWidget *> itEditor(editors);
    QString str;
    if(value.userType() == filePathTypeId())
    {
        FilePathPropertyType path = value.value<FilePathPropertyType>();
        str = path.value();
    }
    if(value.userType() == newFileTypeId())
    {
        NewFilePropertyType path = value.value<NewFilePropertyType>();
        str = path.value();
    }
    while (itEditor.hasNext())
    {
        QWidget* editor = itEditor.next();
        FileEdit* fileEdit = dynamic_cast<FileEdit*>(editor);
        if(fileEdit) fileEdit->setFilePath(str);
    }
}

void VariantFactory::slotPropertyAttributeChanged(QtProperty *property,
            const QString &attribute, const QVariant &value)
{
    if (!theCreatedEditors.contains(property))
        return;

    if (attribute != QLatin1String("filter"))
        return;

    QList<QWidget *> editors = theCreatedEditors[property];
    QListIterator<QWidget *> itEditor(editors);
    while (itEditor.hasNext())
    {
        FileEdit* fileEdit = dynamic_cast<FileEdit*>(itEditor.next());
        if(fileEdit) fileEdit->setFilter(value.toString());
    }
}

void VariantFactory::slotSetValue(const QString &value)
{
    QObject *object = sender();
    QMap<QWidget *, QtProperty *>::ConstIterator itEditor =
                theEditorToProperty.constBegin();
    while (itEditor != theEditorToProperty.constEnd()) {
        if (itEditor.key() == object) {
            QtProperty *property = itEditor.value();
            QtVariantPropertyManager *manager = propertyManager(property);
            if (!manager)
                return;
            manager->setValue(property, value);
            return;
        }
        itEditor++;
    }
}

void VariantFactory::slotEditorDestroyed(QObject *object)
{
    QMap<QWidget *, QtProperty *>::ConstIterator itEditor =
                theEditorToProperty.constBegin();
    while (itEditor != theEditorToProperty.constEnd()) {
        if (itEditor.key() == object) {
            QWidget *editor = itEditor.key();
            QtProperty *property = itEditor.value();
            theEditorToProperty.remove(editor);
            theCreatedEditors[property].removeAll(editor);
            if (theCreatedEditors[property].isEmpty())
                theCreatedEditors.remove(property);
            return;
        }
        itEditor++;
    }
}

