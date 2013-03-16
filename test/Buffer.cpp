//
//  Copyright (C) 16-03-2013 Jasper den Ouden.
//
//  This is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published
//  by the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//

//Computer side test for Buffer.

#define Buffer_size 8
#include "../Buffer.cpp"

#include <time.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
    srand(time(NULL));
    if( argc==2 )
    {   assert( strcmp(argv[1],"stdin")==0 ); }//Must be specifying stdin.
    else{ assert(argc==1); } //May not do anything else.

    Buffer buffer; //Buffer that is tested.
        
    printf("Started.\n");
    int at=0;
    while(1) //Generate random stuff.
    { 
        do //Do a whole bunch of byte-reading and receiving.
        {   byte got[Buffer_size];
            int add_upto = at + rand()%(Buffer_size-at); //Arbitrary lengths received.
            printf("at %d add_upto %d ", at,add_upto);
            for( ; at < add_upto ; at++ )
            {   if( argc==2 ){ got[at] = fgetc(stdin); }
                else         { got[at] = rand()%256; }
                receive_byte(&buffer, got[at]);
            }
            assert(at==add_upto);
            int read_upto = rand()%(at+1); //And arbitrary lengths read.
            for( int i=0 ; i<read_upto ; i++ )
            {   byte r = read_byte(&buffer);
                if( got[i] != r )
                { 
                    printf("#Failed %d of %d; %d vs %d", i,read_upto, got[i],r);
                    return  -1;
                }
                assert(got[i]==r);
            }
            //And get rid of the data we already tested.
            printf("read_upto %d\n", read_upto); 
            assert( at>=read_upto );
            if( at>read_upto )
            { memmove(got, got+read_upto, at-read_upto); }//Move it backwards.
            at -= read_upto;
        } while(at!=0);
        //And some other data size.
        int16_t got[Buffer_size];
        int n = 1;//+rand()%(Buffer_size/2-1);
        printf("Integer size for %d\n", n);
        for( int i=0 ; i<n ; i++ )
        {   byte a[2];
            got[i]= rand();
            ((int16_t*)a)[0]= got[i];
            assert( got[i] == *(int16_t*)a );
            receive_byte(&buffer, a[0]); //Stuff into buffer.
            receive_byte(&buffer, a[1]);
        }
        for( int i=0 ; i<n ; i++ )
        {   int16_t r = read_int(&buffer);
            if( got[i]!=r )
            {   printf("#Failed %d of %d; %d vs %d", i,n, got[i],r);
                return -1;
            }
            assert( got[i] == r );
        }
    }
}
