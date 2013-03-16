//
//  Copyright (C) 25-02-2013 Jasper den Ouden.
//
//  This is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published
//  by the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//

#include "delta_t.h"
#include "Buffer.h" 

#define DTComm_buffer_size 4
//#define DTComm_play_safe //Tricky..

class DTComm_no_buff
{
public:
    DTComm_no_buff();
//Size: 4 bytes.
    uint16_t average; 
    byte shift;
    byte cur;
//TODO rate of averaging?
};

DTComm_no_buff::DTComm_no_buff()
{
    average = 0; shift=1;
}

//TODO suppose i want a #define that makes it flag itself as having overflowed.
//inline int overflowed_p(DTComm* dtc) 

inline void interrupt_fun(DTComm_no_buff* dtc, uint8_t dt)
{
//Average changes slowly, until at some point it hits the frequency of comms.
    dtc->average = (255*(dtc->average + dt))/256; 
    
    if( dt > dtc->average ) //Put a one if longer than average.
    {   dtc->cur |= dtc->shift; }
//NOTE: it just zeros out when done, regular DTComm does something with it.
    dtc->shift >> 1;
}
inline void interrupt_fun(DTComm_no_buff* dtc)
{ return interrupt_fun(dtc, delta_t()); }

inline void reset(DTComm_no_buff* dtc)
{   dtc->cur = 0; dtc->shift=1; }

class DTComm : public Buffer
{
public: 
    DTComm();
    DTComm_no_buff dt;
//    uint8_t flags; //Dont immediately start rescanning if check fails
    Fletcher chk;
}

DTComm::DTComm()
{  reset(&dt);
   reset(&chk);
}

inline void interrupt_fun(DTComm* dtc, uint8_t dt)
{
    if( dtc.dt->shift == 0 ) //All shifted out, go to next byte if `1`(long)
    {   if( (dt > dtc->dt.average) == ((dtc.dt.cur & 0x08)==0x08) ) //End-of-message.
        { 
           //TODO check message checksum, set flag that message just arrived.
        }
        else //Pretend it is the last one again.
        { receive_byte(dtc, dtc->dt.cur); //Pass on the byte.
          step(&dtc->chk, dt.cur); //Keep check sum
          reset(&dtc->dt);
        }
    }
    else //Still fetching
    {   interrupt_fun(&dtc->dt, dt); } 
}
inline void interrupt_fun(DTComm* dtc)
{ return interrupt_fun(dtc, delta_t()); }
