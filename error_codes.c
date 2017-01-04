#include "error_codes.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void error(const char *msg){
    fprintf(stderr,"%s\n",msg);
    
}