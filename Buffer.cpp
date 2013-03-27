//
//  Copyright (C) 25-02-2013 Jasper den Ouden.
//
//  This is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published
//  by the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//

#include <stdint.h>
#include "byte.h"

class Buffer
{
public:
    Buffer(byte* array,int length);
    uint8_t i,j;
    int len;
    byte* buffer;
};

inline void reset(Buffer* b)
{   b->i=0; b->j=0; }
Buffer::Buffer(byte* array,int length)
{   reset(this);
    buffer = array;
    len=length;
}

//Return if at least one byte ready.
inline bool byte_ready_p(Buffer* b)
{ return b->i!= b->j; }

//How much is ready in buffer.
inline int8_t byte_cnt(Buffer* b)
{   int8_t n = b->i - b->j;
    return n<0 ? n + b->len : n;
}
inline int bits_cnt(Buffer* b)
{   return 8*byte_cnt(b); }

inline void write_byte(Buffer* b, byte cur)
{
    b->buffer[b->i] = cur; //Register it.
    b->i = (b->i + 1)%b->len; //Increment buffer pos.
        
#if defined( Buffer_bug_indicator )
    if( b->i == b->j ) //Didnt read quickly enough.
    { Buffer_bug_indicator |= 1; }
#endif
}
inline void write_bytes(Buffer* b, byte *arr,int len)
{   for( int i=0 ; i<len ; i++ )
    {   write_byte(b, arr[i]); }
}

//Write, flushing the current data.
void write_flush(Buffer* b, byte* array,int len, int max_len)
{   b->i = 0;
    b->j = len;
    b->len = max_len;
    b->buffer = array;
}
void write_flush(Buffer* b, byte* array,int len)
{   write_flush(b, array,len, len); }
void swap_out_buffer(Buffer* b, byte* array,int len)
{   write_flush(b, array,0,len); }

//Writes, flushing, but copying the data.
int write_flush_copy(Buffer* b, byte* array,int len)
{   if( b->len < len ){ return -1; }
    b->i = 0;
    b->j = len;
    memcpy((void*)b->buffer, array, len);
}

inline byte peek_byte(Buffer* b)
{
#if defined( Buffer_bug_indicator )
    if( b->i == b->j ) //Read too quickly.
    { Buffer_bug_indicator |= 1; }
#endif
    return b->buffer[b->i];
}

inline byte read_byte(Buffer* b) //Only valid if stuff in the buffer.
{
    byte r = b->buffer[b->j];
    b->j = (b->j + 1)%b->len;
#if defined( Buffer_bug_indicator )
    if( b->i == b->j ) //Read too quickly.
    { Buffer_bug_indicator |= 2; }
#endif
    return r;
}

inline int16_t read_int(Buffer* b)
{   
#if Buffer_large_enough_int //b->len < sizeof int
    Buffer_bug_indicator |= 4; //Buffer not big enough to read int from it.
#endif
    byte a[2]= {read_byte(b),read_byte(b)};
    return *(int16_t*)a;
}
inline int32_t read_long(Buffer* b)
{
#if Buffer_large_enough_long //b->len<sizeof(long)
    Buffer_bug_indicator |= 8; //Buffer not big enough to read long from it.
#endif
    byte a[4]= {read_byte(b),read_byte(b),
                read_byte(b),read_byte(b)};
    return *(int32_t*)a;
}
