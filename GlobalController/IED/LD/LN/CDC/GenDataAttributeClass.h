#ifndef GENDATAATTRIBUTECLASS_H
#define GENDATAATTRIBUTECLASS_H
#include "../../../../../include.h"
#include "CDT/TriggerOption.h"
#include "CDT/EnumFunctionalConstraints.h"


using namespace std;

class GenDataAttributeClass
{
public:
    string DataAttributeName; //Имя атрибута данных
    EnumFunctionalConstraints FunctionalConstraint; //Функциональная связь
    TriggerOption TrgOp; //Триггер запуска отправки отчёта
    string DataAttributeType; //Тип аттрибута данных
    string helpDataType;  //Вспомогательная переменная с описанием возможных значений
    double value;  //Значение атрибута данных
    string DataAttributeRef; //Ссылка на аттрибут данных

    GenDataAttributeClass(string DataAttributeName_, EnumFunctionalConstraints FunctionalConstraint_, TriggerOption TrgOp_ = NULL,
                          string DataAttributeType_ = "", string helpDataType_ = "", double value_ = 0.0, string DataAttributeRef_ = "");
};

#endif