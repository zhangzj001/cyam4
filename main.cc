#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "process.h"
#include "file.h"
#include "utils.h"
using namespace std;
using namespace yam4;


int main(int argc, const char *argv[])
{
    if(argc < 2){
        printf("Usage: yam4 [inputfile...]\n");
        return -1;
    }

    Process p;
    string output;
    output.reserve(4096);
    for(int i=1; i<argc; ++i){
        const char* filename = argv[i];

        File f(filename);
        string buf;
        buf.reserve(4096);
        f.read2End(buf);

        p.process_string(buf, output);
    }

    printf("%s\n", output.c_str());

    return 0;
} 

