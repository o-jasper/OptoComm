//
//  Copyright (C) 02-04-2013 Jasper den Ouden.
//
//  This is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published
//  by the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//

//Computer side test for DTComm_no_buff
//
//NOTE: current state: seems to work, but suppose tests may want to look at
//  behaviors.
//
// * Errors.(mainly missing ones)
// * Long durations of zeros/ones.
//
//Also the idea is that some of these may be caught?

#include <assert.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#include "../Fletcher.cpp"
#include "../DtReceive.cpp"
#include "../Buffer.cpp"
#include "../DtSend.cpp"

#define GEN_MSG_CNT 21

double fail_prob = 0.001; //Chances of success.

int send=0, got=0;
long try_cnt[GEN_MSG_CNT]; //Keep record of success rate.
long success_cnt[GEN_MSG_CNT], false_success_cnt[GEN_MSG_CNT];
long too_short_cnt[GEN_MSG_CNT];

char messages[GEN_MSG_CNT][32]=
{".Zero", ".One", ".Two", ".Three", ".Four",".Five",".Six",".Seven",".Eight",".Nine",".Ten",
 ".Eleven", ".Twelve", ".Thirteen", ".Fourteen",".Fifteen",".Sixteen",".Seventeen",
 ".Eighteen",".Nineteen",".Twenty"};

int16_t msg_i; //Which message currently chosen.
unsigned have_delay = 0; //Delay 'in the pipeline'

void add_delay(int add)
{
    have_delay+= add; //TODO could add random distributions to the actual delay.
}

void sdelay();
//Sender, shouldnt use anything from the sender, except through sdelay.
uint8_t status; //Current status of the sender.
byte _sender_arr[32]; //Array for the temporary buffer
DtSend sender(_sender_arr,32);

void sender_next_message()
{
    send++;
    try_cnt[msg_i]++;
    msg_i = rand()%GEN_MSG_CNT; //Choose message.
    try_cnt[msg_i]++;
    int cur_len = strlen(messages[msg_i]+1)+1;
    printf("Send (%d) %s(%d)\n", msg_i, messages[msg_i]+1, cur_len);
    status = msg_id_1;
}

void sender_act()
{
    if( any_bits_left(&sender) ) //Just send what is in the buffer.
    {   add_delay(!read_bit(&sender) ? 100 : 50); }
    else //Otherwise, send end sign, or progress.
    {   
        if( status== msg_send_end )
        {   add_delay(200); //Send message end indicator.
            sender_next_message();
        }
        else
        {   status = message_pointer(&sender, msg_i, status,
                                     (byte*)messages[msg_i]+1,strlen(messages[msg_i]+1),
                                     _sender_arr,256); 
            sender_act(); 
        }
    }
}
//Receiver, shouldnt use anything from the sender.
byte _receiver_arr[256];
DtReceive receiver(_receiver_arr,256);

void sdelay()
{
    if( rand() > fail_prob*RAND_MAX ) //Give it to  receiver.
    {  report_dt(&receiver, (uint8_t)have_delay); 
       have_delay = 0;
    }
}

void receiver_act()
{
    sdelay();
    uint8_t r = msg_fail_p(&receiver);
    
    switch( r )
    {   
    case 1: break; //Still receiving.
    case 2: case 4: case 6: //Checksum failed.
        reset(&receiver); 
        break; 
    case 8: too_short_cnt[msg_i]++; reset(&receiver); break;
    case 0: //Reports success, check it.
        got++;
        assert(msg_i== messages[msg_i][0]);
        if( strcmp((char*)messages[msg_i]+1, (char*)receiver.arr+1) ||
            msg_i == receiver.arr[0] )
        {   false_success_cnt[msg_i]++; }
        else
        {   success_cnt[msg_i]++; }
        discard(&receiver); 
        break;
    default:
        printf("Error: %d?", r); break;
    }
}
//Reporting about stuff.
void tell_msg()
{   printf("(id %d)(len %d) %s\n", 
           receiver.arr[0],receiver.len, receiver.arr+1);
}

void print_hex(char* arr,int len)
{
    for( int i=0 ; i< len ; i++ )
    {   unsigned char v = (unsigned char) arr[i];
        printf(v<16 ? "0%X" : "%X", v);
    }
}

void tell_overall(int dud)
{
    printf("S %d G %d, success %f av %f\n", send,got, got*1.0/send, 
           receiver.average/256.0);
    for( int i=0 ; i<GEN_MSG_CNT ; i++ )
    {  printf("%d\t",success_cnt[i]); } printf("\n");
    for( int i=0 ; i<GEN_MSG_CNT ; i++ )
    {  printf("%d\t",try_cnt[i]); } printf("\n");
    for( int i=0 ; i<GEN_MSG_CNT ; i++ )
    {  printf("%d\t",false_success_cnt[i]); } printf("\n");
    for( int i=0 ; i<GEN_MSG_CNT ; i++ )
    {  printf("%d\t",too_short_cnt[i]); } printf("\n");
    
    exit(1);
}
//Main loop.
int main(int argc, char* argv[])
{
    signal(SIGINT, tell_overall);
    signal(SIGTERM, tell_overall);
    
    srand(time(NULL));
    for( int i=0 ; i< GEN_MSG_CNT ; i++ ) //Reset counters.
    {   messages[i][0]= i;
        try_cnt[i] = 0;         success_cnt[i]=0;
        false_success_cnt[i]=0; too_short_cnt[i]=0;
    }
    
    printf("Started.\n");
    receiver.average = 78*255;
    status = msg_send_end;
    while(1) //Just look at what the sender and receiver do.
    {   sender_act();
        receiver_act();
    }
}
