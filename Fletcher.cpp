//
//  Copyright (C) 27-03-2013 Jasper den Ouden.
//
//  This is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published
//  by the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//

#include <stdint.h>

class Fletcher
{
public:
    Fletcher();
    uint16_t a,b;
};

Fletcher::Fletcher()
{ a=0; b=0; }

inline void reset(Fletcher* f)
{ f->a=0; f->b=0; }

inline void step(Fletcher* f, uint8_t data)
{ 
//Adds one so the effective input is never zero, pretends there are nine bits.
    f->a = ((f->a + data) + 1)%511;
    f->b = (f->a + f->b)%511;
}

inline uint16_t sum(Fletcher* f)
{ return (f->a << 8) | f->b; }
