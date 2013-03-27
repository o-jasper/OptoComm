
Lets say a robot wants to constantly send out its status. It's code has global
variables it wants to broadcast:

    StaticStatus stat; //Stuff that doesnt change in memory.
    DynamicA a;
    
    typedef struct
    {  int16_t x;
       byte y;
       int16_t z;
    } DynamicB;
    DynamicB b;

So it needs a little buffer, the sender object, and an index for knowing what it
is sending.

    //Assume Buffer_cnt long enough, at least max(sizeof(DynamicA),sizeof(DynamicB),1)
    byte buffer_arr[Buffer_cnt]; 
    DtSend sender(buffer_arr, Buffer_cnt);
    unsigned char sender_i = 0;

The timing intterupt for `DtSend` 

    if( any_bits_left(&sender) ) //Still bits left; message cant be complete.
    {   next_delay(read_bit(&sender) ? dt_long : dt_short); }
    else //No bits left.
    {  
        int nd = 0;
        switch(sender_i)
        {
            case 0: //Currently sending the identifier of the static one.
                nd = dt_extra_long; //Delay indicating new message.
                write_byte(&sender,0); //it's zero.
                break;
            case 1: //Send the body of it by aiming the pointer there.
                flush_replace_msg(&sender, &stat,sizeof(StaticStatus)); 
                break;
            case 2: //The end of the static status, need to get checksum, but have to
                    // use the buffer first, or will write to the `stat` object..
                swap_out_buffer(&sender, buffer, Buffer_cnt);
                finalize_message(&sender);
                break;
            
            case 3:
                nd = dt_extra_long; //(new message)
                write_byte(&sender,1); //(id) note that we're on the buffer.
                break;
            case 4: //We cant just aim the pointer, must copy!
                    //For instance the two bytes of an `int16_t` could be seen
                    //in different states!
                flush_replace_msg_cpy(&sender, &a,sizeof(DynamicA));
                break;
            case 5: //And the finalizing again.
                finalize_message(&sender);
                break;
            
            case 6: //b will be done more piecemeal, requiring less buffer.
                nd = dt_extra_long; //(new message)
                write_byte(&sender,2); //(id)
                break;
            case 7: //_First_part_ of b
                flush_replace_msg_cpy(&sender, &b.x,2);
                break;
            case 8: //Second
                write_byte(&sender, &b.y);
                break;
            case 9: //Third
                flush_replace_msg_cpy(&sender, &b.y,2);
                break;
            case 10: //And finalize
                finalize_message(&sender);
                break;
        }
        sender_i++;
        if( sender_i > 10 ){ sender_i = 0; }
        if( nd==0 ) //Figure out the delay.
        {   next_delay(read_bit(&sender) ? dt_long : dt_short); }
        else{ next_delay(nd); }
    }
