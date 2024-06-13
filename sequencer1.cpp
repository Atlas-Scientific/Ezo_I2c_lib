
#include <sequencer1.h>

Sequencer1::Sequencer1(void (*step1)(), unsigned long time1){
    t1 = time1;
    s1func = step1;
                
}

void Sequencer1::set_step1_time(unsigned long time){
    t1 = time;
}


void Sequencer1::reset(){
     next_step_time = 0;
}

void Sequencer1::reset(unsigned long delay){
     next_step_time = millis() + delay;
}

unsigned long Sequencer1::get_step1_time(){
    return t1;
}

void Sequencer1::run(){
    if (millis() >= next_step_time) {  
        s1func();
        next_step_time = millis() + t1; 
    }
}