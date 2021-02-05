
#include <sequencer4.h>

Sequencer4::Sequencer4( void (*step1)(), unsigned long time1, 
                        void (*step2)(), unsigned long time2,
                        void (*step3)(), unsigned long time3,
                        void (*step4)(), unsigned long time4){
    t1 = time1;
    t2 = time2;
    t3 = time3;
    t4 = time4;
    s1func = step1;
    s2func = step2;
    s3func = step3;
    s4func = step4;
                
}

void Sequencer4::set_step1_time(unsigned long time){
    t1 = time;
}
void Sequencer4::set_step2_time(unsigned long time){
    t2 = time;
}
void Sequencer4::set_step3_time(unsigned long time){
    t3 = time;
}
void Sequencer4::set_step4_time(unsigned long time){
    t4 = time;
}

unsigned long Sequencer4::get_step1_time(){
    return t1;
}
unsigned long Sequencer4::get_step2_time(){
    return t2;
}
unsigned long Sequencer4::get_step3_time(){
    return t3;
}
unsigned long Sequencer4::get_step4_time(){
    return t4;
}


void Sequencer4::reset(){
     current_step = STEP1;
     next_step_time = 0;
}

void Sequencer4::run(){
    
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
                current_step = STEP4; 
            }
        break;
        
        case STEP4:
            if (millis() >= next_step_time) {  
                s4func();
                next_step_time = millis() + t4; 
                current_step = STEP1; 
            }
        break;
    }
}