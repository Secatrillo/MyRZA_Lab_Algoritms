#ifndef GENDATAOBJECTCLASS_H
#define GENDATAOBJECTCLASS_H

#include "../../../../../include.h"

using namespace std;

class GenDataObjectClass
{
public:
    string DataObjectName; //Наименование dataObject
    string DataObjectRef; //Имя пути экземпляра dataObject
    bool Presence; //Обязательность/опциональность
    string DataObjectType; //CDC тип dataObject

    GenDataObjectClass(string DataObjectName_ = NULL, string DataObjectRef_ = NULL, bool Presence_ = false, string DataObjectType_ = NULL);
};

#endif