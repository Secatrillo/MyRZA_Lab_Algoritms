#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <array>
#include "TriggerOption.h"
#include "EnumFunctionalConstraints.h"




typedef class GenDataAttributeClass{
    public:
        
        GenDataAttributeClass(std::string DataAttributeName_, EnumFunctionalConstraints FunctionalConstraint_, TriggerOption TrgOp_ = NULL,
                            std::string DataAttributeType_ = "", std::string helpDataType_ = "", double value_ = 0.0, std::string DataAttributeRef_ = "");

        std::string               getDataAttributeName();
        EnumFunctionalConstraints getFunctionalConstraint();
        TriggerOption             getTrgOp();
        std::string               getDataAttributeType();
        std::string               gethelpDataType();
        double                    getvalue();
        std::string               getDataAttributeRef();

        void                      setDataAttributeName(std::string DAName);
        void                      setFunctionalConstraint(EFC FuncConstr);
        void                      setTrgOp(TriggerOption trigOption);
        void                      setDataAttributeType(std::string DAType);
        void                      sethelpDataType(std::string HDAType);
        void                      setvalue(double data);
        void                      setDataAttributeRef(std::string DARef);


    private:
        std::string DataAttributeName; //Имя атрибута данных
        EnumFunctionalConstraints FunctionalConstraint; //Функциональная связь
        TriggerOption TrgOp; //Триггер запуска отправки отчёта
        std::string DataAttributeType; //Тип аттрибута данных
        std::string helpDataType;  //Вспомогательная переменная с описанием возможных значений
        double value;  //Значение атрибута данных
        std::string DataAttributeRef; //Ссылка на аттрибут данных
} GDAClass;

