#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// #include <sys/resource.h>
// #include <sys/utsname.h>
// #include <sys/sysinfo.h>
// #include <sys/types.h>
// #include <utmp.h>

#include <unistd.h>

int main(int argc, char **argv){

    char *input =  strtol(*argv, NULL, 10);

    printf("%s", input);




    //function for --system'

    //function for --user

    //function for --graphics

    //function for --sequential

    //function for --samples=N

    //function for --tdelay-T

    return 0;
}

