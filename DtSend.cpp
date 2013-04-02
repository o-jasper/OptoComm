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
{   return dts->shift!=128 || byte_ready_p(&dts->b); }
//Returns number of bits left.
int bits_cnt(DtSend* dts)
{   int ret = 8*byte_cnt(&dts->b);
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
    int ret = any_bits_left(dts);
    dts->shift = 0;
    write_flush_copy(&dts->b, array,len);
    return ret;
}

//Reads a single bit and checksums the relevant byte if needed.
// (This is used to send out the data bit by bit.)
byte read_bit(DtSend* dts)
{
    dts->shift = dts->shift << 1;
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
    int16_t chk_i = sum(&dts->chk); //Write both checksums.
    write_bytes(&dts->b, (byte*)&chk_i,2);
    chk_i = sum(&dts->nchk);
    write_bytes(&dts->b, (byte*)&chk_i,2);
    //Reset both.
    reset(&dts->chk); reset(&dts->nchk);
}
//`bits` Indicates for up to 32 bytes which ones should be absorbed into the buffer in
// multiples at a time. When a bit is `1`, the next byte is connected to it.
// (idem if next bit is `1`)
int8_t absorb_bytes(DtSend* dts, byte* ref, int8_t at, uint32_t bits, uint8_t len)
{
    if( at>=len ){ return -at; }
    write_byte(dts, ref[at]);
    while( bits&(1<<at) && at<len-1 ) //While we also should get the next one...
    {   at++; 
        write_byte(dts, ref[at]); //..get it.
    }
    return at;
}

enum //Non-data statusses of data sending.
{
    msg_id_1 = 128, msg_id_2 = 129,
    msg_finalized = 130,
    msg_send_end = 131,
};

//Does the handling of a message, including identity, and finalizing. 
// Status starts at 32 for 1-bit identity and 33 for two.
//The return value is the resulting status, status changes:
// `msg_id_1` or `msg_id_2`  For two lengths of ids.
// `0`                       Zero during the message
// `msg_finalized`           Indicates the checksums are being send.
// `msg_send_end`            End of the message.(checksum is sent.
int8_t message_chopper(DtSend* dts, uint16_t id,uint8_t status, byte* ref, 
                       uint32_t bits,uint8_t len)
{
    switch( status )
    {
     //Send identity.(first)
      case msg_id_1: write_byte(dts, (byte)id);      return 0;
      case msg_id_2: write_bytes(dts, (byte*)&id,2); return 0;
      case msg_finalized: case msg_send_end: return msg_send_end;
     //Which goes to the part where the message is sent.
      default: //Now, `status` indicates where on the data.
          if( status >= len ) //Message is done, send checksums, wrap up.
          {   finalize_message(dts);
              return msg_finalized;
          }
          //Send the data.
          // (same as return absorb_bytes(dts, status, bits,len);)
          write_byte(dts, ref[status]); 
          while( bits&(1<<status) && status<len-1 ) //While we also should get the next one...
          {   status++; 
              write_byte(dts, ref[status]); //..get it.
          }
          return status;
    }
}
//Works the same as message chopper, but 'switches the buffer to the data'.
int8_t message_pointer(DtSend* dts, int16_t id, uint8_t status,
                       byte* ref, uint8_t len,
                       byte* buffer,int buffer_len)
{
    switch( status )
    {
    case msg_id_1:
        write_byte(dts, (byte)id);
        flush_replace_msg(dts, ref,len); //Switch from the buffer to `ref`.
        return 0;
    case msg_id_2: 
        write_bytes(dts, (byte*)&id,2); 
        flush_replace_msg(dts, ref,len); //Switch from the buffer to `ref`.
        return 0;
    case msg_finalized: return msg_send_end;
        
    default: //`ref` already 'is' the buffer, no need moving anything.
             // So if here, must finalize.
        swap_out_buffer(dts, buffer,buffer_len); //Back to the buffer.
        finalize_message(dts); //Write checksums.
        return msg_finalized;
    }
}
