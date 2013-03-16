//
//  Copyright (C) 25-02-2013 Jasper den Ouden.
//
//  This is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published
//  by the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//

//Thanks for the help from 
// http://www.mythic-beasts.com/~markt/ATmega-timers.html
void timer2_setup (byte mode, int prescale, byte outmode_A, byte outmode_B,
                   byte capture_mode)
{
  // enforce field widths for sanity
  mode &= 15 ;
  outmode_A &= 3 ;
  outmode_B &= 3 ;
  capture_mode &= 3 ;

  byte clock_mode = 0 ; // 0 means no clocking - the counter is frozen.
  switch (prescale)
  {
    case 1: clock_mode = 1 ; break ;
    case 8: clock_mode = 2 ; break ;
    case 64: clock_mode = 3 ; break ;
    case 256: clock_mode = 4 ; break ;
    case 1024: clock_mode = 5 ; break ;
    default:
      if (prescale < 0)
        clock_mode = 7 ; // external clock
  }
  TCCR2A = (outmode_A << 6) | (outmode_B << 4) | (mode & 3) ;
  TCCR2B = (capture_mode << 6) | ((mode & 0xC) << 1) | clock_mode ;
}

void delta_t_setup(byte prescale)
{//Hope the compiler catches it.
    timer2_setup(0, prescale, 1,1,0);
}

//Altered arduinos wiring.c `micros()`, stripped timer overflow,
//Timer 2; hands off pin 3 and 11 PWM.
inline uint8_t delta_t()
{
	uint8_t oldSREG = SREG, t;
	
	cli();
#if defined(TCNT2)
	t = TCNT2;
        TCNT2 = 0;
#elif defined(TCNT2L)
	t = TCNT2L;
        TCNT2L = 0;
#else
	#error TIMER 2 not defined
#endif
	SREG = oldSREG;
        
	return t;
}
