
#ifndef SEQUENCER1_H
#define SEQUENCER1_H

#include "Arduino.h"

class Sequencer1{
    public:
    
    Sequencer1( void (*step1)(), unsigned long time1);
    
    void reset();
    
    void run();
    
    void set_step1_time(unsigned long time);
    unsigned long get_step1_time();
    
    private:
    
    unsigned long t1 = 0;
    void (*s1func)() = 0;
    
    uint32_t next_step_time = 0;
};

#endif