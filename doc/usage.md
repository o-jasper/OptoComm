
NOTE: I havent actually done this yet!!(WIP)

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

It needs a little buffer, the sender object, and an index for knowing where
it is in sending stuff. So initially, we need:

    //Assume Buffer_cnt long enough, at least max(sizeof(ChangingA),sizeof(DynamicB),1)
    byte buffer_arr[Buffer_cnt]; 
    DtSend sender(buffer_arr, Buffer_cnt);
    uint16_t sender_i=0, sender_status = msg_id_1;

The timing interupt will do:  (TODO `next_delay` is pseudocode right now..)

    if( any_bits_left(&sender) ) //Still bits left in the buffer, keep sending.
    {   next_delay(read_bit(&sender) ? dt_long : dt_short); }
    else //No bits left.
    {  
        switch(sender_i)
        {  //Sends the static one, this one iterates in the data structure of
           // the thing itself.
            case 0: 
                sender_status = message_pointer(&sender, 0,sender_status, &stat,
                                                buffer_arr,Buffer_cnt);
                break;
            case 1:
           //Since we dont know what it looks like, copies everything into the buffer,
           //and takes a buffer size >= `sizeof(ChangingA)`.
           //Since `a` changes, we dont iterate over it.
                sender_status = message_chopper(&sender, 1,sender_status, &a,
                                                0xffffffff,sizeof(ChangingA));
                break;
            case 2:
           //We can chop this one up, using what we know of the structure; 
           // which means the buffer can be smaller.
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
