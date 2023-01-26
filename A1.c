#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <utmp.h>

#include <unistd.h>

int isolateCom(int argc, char **argv){
    //come back to this later to fix up edge cases

    if (argc < 2){
        return -1;
    }
    else if (strcmp(argv[1], "--system") == 0){
        return 1;
    }
    else if (strcmp(argv[1], "--user") == 0){
        return 2;
    }
    else if (strcmp(argv[1], "--graphics") == 0){
        return 3;
    }
    else if (strcmp(argv[1], "--sequential") == 0){
        return 4;
    }
    else if (strstr(argv[1], "--samples=") != NULL){

        return 5;
    }
    else if (strcmp(argv[1], "--system") == 0){
        return 6;
    }
    return -1;



}

void defaultC(){

}



void userC(){
    struct utmp* data;
    char aux[50];
    data = getutent();
    //getutent() populates the data structure with information 
    while(data != NULL )
    {
        /*make sure to use strncpy 
         *because most data fiels in the utmp struct 
         *have ___nonstring___ atribute */
        strncpy(aux, data->ut_user, 32);
                aux[32] = '\0';
        printf("ut_user: %s\n", aux);
        printf("ut_type: %hi\n\n", data->ut_type);
        data = getutent();
    }
    
}

int main(int argc, char **argv){

    //char *input =  strtol(*argv, NULL, 10);

    int num = isolateCom(argc, argv);
    printf("%s | %d", argv[1], num);

    if (num == 0){defaultC(); }
    else if (num ==1){systemC(); }
    else if (num == 2){userC(); }
    else if (num == 3){graphicsC(); }
    else if (num == 4){sequentialC(); }
    else if (num == 5){samplesC();}
    else if (num == 6){tdelayC();}
    else{
        printf("%s", "INVALID COMMAND");
    }


    //function for --system'

    //function for --user

    //function for --graphics

    //function for --sequential

    //function for --samples=N

    //function for --tdelay-T

    return 0;
}

