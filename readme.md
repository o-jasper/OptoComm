
# Time difference communications
Idea of this project is to use time differences between pulses to send data
without clock, and assuming any data may be lost. I want it to be able to accept
a whole range of time differences.

The 'protocol' is to use three different durations;

    short                   : 0
    long                    : 1
    extra long; > 2*average : end of message
    
Messages end in checksums(currently a 'vapidly chosen' Fletcher variant) 
otherwise people determine what a checksum looks like. For instance starting
with a byte identifying what the message is about. 

The whole idea is that the connection may be unreliable, basically any missed
data is viewed as 'gone' in this way. Communications as one sided `DTMsg` come
down to just constantly sending data and hoping the other side can make use of 
it. If the other side can reply, in principle more effective methods may be 
used to send messages by request, or manage communication otherwise.

One worry is that the chip isnt ready to receive(for instance old message still
waiting to be discarded)in some pattern, matching the pattern in data is send;
that particular messages get 'unlucky' and never get sent. You might want to
use patterns that avoid this.

## Sending

### With 555 timer
[Monostable 555 timer](https://en.wikipedia.org/wiki/555_timer#Monostable), the
ATMega could trigger on OUTPUT, and instead of one capacitor, one to ground, and 
one to an ATMega pin, for a long pulse, that pin to `OUTPUT`, for short to
`INPUT`. Sure to work for *some* range of frequencies.(TODO dont know what)

### Internal timing
TODO a timed interrupt, i suppose.

## Receiving
Stuff in `delta_t.c` uses
[advanced timer use from mythic-beasts.com/~markt](http://www.mythic-beasts.com/~markt/ATmega-timers.html), and arduinos `timer_setup`.

`Buffer.cpp` implements a basic buffer.

`DTComm_no_buff.cpp` implements what the interrupt might look like.

`DTComm.cpp` is intended to have 'the receiver for use'. It contains

A data and offset checker, is also needed, by far most possibilities must not
pass, because it will catch a lot of non-data when it has not yet found the time
difference. The current idea is to use `Fletcher

# TODO

* A sender of data.
* Nothing is tested, probably can be tested partially offline.
* Decoder.
* `.h` files and stuff.

# License

GPLv3
