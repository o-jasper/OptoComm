//
//  Copyright (C) 25-02-2013 Jasper den Ouden.
//
//  This is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published
//  by the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//

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
//Pretends it is nine bits, so it is never zero, or 2^9-1
    f->a = (f->a + data + (data>127 ? 0 : 256))%511;
    f->b = (f->a + f->b)%511;
}

inline uint16_t sum(Fletcher* f)
{ return (f->a << 8) | f->b; }
