//
//  Copyright (C) 27-03-2013 Jasper den Ouden.
//
//  This is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published
//  by the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//

#include "byte.h"

class DtSend 
{
public: 
    DtSend(byte* array,int len);
    
    Buffer b;
    byte cur,shift;
    Fletcher chk,nchk;
};

inline DtSend::DtSend(byte* array,int len)
{   b = Buffer(array, len);
    shift=0;
}
//Returns if any bits are left.
// If there are any left, you may not start writing the next message.
int any_bits_left(DtSend* dts)
{   return dts->shift==255 || byte_ready_p(dts); }

int bits_cnt(DtSend* dts)
{   int ret = 8*byte_cnt(&b->dts);
    switch(dts->cur)
    {   case 0:   return ret;
        case 1:   return ret + 7;
        case 2:   return ret + 6;
        case 4:   return ret + 5;
        case 8:   return ret + 4;
        case 16:  return ret + 3;
        case 32:  return ret + 2;
        case 64:  return ret + 1;
    }
    return -2;
}

inline void write_byte(DtSend* dts, byte b)
{   write_byte(&dts->b, b); }
inline void write_bytes(DtSend* dts, byte* arr,int len)
{   write_bytes(&dts->b, arr,len); }

//Replace old message, discarding it.(if anything there)
int flush_replace_msg(DtSend* dts, byte* array,int len, int max_len)
{   
    int ret = any_bits_left(dts);
    dts->shift = 0;
    reset(&dts->chk); reset(&dts->nchk); //Reset checksums.
    write_flush(&dts->b, array,len,max_len);
    
    return ret;
}
int flush_replace_msg(DtSend* dts, byte* array,int len)
{   flush_replace_msg(dts, array,len,len); }

void swap_out_buffer(DtSend* dts, byte* array,int len)
{   dts->shift = 0;
    swap_out_buffer(&dts->b, array,len); 
}

//Flushes old message, copying it entirely.
int flush_replace_msg_cpy(DtSend* dts, byte* array,int len)
{   if( dts->b.len < len ){ return -1; }
    int ret = any_bits_left();
    dts->shift = 0;
    write_flush_copy(&dts->b, array,len);
    return ret;
}

//Reads a single bit and checksums the relevant byte if needed.
// (This is used to send out the data bit by bit.)
byte read_bit(DtSend* dts)
{
    dts->shift = dtm->shift << 1;
    if( dts->shift ==0 ) //Current byte ran out.
    {   dts->cur = read_byte(&dts->b);
        //Add to checksums.
        step(&dts->chk,   dts->cur);
        step(&dts->nchk, ~dts->cur);
        dts->shift = 1;
        return dts->cur&1;
    }
    return  dts->cur & dts->shift;
}
//Finalizes a message by setting `finalize_i` and writing the fletcher sum.
void finalize_message(DtSend* dts)
{
    union
    {   uint16_t chk;
        byte arr[2];
    }
    chk = sum(&dts->chk); //Write both checksums.
    write_byte(&dts->b, arr+0); write_byte(&dts->b, arr+1);
    chk = sum(&dts->nchk);
    write_byte(&dts->b, arr+0); write_byte(&dts->b, arr+1);
    //Reset both.
    reset(&dts->chk); reset(&dts->nchk);
}
