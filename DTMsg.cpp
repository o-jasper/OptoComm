//
//  Copyright (C) 16-03-2013 Jasper den Ouden.
//
//  This is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published
//  by the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//

//Just adds the (quite needed) buffer to the DTComm.

#include "byte.h"

class DTMsg 
{
public: 
    DTMsg(byte* array,int max_length);
    byte cur,shift;
    byte* arr;
    int len,max_len;
}

void reset(DTMsg* dtm)
{
    dtm->len = 0;
    dtm->cur = 0; dtm->shift=1;
}
inline void discard(DTMsg* dtm) //Done with message.
{ reset(dtm); }

void swap_arr(DTMsg* dtm, byte* array,int max_length)
{   dtm->arr = array;
    dtm->max_len = max_length;
}

inline DTMsg::DTMsg(byte* array,int max_length)
{   swap_arr(this, array,max_length);
    reset(this);
}
                    
inline void report_dt(DTMsg* dtc, uint8_t dt)
{
    dtm->average = (rate*(dtm->average + dt))/(rate+1); 
    if( dtm->shift==255 ) //Old message not processed yet.(screw it.)
    {  return; }
    
    const char i = (256*dt) / dtm->average;
    if( i>=2 )
    {   dtm->shift=255; //Indicates a message might be ready.
        return; 
    }
    
    if( i ) //Put a one if longer than average.
    {   dtm->cur |= dtm->shift; }
//NOTE: it just zeros out when done, regular DTComm does something with it.
    dtm->shift = dtm->shift << 1;

    if( dtc.dt->shift == 0 ) //All shifted out, go to next byte if `1`(long)
    {   dtm->arr[dtm->len] = dtm->cur; //Put into message array.
        dtm->len++; //Next element.
        dtm->cur=0; dtm->shift = 1; //Get ready for the next byte.
    }
}

//_you_ have to `reset` the buffer when done with it, 
// do a `swap_arr` if you want to hold on to the data.
//  0      if message ok.
//  1      if still receiving.
//  2,4,6  if doesnt validate.(two distinct ways)
char msg_fail_p(DTMsg* dtm) 
{   if( dtm->shift!=255 )
    {   return 1; }
    Fletcher chk,nchk; //Calculate checksums.
    for( int i=0 ; i< dtm->len-4 ; i++ )
    {   step(&chk, dtm->arr[i]); 
        step(&nchk, ~dtm->arr[i]); 
    }
 //Hopefully ~2^-30~1/1G or something false positive, prob much worse.
    char ret = 2*(*(uint16_t*)(dtm->arr + dtm->len-4) != sum(&chk)) +
               4*(*(uint16_t*)(dtm->arr + dtm->len-2) != sum(&nchk));
    return ret;
}
//Returns the pointer to the array.
byte* msg_array(DTMsg* dtm)
{ return dtm->arr; }
