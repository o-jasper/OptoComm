
# Time difference communications
Idea of this project is to use time differences between pulses to send data
without clock. I want it to be able to accept a whole range of time differences.

Stuff in `delta_t.c` uses
[advanced timer use from mythic-beasts.com/~markt](http://www.mythic-beasts.com/~markt/ATmega-timers.html), and arduinos `timer_setup`.

`Buffer.cpp` implements a basic buffer.

`DTComm.cpp` implements receiving data and puts it in said stuff.

Conspicuously missing is a data and offset checker, by far most data must not pass, because
it will catch a lot of non-data when it has not yet found the frequency. Or if 
some other situation is not suitable.

# TODO

* Nothing is tested, probably can be tested partially offline.
* Decoder.
* `.h` files and stuff.

# License

GPLv3
