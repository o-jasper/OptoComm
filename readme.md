
# Time difference communications
Idea of this project is to use time differences between pulses to send data
without clock. I want it to be able to accept a whole range of time differences.

## Sending

### With 555 timer
[Monostable 555 timer](https://en.wikipedia.org/wiki/555_timer#Monostable), the
ATMega could trigger on OUTPUT, and instead of one capacitor, one to ground, and 
one to an ATMega pin, for a long pulse, that pin to `OUTPUT`, for short to
`INPUT`.

(TODO)

### Internal timing
TODO a timed interrupt, i suppose.

## Receiving
Stuff in `delta_t.c` uses
[advanced timer use from mythic-beasts.com/~markt](http://www.mythic-beasts.com/~markt/ATmega-timers.html), and arduinos `timer_setup`.

`Buffer.cpp` implements a basic buffer.

`DTComm.cpp` implements receiving data and puts it in said stuff.

Conspicuously missing is a data and offset checker, by far most data must not pass, because
it will catch a lot of non-data when it has not yet found the frequency. Or if 
some other situation is not suitable.

# TODO

* A sender of data.
* Nothing is tested, probably can be tested partially offline.
* Decoder.
* `.h` files and stuff.

# License

GPLv3
