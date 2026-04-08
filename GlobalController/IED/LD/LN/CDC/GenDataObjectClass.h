#pragma once

#include <string>
#include <memory>
#include <vector>
#include <string>

template<typename T>
using ptr = std::unique_ptr<T>;




class GenDataObjectClass
{
    public:
        
        std::string getDataObjectName();
        std::string getDataObjectRef();
        bool        getPresence();
        std::string getDataObjectType();

        void setDataObjectName(std::string name);
        void setDataObjectRef(std::string ref);
        void setPresence(bool presence);
        void setDataObjectType(std::string type);

        GenDataObjectClass(std::string DataObjectName_ = NULL, std::string DataObjectRef_ = NULL, bool Presence_ = false, std::string DataObjectType_ = NULL);
    private:
        std::string DataObjectName; //Наименование dataObject
        std::string DataObjectRef; //Имя пути экземпляра dataObject
        bool Presence; //Обязательность/опциональность
        std::string DataObjectType; //CDC тип dataObject
};

