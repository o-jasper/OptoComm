//
//  Copyright (C) 16-03-2013 Jasper den Ouden.
//
//  This is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published
//  by the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//

#include "byte.h"
#include <inttypes.h>

#ifndef DTComm_PC //Dont have the deps for it on the PC.
#include "delta_t.h"
#endif

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

inline void report_dt(DTComm_no_buff* dtc, uint8_t dt)
{
//Average changes slowly, until at some point it hits the frequency of comms.
    dtc->average = (255*(dtc->average + dt))/256; 
    
    if( 256*dt > dtc->average ) //Put a one if longer than average.
    {   dtc->cur |= dtc->shift; }
//NOTE: it just zeros out when done, regular DTComm does something with it.
    dtc->shift = dtc->shift << 1;
}
#ifndef DTComm_PC
inline void report_dt(DTComm_no_buff* dtc)
{ return report_dt(dtc, delta_t()); }
#endif

inline void reset(DTComm_no_buff* dtc)
{   dtc->cur = 0; dtc->shift=1; }
