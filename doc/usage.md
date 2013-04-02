
Lets say a robot wants to constantly send out its status. It's code has global
variables it wants to broadcast:

    StaticStatus stat; //Stuff that doesnt change in memory.
    ChangingA a; //Stuff that does change.
    
    typedef struct //Of this one we know more.
    {  int16_t x;
       byte y;
       int16_t z;
    } ChangingB;
    DynamicB b;

So it needs a little buffer, the sender object, and an index for knowing wher it
is in sending stuff.

    //Assume Buffer_cnt long enough, at least max(sizeof(ChangingA),sizeof(DynamicB),1)
    byte buffer_arr[Buffer_cnt]; 
    DtSend sender(buffer_arr, Buffer_cnt);
    uint16_t sender_i=0, sender_status = msg_id_1;

The timing intterupt for `DtSend`  (TODO `next_delay` is pseudocode right now..)

    if( any_bits_left(&sender) ) //Still bits left in the buffer.
    {   next_delay(read_bit(&sender) ? dt_long : dt_short); }
    else //No bits left.
    {  
        switch(sender_i)
        {
            case 0: //Currently sending the identifier of the static one.
                sender_status = message_pointer(&sender, 0,sender_status, &stat,
                                                buffer_arr,Buffer_cnt);
                break;
            case 1:
            //Not chopping at all since we dont know what `ChangingA` looks like,
            // Basically all-ones catches everything, but copies it all at once
            //and takes a buffer size >= `sizeof(ChangingA)`.
                sender_status = message_chopper(&sender, 1,sender_status, &a,
                                                0xffffffff,sizeof(ChangingA));
                break;
            case 2:
            //We can chop this one up, using what we know of the structure;
            // int16_t,byte,int16_t is respectively 2,1,2 consecutive bytes,
            // so in 10010 the `1` indicates it whenever the next byte is attached.
                sender_status = message_chopper(&sender, 1,sender_status, &b,
                                                0b10010,sizeof(ChangingB));
                break;
        }
        if( sender_status == msg_send_end ) //See if last message was done.
        {   sender_status = msg_id_1; //Back to start of status.(1-byte identifier)
            sender_i = (sender_i+1)%3;
            next_delay(dt_extra_long); //Indicate the new message.
        }
        else //Just send the first bit of this bunch.
        {   next_delay(read_bit(&sender) ? dt_long : dt_short); }
    }
