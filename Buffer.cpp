//
//  Copyright (C) 25-02-2013 Jasper den Ouden.
//
//  This is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published
//  by the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//

#include <stdint.h>

typedef uint8_t byte;

#define Buffer_size 4

class Buffer
{
public:
    Buffer();
//Size: 2 + Buffer_size bytes.
    uint8_t i,j;
    byte buffer[Buffer_size];
};

Buffer::Buffer()
{   i=0; j=0; }

//How much is ready in buffer.
inline int8_t buffer_cnt(Buffer* b)
{   int8_t n = b->i - b->j;
    return n<0 ? n + Buffer_size : n;
}
//Return if at least one byte ready.
inline bool byte_ready_p(Buffer* b)
{ return b->i!= b->j; }

inline void receive_byte(Buffer* b, byte cur)
{
    b->buffer[b->i] = cur; //Register it.
    b->i = (b->i + 1)%Buffer_size; //Increment buffer pos.
        
#if defined( Buffer_bug_indicator )
    if( b->i == b->j ) //Didnt read quickly enough.
    { Buffer_bug_indicator |= 1; }
#endif
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
    b->j = (b->j + 1)%Buffer_size;
#if defined( Buffer_bug_indicator )
    if( b->i == b->j ) //Read too quickly.
    { Buffer_bug_indicator |= 2; }
#endif
    return r;
}

inline int16_t read_int(Buffer* b)
{   
#if Buffer_large_enough_int //Buffer_size < sizeof int
    Buffer_bug_indicator |= 4; //Buffer not big enough to read int from it.
#endif
    int16_t val = *(int*)b->buffer;
    b->j = (b->j + sizeof(int))%Buffer_size;
#if Buffer_large_enough_int //Buffer_size < sizeof int
    if( b->i == b->j ) //Read too quickly.
    { Buffer_bug_indicator |= 2; }
#endif
    return val;
}
inline int32_t read_long(Buffer* b)
{
#if Buffer_large_enough_long //Buffer_size<sizeof(long)
    Buffer_bug_indicator |= 8; //Buffer not big enough to read long from it.
#endif
    int32_t val = *(long*)b->buffer;
    b->j = (b->j + sizeof(long))%Buffer_size;
#if Buffer_large_enough_long //Buffer_size<sizeof(long)
    if( b->i == b->j ) //Read too quickly.
    { Buffer_bug_indicator |= 2; }
#endif
    return val;
}
