
# Status
There are basically only tests that do some of the 'logic of the situation'
(including missed pulses) I havent beeen able to get my atmegas programmed :/

I have been able to send music using light, so presumably a circuit can be made
that can isolate pulses to some degree.

# Time difference communications
Idea of this project is to use time differences between pulses to send data
without clock, and assuming any data may be lost. I want it to be able to accept
a whole range of time differences.

The 'protocol' is to use three different durations;

    short                   : 1
    long                    : 0
    extra long; > 2*average : end of message
    
Messages end in checksums, currently a Fletchers otherwise people determine what
a checksum looks like. For instance starting with a byte identifying what the
message is about.

The whole idea is that the connection may be unreliable, basically any missed
data is viewed as 'gone' in this library. Communications as one sided `DtSend`
come down to just constantly sending data and hoping the other side can make
use of it. If the other side can reply, in principle more effective methods may be 
used to send messages by request, or manage communication otherwise.

One worry is that the chip isnt ready to receive(for instance old message still
waiting to be discarded)in some pattern, matching the pattern in data is send;
that particular messages get 'unlucky' and never get sent. You might want to
use patterns of sending data that avoid this.

## Sending ideas

### With 555 timer
[Monostable 555 timer](https://en.wikipedia.org/wiki/555_timer#Monostable), the
ATMega could trigger on OUTPUT, and instead of one capacitor, one to ground, and 
one to an ATMega pin, for a long pulse, that pin to `OUTPUT`, for short to
`INPUT`. Sure to work for *some* range of frequencies.(TODO dont know what)

### Internal timing
TODO a timed interrupt, i suppose.

## Short description of the files

`delta_t.c` uses
[advanced timer use from mythic-beasts.com/~markt](http://www.mythic-beasts.com/~markt/ATmega-timers.html), and arduinos `timer_setup` to determine time differences.

`Buffer.cpp` implements a basic buffer.

`Fletcher.cpp` checksums.

`DtReceive.cpp` does the receiving in the sense that it assumes it gets a
series of numbers; the durations between pulses. Checks messages `Fletcher.cpp`.

`DtSend.cpp` sends the data, well has a `read_bit`, which can be used to look
at the data a bit at a time and send it. It also makes checksums at the point it
is being sent.

`unused/` contains sources i currently dont have a use for.
(I dont like the inconvenience of checking out some git history)
(The idea of messages seems more useful than that of streams in microcontrollers..)

# TODO

* A sender of data.
* Tests currently only do the logic.
* Decoder.
* `.h` files and stuff.
* What is the probability of getting data wrong indicated as correct accidentally.
  
NOTE: obviously fletcher is not a secure checksum, though if done with a light 
beam, locating and inserting data into the connection should already be a bunch 
of work, no attempt is made to prevent the data being manipulated. 
(Of course, data send can be further checked..)

# License

GPLv3, see `doc/gpl-3.0.txt`
