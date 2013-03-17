//
//  Copyright (C) 16-03-2013 Jasper den Ouden.
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

#include "../DTComm_no_buff.cpp"

#include <assert.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
    srand(time(NULL));
    if( argc==2 )
    {   assert( strcmp(argv[1],"stdin")==0 ); }//Must be specifying stdin.
    else{ assert(argc==1); } //May not do anything else.
    
    printf("Started.\n");

    DTComm_no_buff dtc;
    
    dtc.average = 256*255; 
    
    printf("%d", rand());
    while(1)
    {
        byte enter=43; //Take a byte, any byte.
        if( argc==2 ){ enter = fgetc(stdin); }
        else         { enter = rand()%256; }
        printf("%d\n",enter);
        
        for( byte shift=1 ; shift!=0 ; shift = shift<<1 )
        {   report_dt(&dtc, (enter&shift) ? 64 : 128 ); }
        
        printf("%d vs %d ; av %d", enter, dtc.cur,dtc.average/256); //Compare results.
        printf(enter==dtc.cur ? "        *\n" : "\n");
        reset(&dtc);
    }
}
