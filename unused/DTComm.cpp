//
//  Copyright (C) 16-03-2013 Jasper den Ouden.
//
//  This is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published
//  by the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//

//Just adds the (quite needed) buffer to the DTComm.

//#include "DTComm_no_buff.h" 
//#include "Buffer.h" 

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

inline void report_dt(DTComm* dtc, uint8_t dt)
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
    {   report_dt(&dtc->dt, dt); } 
}
