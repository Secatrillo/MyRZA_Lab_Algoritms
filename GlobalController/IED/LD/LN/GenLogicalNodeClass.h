#pragma once

#include <string>

 //General Logical Node Class
class GenLogicalNodeClass{


    public:
        
        GenLogicalNodeClass(std::string LogicalNodeName__, std::string LNRef_);

        std::string getLogicalNodeName();
        std::string getLNRef();

        void setLogicalNodeName(std::string name);
        void setLNRef(std::string ref);

    private:
        std::string LogicalNodeName=NULL;
        std::string LNRef=NULL;
};

