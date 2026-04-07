#ifndef GENCOMMONDATACLASS_H
#define GENCOMMONDATACLASS_H

#include <string>
using namespace std;

class GenCommonDataClass
{
public:
    string CDCid; //Имя типа атрибута данных
    GenCommonDataClass(string CDCid_ = "");
};

#endif