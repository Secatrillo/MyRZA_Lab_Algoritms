#ifndef TRIGGEROPTION_H
#define TRIGGEROPTION_H

class TriggerOption
{
public:
    /* data */
    bool dchg;
    bool qchg;
    bool dupd;


    TriggerOption(bool dchg = false, bool qchg = false, bool dupd = false);
};

#endif