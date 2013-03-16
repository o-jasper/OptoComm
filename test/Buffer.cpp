//
//  Copyright (C) 25-02-2013 Jasper den Ouden.
//
//  This is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published
//  by the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//

//Computer side test for Buffer.

#include "../Buffer.cpp"

#include <time.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

Buffer buffer; //Buffer that is tested.
byte in[Buffer_size];

int main(int argc, char* argv[])
{
    srand(time(NULL));
    if( argc==2 )
    {   assert( strcmp(argv[1],"stdin")==0 ); }//Must be specifying stdin.
    else{ assert(argc==1); } //May not do anything else.
    
    printf("Started.\n");
    int n=0,m=0;
    while(1) //Generate random stuff.
    { 
        if( 1 ) //m!=0 || rand()%2 )
        {   printf("b\n");
            byte got[Buffer_size];
            int n = 1+ rand()%(Buffer_size-m-1); //TODO want to read some arbitrary ammount.
            for( int i=m ; i<n ; i++ )
            {   printf("*\n");
                if( argc==2 ){ got[i] = fgetc(stdin); }
                else         { got[i] = rand()%256; }
                receive_byte(&buffer, got[i]);
            }
            for( int i=0 ; i<n ; i++ )
            {   byte r = read_byte(&buffer);
                if( got[i] != r )
                { 
                    printf("#Failed %d of %d,\n %d vs %d", i,n, got[i],r);
                    return 0;
                }
                assert(got[i]==r);
            }
        }
        else
        {   printf("i\n");
            int16_t got[Buffer_size];
            
            int n = 1+rand()%(2*(Buffer_size-2));
            for( int i=0 ; i<n ; i++ )
            {   printf("*\n");
                union
                {   int16_t v;
                    byte a[2];
                } x;
                x.v = rand();
                got[i]= x.v;
                receive_byte(&buffer, x.a[0]);
                receive_byte(&buffer, x.a[1]);
            }
            for( int i=0 ; i<n ; i++ )
            {  assert( got[i] == read_int(&buffer) ); }
        }
    }
}
