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

void readCPU(int *cpu_arr){
    FILE* file =fopen("/proc/stat", "r");
    fscanf(file, "cpu %d %d %d", &cpu_arr[0], &cpu_arr[1],&cpu_arr[2]);
    fclose(file);
}

void cpu_info(int samples, int seconds, int graphics){
    /* prints the total number of cpu cores and total cpu use
     *
     * samples:
     * seconds:
     * graphics:
     *
     *
     *
     */

    //printf("------------------------------------------\n");

    int cores = sysconf(_NPROCESSORS_ONLN);
    printf("Number of cores: %d\n", cores);

    //int prev = 0;
    //int curr = 0;

    //for (int i = 0; i < samples; i++){

        //sleep(seconds);
   // }


    //printf(" total cpu use = %ld %%\n",  sysconf(_POSIX_THREAD_PROCESS_SHARED));
    //printf("%d \n", getrlimit(RLIMIT_CPU));


    printf("\n");
    int cpu_arr [3] = {0, 0, 0};
    readCPU(&cpu_arr[0]);
    printf("1: %d %d %d \n", cpu_arr[0], cpu_arr[1], cpu_arr[2]);
    sleep(2);
    readCPU(cpu_arr);
    printf("2: %d %d %d \n", cpu_arr[0], cpu_arr[1], cpu_arr[2]);
}




void samplesMemory(int samples, int seconds){
    /* Prints number of samples and time delay as well as memory usage of the current running program
     *
     * samples: number of samples
     * seconds: time delay, time between each sample
     *
     */

    struct rusage file_data;
    getrusage(RUSAGE_SELF,&file_data);

    printf("Number of Samples: %d -- every %d secs\n", samples, seconds);
    printf(" Memory Usage: %ld kilobytes\n",file_data.ru_maxrss);
    printf("------------------------------------------\n");
}


void memory_info(){
    /* Function: memory_info
     *     Prints the memory portion of the program
     *
     * Note on Calculation: (row, column) referencing linux's free
     *     Total Physical RAM: total (mem, total)
     *     Used Physical Ram: total - free
     *
     *     Total Virtual RAM: Mem + swap (total, total)
     *     Used Virtual RAM: total - free
     *
     */

    struct sysinfo memory_data;
    sysinfo(&memory_data);

    //printf("------------------------------------------\n");
    printf("### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");

    //printf("loads average: %ln \n", memory_data.loads);
    float ram = memory_data.totalram/1073741824.0;
    float freeram = memory_data.freeram/1073741824.0;
    //float sharedram = memory_data.sharedram/1073741824.0;
    //float bufferram = memory_data.bufferram/1073741824.0;
    float totalswap = memory_data.totalswap/1073741824.0;

    //float freeswap = memory_data.freeswap/1073741824.0;
    //float procs = memory_data.freeswap/1073741824.0;


    for (int i = 0; i < 10; i++){
        printf("%.2f GB / %.2f  -- %.2f GB / %.2f GB    | ?? \n", ram - freeram, ram, ram - freeram,ram + totalswap);

        //sleep(1);
    }


    //printf("total usable memory size/ total ram: %.2f\n GB", ram);
    //printf("available memory size/ freeram:  %.2f\n", freeram);
    //printf("amount of shared memory/ sharedram:  %.2f\n", sharedram);
    //printf("memory used by buffers/ buggerram:  %.2f\n", bufferram);
    //printf("total swap space ram/totalswap :  %.2f\n", totalswap);
    //printf("swap: %.2lf\n", freeswap);
    //printf("pads: %.2lf\n", procs);


    printf("------------------------------------------\n");
}

void system_info(){

    struct utsname system_data;
    uname(&system_data);


    printf("------------------------------------------\n");
    printf("### System Information ###");
    printf("System Name  : %s\n", system_data.sysname);
    printf("Machine Name : %s\n", system_data.nodename);
    printf("Version      : %s\n", system_data.version);
    printf("Release      : %s\n", system_data.release);
    printf("Architecture : %s\n", system_data.machine);

    printf("------------------------------------------\n");
}



void userC(){
    //still not complete, remember to add comments, parameters etc
    //printf("------------------------------------------\n");

    struct utmp* data;
    char utorids[50]; //gets names or usernames, UTORIDs
    data = getutent();

    printf("%s\n", "### Sessions/users ###");
    while(data != NULL )
    {
        strncpy(utorids, data->ut_user, 32);
                utorids[32] = '\0';


        if (strcmp(utorids, "") != 0 ){
                printf("  %s     %s (%s) \n", utorids, data->ut_line, data -> ut_host);

        }
        data = getutent();
    }

    printf("------------------------------------------\n");
}

int main(int argc, char **argv){

    //char *input =  strtol(*argv, NULL, 10);

    //int num = isolateCom(argc, argv);
    //printf("%s | %d", argv[1], num);

    //if (num == 1){defaultC(); }
    // else if (num ==1){systemC(); }
    //else if (num == 2){userC(); }
    // else if (num == 3){graphicsC(); }
    // else if (num == 4){sequentialC(); }
    // else if (num == 5){samplesC();}
    // else if (num == 6){tdelayC();}
    //else{
    //    printf("%s", "INVALID COMMAND");
    //}
   // refresh();
    system("clear");
    samplesMemory(10, 1);

    userC();
    memory_info();
    cpu_info(10, 1, 1);
    system_info();

    //function for --system'

    //function for --user

    //function for --graphics

    //function for --sequential

    //function for --samples=N

    //function for --tdelay-T

    return 0;
}
