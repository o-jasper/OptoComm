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

#include "../Fletcher.cpp"

#include <assert.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#include "../DtReceive.cpp"

#define GEN_MSG_CNT 21
char messages[GEN_MSG_CNT][32]=
{".Zero", ".One", ".Two", ".Three", ".Four",".Five",".Six",".Seven",".Eight",".Nine",".Ten",
 ".Eleven", ".Twelve", ".Thirteen", ".Fourteen",".Fifteen",".Sixteen",".Seventeen",
 ".Eighteen",".Nineteen",".Twenty"};

long try_cnt[GEN_MSG_CNT];
long success_cnt[GEN_MSG_CNT], false_success_cnt[GEN_MSG_CNT];

int msg_i,msg_j; //Which message and where in it.(_bit_)
unsigned have_delay = 0;

byte _receiver_arr[256];
DtReceive receiver(_receiver_arr,256);

double fail_prob = 0.001, read_prob = 1;

void sdelay()
{
    report_dt(&receiver, (uint8_t)have_delay); //Send bit.
    have_delay = 0;
}
void sendbit()
{
    char which = messages[msg_i][msg_j/8]&(1<<(msg_j%8));
    have_delay += (!which ? 100 : 50);
    msg_j++;
//Failed bit.(likely results in assumed new message)
    if( rand() > fail_prob*RAND_MAX ) 
    {  sdelay(); }
}
void send_between()
{
    have_delay += 200;
    sdelay();
}

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

int send=0, got=0;
void tell_overall(int dud)
{
    printf("S %d G %d, success %f av %f\n", send,got, got*1.0/send, 
           receiver.average/256.0);
    for( int i=0 ; i<GEN_MSG_CNT ; i++ )
    {  printf("%d\t",success_cnt[i]); } printf("\n");
    for( int i=0 ; i<GEN_MSG_CNT ; i++ )
    {  printf("%d\t",try_cnt[i]); } printf("\n");
    for( int i=0 ; i<GEN_MSG_CNT ; i++ )
    {  printf("%d\t",false_success_cnt[i]); }

    printf("\n");
    
    exit(1);
}

//Attaches the fletcher checksum at the end.
void fletcher_arr(char* arr,int len)
{
    Fletcher chk,nchk;
    for( int j=0 ; j<len ; j++ )
    {   step(&chk,   (uint8_t)arr[j]);
        step(&nchk, ~(uint8_t)arr[j]);
    }
    *(uint16_t*)(arr+len)   = sum(&chk);
    *(uint16_t*)(arr+len+2) = sum(&nchk);
}

int main(int argc, char* argv[])
{
    signal(SIGINT, tell_overall);
    signal(SIGTERM, tell_overall);
    
    srand(time(NULL));
/*    if( argc==2 )
    {   assert( strcmp(argv[1],"stdin")==0 ); }//Must be specifying stdin.
    else{ assert(argc==1); } */ //May not do anything else.
    printf("Started.\n");
    
//Prepare messages.
    receiver.average = 78*255;
    for( int i=0 ; i<GEN_MSG_CNT ; i++ )
    {
        success_cnt[i] = 0;
        try_cnt[i]=0;
        messages[i][0] = i; //Just happens to be convenient.
        
        int len = strlen(messages[i]+1)+2;
        fletcher_arr(messages[i], len);
        printf("%d: %s(%d) ", i, messages[i]+1, messages[i][0]);
        print_hex(messages[i], len); printf(":");
        print_hex(messages[i]+len, 4);printf("\n");

        Fletcher chk,nchk; //Recheck checksums again.
        for( int j=0 ; j<len ; j++ )
        {  step(&chk,   (uint8_t)messages[i][j]);
           step(&nchk, ~(uint8_t)messages[i][j]);
        }
        assert( sum(&chk)  == *(uint16_t*)(messages[i]+len));
        assert( sum(&nchk) == *(uint16_t*)(messages[i]+len+2));
    }
//TODO idiot, actually send the fletcher code...
// Also, *how* is it passing the checksum now and then!!
    while(1) 
    {
        send_between();
        
        msg_i = rand()%GEN_MSG_CNT; //Choose message.
        try_cnt[msg_i]++;
        
        msg_j=0; //At start of message.

        //+1 for index+1 for null char +4  for Fletcher.
        //Be warned, _do_ start from the actual string!
        int cur_len = strlen(messages[msg_i]+1)+6; 
        printf("Send (%d) %s(%d)\n", msg_i, messages[msg_i]+1, cur_len);
        send++;

        while( msg_j < 8*cur_len )
        {
            //if( msg_j%8==0 ){ printf("%x", messages[msg_i][msg_j/8]); }
            //Probability of it looking if there is a message.(bit arbitrary)
            if( rand() < read_prob*RAND_MAX ) 
            {
                int f = msg_fail_p(&receiver);
                switch( f )
                {
                case 0: //Have a message, show it:
                {   int id = receiver.arr[0];
                    assert( id < GEN_MSG_CNT );
                    if( strcmp((char*)messages[id]+1, (char*)receiver.arr+1) )
                    {   false_success_cnt[id]++; }
                    else
                    {   success_cnt[id]++; }
                    printf("Received "); tell_msg();
                    got++;
                    discard(&receiver); 
                } break;
                case 1: 
                    break; //Not done receiving yet..
                case 2:case 4: case 6:  //TODO cross check with simulated comms fail.
                    printf("Checksum failed(f %d) ", f); tell_msg();
                    reset(&receiver); break;
                case 8: 
                    printf("Shortie (%d)\n", receiver.len); reset(&receiver); break;
                }
            }
            sendbit(); //Send a bit.
        }
        assert(msg_j==8*cur_len);
    }
}
