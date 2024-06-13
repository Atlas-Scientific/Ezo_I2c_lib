
#ifndef SEQUENCER2_H
#define SEQUENCER2_H

#include "Arduino.h"

class Sequencer2{
    public:
    
    enum reading_step {STEP1, STEP2};
    
    Sequencer2( void (*step1)(), unsigned long time1,
                void (*step2)(), unsigned long time2);
    
    void reset();
	void reset(unsigned long delay);
    
    void run();
    
    void set_step1_time(unsigned long time);
    void set_step2_time(unsigned long time);
    
    unsigned long get_step1_time();
    unsigned long get_step2_time();
    
    private:
    
    enum reading_step current_step = STEP1;
    
    unsigned long t1 = 0;
    unsigned long t2 = 0; 
    void (*s1func)() = 0;
    void (*s2func)() = 0;
    
    uint32_t next_step_time = 0;
};

#endif