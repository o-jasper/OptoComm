//
//  Copyright (C) 27-03-2013 Jasper den Ouden.
//
//  This is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published
//  by the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//

#include "byte.h"

class DtReceive 
{
public: 
    DtReceive(byte* array,int max_length);

    uint16_t average;
    
    byte cur,shift;
    byte* arr;
    int len,max_len;
};

void reset(DtReceive* dtm)
{
    dtm->len = 0;
    dtm->cur = 0; dtm->shift=1;
}
inline void discard(DtReceive* dtm) //Done with message.
{ reset(dtm); }

void swap_arr(DtReceive* dtm, byte* array,int max_length)
{   dtm->arr = array;
    dtm->max_len = max_length;
}

inline DtReceive::DtReceive(byte* array,int max_length)
{   swap_arr(this, array,max_length);
    reset(this);
    average=0;
}
                    
inline void report_dt(DtReceive* dtm, uint8_t dt, uint8_t rate)
{    
    if( dtm->shift==255 ) //Old message not processed yet.(screw it.)
    {  return; }
    
    dtm->average = (rate*(dtm->average + dt))/(rate+1);
    
    const char i = (256*dt) / dtm->average;
    if( i>=2 )
    {   dtm->shift=255; //Indicates a message might be ready.
        return; 
    }
    
    if( i==0 ) //Put a one if longer than average.
    {   dtm->cur |= dtm->shift; }
    dtm->shift = dtm->shift << 1;

    if( dtm->shift == 0 ) //All shifted out, go to next byte if `1`(long)
    {   dtm->arr[dtm->len] = dtm->cur; //Put into message array.
        dtm->len++; //Next element.
        if( dtm->len==dtm->max_len ) //Ran out, assume it is a message.
        {   dtm->shift= 255; return; }
        dtm->cur=0; dtm->shift = 1; //Get ready for the next byte.
    }
}
inline void report_dt(DtReceive* dtm, uint8_t dt)
{    report_dt(dtm, dt,255); }

//_you_ have to `reset` the buffer when done with it, 
// do a `swap_arr` if you want to hold on to the data.
//  0      if message ok.
//  1      if still receiving.
//  2,4,6  if doesnt validate.(two distinct ways)
//  8      unacceptably short.
char msg_fail_p(DtReceive* dtm) 
{   if( dtm->shift!=255 ){   return 1; } //Still receiving.
    if( dtm->len <= 4 )  { return 8; } //Unacceptably short.
    Fletcher chk,nchk; //Calculate checksums.
    for( int i=0 ; i< dtm->len-4 ; i++ )
    {   step(&chk,   dtm->arr[i]); 
        step(&nchk, ~dtm->arr[i]); 
    }
 //Hopefully ~2^-30~1/1G or something false positive, prob much worse.
    char ret = 2*(*(uint16_t*)(dtm->arr + dtm->len-4) != sum(&chk)) +
               4*(*(uint16_t*)(dtm->arr + dtm->len-2) != sum(&nchk));
    return ret;
}
//Returns the pinter to the array.
byte* msg_array(DtReceive* dtm)
{ return dtm->arr; }
