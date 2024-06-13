
#include <sequencer2.h>

Sequencer2::Sequencer2( void (*step1)(), unsigned long time1,
                        void (*step2)(), unsigned long time2){
    t1 = time1;
    t2 = time2;
    s1func = step1;
    s2func = step2;
                
}

void Sequencer2::set_step1_time(unsigned long time){
    t1 = time;
}

void Sequencer2::set_step2_time(unsigned long time){
    t2 = time;
}

unsigned long Sequencer2::get_step1_time(){
    return t1;
}
unsigned long Sequencer2::get_step2_time(){
    return t2;
}

void Sequencer2::reset(){
     current_step = STEP1;
     next_step_time = 0;
}

void Sequencer2::reset(unsigned long delay){
     current_step = STEP1;
     next_step_time = millis() + delay;
}

void Sequencer2::run(){
    
    switch (current_step) {
        
        case STEP1:
            if (millis() >= next_step_time) {  
                s1func();
                next_step_time = millis() + t1; 
                current_step = STEP2; 
            }
        break;
        
        case STEP2:
            if (millis() >= next_step_time) {  
                s2func();
                next_step_time = millis() + t2; 
                current_step = STEP1; 
            }
        break;
    }
}