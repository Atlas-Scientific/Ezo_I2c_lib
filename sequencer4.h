
#ifndef SEQUENCER4_H
#define SEQUENCER4_H

#include "Arduino.h"

class Sequencer4{
    public:
    
    enum reading_step {STEP1, STEP2, STEP3, STEP4};
    
    Sequencer4( void (*step1)(), unsigned long time1, 
                void (*step2)(), unsigned long time2,
                void (*step3)(), unsigned long time3,
                void (*step4)(), unsigned long time4);
    
    void reset();
	void reset(unsigned long delay);
    
    void run();
    
    void set_step1_time(unsigned long time);
    void set_step2_time(unsigned long time);
    void set_step3_time(unsigned long time);
    void set_step4_time(unsigned long time);
    
    unsigned long get_step1_time();
    unsigned long get_step2_time();
    unsigned long get_step3_time();
    unsigned long get_step4_time();
    
    private:
    
    
    enum reading_step current_step = STEP1;
    
    unsigned long t1, t2, t3, t4 = 0;
    void (*s1func)() = 0;
    void (*s2func)() = 0;
    void (*s3func)() = 0;
    void (*s4func)() = 0;
    
    uint32_t next_step_time = 0;
};

#endif