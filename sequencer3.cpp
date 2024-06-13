
#include <sequencer3.h>

Sequencer3::Sequencer3( void (*step1)(), unsigned long time1, 
                        void (*step2)(), unsigned long time2,
                        void (*step3)(), unsigned long time3){
    t1 = time1;
    t2 = time2;
    t3 = time3;
    s1func = step1;
    s2func = step2;
    s3func = step3;
                
}

void Sequencer3::set_step1_time(unsigned long time){
    t1 = time;
}
void Sequencer3::set_step2_time(unsigned long time){
    t2 = time;
}
void Sequencer3::set_step3_time(unsigned long time){
    t3 = time;
}

unsigned long Sequencer3::get_step1_time(){
    return t1;
}
unsigned long Sequencer3::get_step2_time(){
    return t2;
}
unsigned long Sequencer3::get_step3_time(){
    return t3;
}


void Sequencer3::reset(){
     current_step = STEP1;
     next_step_time = 0;
}

void Sequencer3::reset(unsigned long delay){
     current_step = STEP1;
     next_step_time = millis() + delay;
}

void Sequencer3::run(){
    
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
                current_step = STEP3; 
            }
        break;
        
        case STEP3:
            if (millis() >= next_step_time) {  
                s3func();
                next_step_time = millis() + t3; 
                current_step = STEP1; 
            }
        break;
    }
}