#include "CustomTypes.h"

void registerCustomTypes()
{
    qRegisterMetaType<FilePathPropertyType>("FilePathPropertyType");
    qRegisterMetaType<NewFilePropertyType>("NewFilePropertyType");
}

int filePathTypeId()
{
    return qMetaTypeId<FilePathPropertyType>();
}

int newFileTypeId()
{
    return qMetaTypeId<NewFilePropertyType>();
}

