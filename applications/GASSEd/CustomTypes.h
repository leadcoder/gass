#ifndef CUSTOMTYPES_H
#define CUSTOMTYPES_H
#include <QString>
#include <QMetaType>
#include "QtPropertyBrowser.h"

struct QT_QTPROPERTYBROWSER_EXPORT FilePathPropertyType
{
protected:
    QString _value;
public:
    QString value()
    {
        return _value;
    }

    FilePathPropertyType()
    {
    }

    FilePathPropertyType(QString val)
    {
        _value = val;
    }
    FilePathPropertyType(const FilePathPropertyType &b)
    {
        this->_value = b._value;
    }
    ~FilePathPropertyType()
    {

    }
    QString toString() const {
        return _value;
      }
};

struct QT_QTPROPERTYBROWSER_EXPORT NewFilePropertyType : public FilePathPropertyType
{
    NewFilePropertyType()
    {
    }

    NewFilePropertyType(QString val) : FilePathPropertyType(val)
    {
    }
    NewFilePropertyType(const NewFilePropertyType &b) : FilePathPropertyType(b)
    {
    }
    ~NewFilePropertyType()
    {

    }
};
Q_DECLARE_METATYPE(FilePathPropertyType)
Q_DECLARE_METATYPE(NewFilePropertyType)

int QT_QTPROPERTYBROWSER_EXPORT filePathTypeId();

int QT_QTPROPERTYBROWSER_EXPORT newFileTypeId();


void QT_QTPROPERTYBROWSER_EXPORT registerCustomTypes();

#endif // CUSTOMTYPES_H
