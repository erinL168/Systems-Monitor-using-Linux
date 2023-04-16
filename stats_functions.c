#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <utmp.h>
#include <unistd.h>
#include <math.h>
#include <ctype.h>
#include <signal.h>


int isNumber(char *s)
{//checks if the number provided by isolateCom is valid as a sample or time delay
    
    for (int i = 0; s[i]!= '\0'; i++)
    {
        if (isdigit(s[i]) == 0){
            //printf("hello");
            return 0;
        }
    }
    return 1;
}

void isolateCom(int argc, char **argv, int *seq, int *flag, int *samples, int *t_delay, int *graphics){
    /*Purpose: assigns int values for each pointer parameter 
        seq: 1 if sequential flag is provided, 0 if not
        flag: 
           - 0: default (prints default)
           - 1: --system flag
           - 2: --user flag
        samples: stores how many samples the user wants
        t_delay: stores how many seconds for each delay


    */

    if (argc < 1){
        *flag = 0;
    }

    if (argc > 2){//takes in the samples and time delay values
        
        if (isNumber((char *)argv[1]) ==1 && isNumber((char*)argv[2])== 1){
                       
	        sscanf((char *)argv[1], "%d", samples);
            if (*samples < 0){
                *samples = 1;
            }

            sscanf((char*)argv[2], "%d", t_delay);
            if (*t_delay < 0){
                *t_delay = 1;
            }

        }
        else if (isNumber((char *)argv[1]) ==1){
                       
	        sscanf((char *)argv[1], "%d", samples);
            if (*samples < 0){
                *samples = 1;
            }

        }
    }


    
    for (int i = 0; i < argc; i++){

        
        if (strcmp(argv[i], "--system") == 0){
            *flag = 1;
        }
        else if (strcmp(argv[i], "--user") == 0){
            *flag = 2;
        }
        else if (strcmp(argv[i], "--sequential") == 0){
            *seq = 1;
            
	    }
        else if (strstr(argv[i], "--samples=") != NULL){
            
            const char* s= &argv[i][10];
	        sscanf(s, "%d", samples);
            if (*samples < 0){
                *samples = 1;
            }

	    
        }
        else if (strstr(argv[i], "--tdelay=") != NULL){
            const char* s= &argv[i][9];
	        sscanf(s, "%d", t_delay);
            if (*t_delay < 0){
                *t_delay = 1;
            }
        }
        else if (strcmp(argv[i], "--graphics")== 0 || strcmp(argv[i], "-g") == 0){
            *graphics = 1;
        }
    
    }




}

void readCPU(long *cpu_arr){
    /* Purpose: reads the /proc/stat file and stores the information in cpu_arr
        Note: this function works together with cpu_info and feeds it the values given from /proc/stat

        cpu_arr: array that stores the first line of /proc/stat

    */

    FILE* file =fopen("/proc/stat", "r");
    fscanf(file, "cpu %ld %ld %ld %ld %ld %ld %ld ", &cpu_arr[0], &cpu_arr[1],&cpu_arr[2], &cpu_arr[3], &cpu_arr[4],&cpu_arr[5], &cpu_arr[6]);
    
    
    fclose(file);
    
}

void graphics_cpu(double *cpu_arr, double cpu, int iteration){
    //this is the non sequential implementation
    cpu_arr[iteration] = cpu;
    
    for (int i = 0; i < iteration+1; i++){
        int cpu_num = cpu_arr[i];
        for (int j = 0;j < 2+cpu_num; j++){
            printf("|");
        }
        printf(" %.2f \n", cpu_arr[i]);
    }
    printf("\n");

}


float cpu_info(long *past_total, long *past_idle, int *cores){
    /* Purpose: prints the total number of cpu cores and total cpu usage
     *
     * samples: # of samples decided by user
     * seconds: # of seconds decided by user
     * graphics:
     *
     *
     *
     */

    //printf("------------------------------------------\n");

    //plan
    /*
      - make sample sized array for graphics, then have it save the chars for cpu
      - what about sequential graphics?, probably for each thing then??
    */

    int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
    *cores = num_cores;

    //printf("Number of cores: %d\n", cores);


    long cpu_arr [8] = {0, 0, 0, 0, 0, 0, 0, 0};
    readCPU(&cpu_arr[0]);


    long total = (cpu_arr[0] + cpu_arr[1] + cpu_arr[2] + cpu_arr[3] + cpu_arr[4] + cpu_arr[5] + cpu_arr[6] + cpu_arr[7]);
    long idle = cpu_arr[3];

    long past_tot = *past_total;
    long past_id = *past_idle;

    float cpu = 0.0;
    if (total != past_tot) {
        cpu = (float)(total - idle - past_tot + past_id) / (total - past_tot);
    }
    cpu *= 100;

    *past_idle = idle;
    *past_total = total;

    // printf(" parent pt: %ld", *past_idle);
    // printf(" pi: %ld", *past_total);

    return cpu;
}


void samplesMemory(int samples, int seconds, int seq, int iteration){
    /* Prints number of samples and time delay as well as memory usage of the current running program
     * 
     * samples: number of samples
     * seconds: time delay, time between each sample
     *
     */
	
    struct rusage file_data;
    getrusage(RUSAGE_SELF,&file_data); 
    if (seq == 0){
        printf("Number of Samples: %d -- every %d secs\n", samples, seconds);

    }
    else if (seq == 1){
        printf(">>> iteration %d\n", iteration);
    }
    printf(" Memory Usage: %ld kilobytes\n",file_data.ru_maxrss);
    printf("------------------------------------------\n");

}

void graphics_mem(float difference, float p_used, float pUsed){
/*

*/



    if (pUsed == -1.0){
        printf("*");
        printf(" 0.00 ");
        printf("(%.2f)\n", p_used);
        return;
    }

    int repeat = fabs(difference * 100);

            //double check with ta to see if this is correct
            
    if (p_used > pUsed){
        for (int i = 0; i < repeat; i++){
            printf("#");
        }
        printf("*");
        
    }
    else{
        for (int i = 0; i < repeat; i++){
            printf(":");
        }
        printf("@");
    }


    printf(" %.2f ", difference);
    printf("(%.2f)\n", p_used);
}


void memory_info(float *ind_mem_arr){
    /* Function: memory_info 
     *     Prints the memory portion of the program
     * 
     *    More on parameter in documentation
     */
	
    struct sysinfo memory_data;
    sysinfo(&memory_data);

    //printf("------------------------------------------\n");
    //printf("### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");

    //Accesses the memory data needed for memory calculation
    float ram = memory_data.totalram/(1024.0*1024.0*1024.0); 
    float freeram = memory_data.freeram/(1024.0*1024.0*1024.0); 
    float totalswap = memory_data.totalswap/(1024.0*1024.0*1024.0); 
    float freeswap = memory_data.freeswap/(1024.0*1024.0*1024.0); 

    //calculates each category, used physical, total physical, used virtual, total virtual memory respectively
    float p_used = ram - freeram;
    float p_total = ram;
    float v_used = (ram - freeram)+(totalswap - freeswap);
    float v_total = ram + totalswap;
    
    
    *(ind_mem_arr + 0) = p_used;
    *(ind_mem_arr + 1) = p_total;
    *(ind_mem_arr + 2) = v_used;
    *(ind_mem_arr + 3)= v_total;

    //write(fd_mem[1], &ind_mem_arr, sizeof(ind_mem_arr));
    
    return;

}

void system_info(){
    /*Purpose: prints out system information
    */
    
    struct utsname system_data;
    uname(&system_data);
    

    printf("------------------------------------------\n");
    printf("### System Information ###\n");
    printf("System Name  : %s\n", system_data.sysname);
    printf("Machine Name : %s\n", system_data.nodename);
    printf("Version      : %s\n", system_data.version);
    printf("Release      : %s\n", system_data.release);
    printf("Architecture : %s\n", system_data.machine);

    printf("------------------------------------------\n");
}

void userC(pid_t fd_users[2]){
    //Purpose: prints out current users on server
    
    struct utmp* data;
    char utorids[50]; //gets names or usernames, UTORIDs
    data = getutent(); //gets data from function

    struct utmp data_copy;
    while(data != NULL ) //for all data
    {
        strncpy(utorids, data->ut_user, 32);
        utorids[31] = '\0';

        if (strcmp(utorids, "") != 0){
            if ((strcmp(utorids, "reboot") != 0) && (strcmp(utorids, "LOGIN") != 0) && (strcmp(utorids, "runlevel")!= 0)){
                memcpy(&data_copy, data, sizeof(struct utmp));
                data_copy.ut_user[sizeof(data_copy.ut_user) - 1] = '\0'; // make sure the string is null-terminated
                write(fd_users[1], &data_copy, sizeof(struct utmp));
            }
        }
        data = getutent();
    }
    //printf("This is reached.\n");
    // Initialize data_copy to all zeroes
    memset(&data_copy, 0, sizeof(struct utmp));
    write(fd_users[1], &data_copy, sizeof(struct utmp));

    setutent(); // to close the utmp file
}

void userC_print(pid_t fd_users[2]){
    //Purpose: prints for parent the session users by reading the data struct from userC()
    printf("%s\n", "### Sessions/users ###");

    //char utorids[50];
    struct utmp data;

    read(fd_users[0], &data, sizeof(struct utmp));
    while (strlen(data.ut_user) != 0) {
        read(fd_users[0], &data, sizeof(struct utmp));
        data.ut_user[sizeof(data.ut_user) - 1] = '\0'; // make sure the string is null-terminated
        printf("  %s     %s (%s) \n", data.ut_user, data.ut_line, data.ut_host);
    }
    printf("------------------------------------------\n");
}



void initialize(float *mem_arr, int samples){
    //Purpose: this function initializes all the memory array data to 0
    for (int i = 0; i < samples; i++){
        for (int j = 0; j < 4; j++){
            *((mem_arr+i*4) + j) = -1.0;
        }
    }
}

void ignore_signal(int signal_number) {
    // Handler for Ctrl-Z signal. 
    return;
}

void quit_signal(int signal_number) {
    /* Handler for Ctrl-C signal. */
    char answer;

    printf("\nDo you really want to quit? [y/n] ");
    if (scanf(" %c", &answer) == 0){
        perror("User Input");
    } 

    if (answer == 'y' || answer == 'Y') {
        exit(0);
    } else {
        /* Re-register the signal handler to catch the signal again. */
        signal(SIGINT, quit_signal);
    }
}



int main(int argc, char **argv){
    //get signals for ctrl z and ctrl c
    

    //default values
    int samples = 10;
    int t_delay = 1;
    int flag = 0;
    int seq = 0;
    int graphics = 0;
    

    //get arguments
    isolateCom(argc, argv, &seq, &flag, &samples, &t_delay, &graphics);


    //initial cpu and values for memory
    long cpu_arr [8] = {0, 0, 0, 0, 0, 0, 0};
    readCPU(&cpu_arr[0]);
    long cpu_use = cpu_arr[0]+ cpu_arr[1]+ cpu_arr[2] +cpu_arr[3] + cpu_arr[4] + cpu_arr[5] + cpu_arr[6];
    long idle = cpu_arr[3];

    float pUsed = -1.0;
    
    
    
//initialization of array for seq array 
    
    float mem_arr[samples][4];
    initialize((float *)mem_arr, samples);
    float ind_mem_arr[4];

//initialization of cpu array 
    float cpu = 0.0; //for parinting in parent
    double cpu_graphics[samples];
    for (int i = 0; i < samples; i++){
        cpu_graphics[i] = -1.0;
    }
    int cores = 0;
    




    int fd_users[2], fd_mem[2], fd_cpu[2];
    pid_t pid_users, pid_mem, pid_cpu;


    // create the pipes
    if (pipe(fd_users) == -1 || pipe(fd_mem) == -1 || pipe(fd_cpu) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // fork the processes
    pid_users = fork();

    if (pid_users == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid_users == 0) {
        // child process 1
        close(fd_users[0]); // close the read end of pipe 1

      

         for (int i = 0; i < samples; i++) {

            userC(fd_users);
            
            
            sleep(t_delay); // wait for one second
        }

        close(fd_users[1]); // close the write end of pipe 1
    } else {
        // fork the second child process
        pid_mem = fork();

        if (pid_mem == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid_mem == 0) {
            // child process 2 - memory
            close(fd_mem[0]); // close the read end of pipe 2



             for (int i = 0; i < samples; i++) {
               
                memory_info((float*)ind_mem_arr);
                //write(fd_mem[1], &ind_mem_arr, sizeof(ind_mem_arr));
                if (write(fd_mem[1], ind_mem_arr, sizeof(ind_mem_arr)) == -1) {
                    perror("Memory write Pipe");
                } // write float value to the pipe


                sleep(t_delay); // wait for one second
            }

            close(fd_mem[1]); // close the write end of pipe 2
        } else {
            
            // fork the third child process
            pid_cpu = fork();

            if (pid_cpu == -1) {
                perror("fork");
                exit(EXIT_FAILURE);
            }

            if (pid_cpu == 0) {
                // child process 3
                close(fd_cpu[0]); // close the read end of pipe 3



                 for (int i = 0; i < samples; i++) {
                    
                    float cpu = cpu_info(&cpu_use, &idle, &cores);



                    if (write(fd_cpu[1], &cpu, sizeof(cpu)) == -1) {
                        perror("cpu write Pipe");
                    } // write float value to the pipe

                    if (write(fd_cpu[1], &cpu_use, sizeof(cpu_use)) == -1){
                        perror("cpu write pipe");
                    }  // write integer value to the pipe
                    if (write(fd_cpu[1], &idle, sizeof(idle)) == -1){
                        perror("idle cpu write pipe");
                    } 
                    if (write(fd_cpu[1], &cores, sizeof(cores)) == -1){
                        perror("cpu write pipe");
                    } 



                    sleep(t_delay); // wait for one second
                }

                close(fd_cpu[1]); // close the write end of pipe 3



/*   ##########################################################                 


            PARENT PROCESS

      ##########################################################
*/



            } else {
                //parent process
                //get signals for ctrl z and ctrl c
                signal(SIGTSTP, ignore_signal);
                signal(SIGINT, quit_signal);
            

                // parent process
                close(fd_users[1]); // close the write end of pipe 1
                close(fd_mem[1]); // close the write end of pipe 2
                close(fd_cpu[1]); // close the write end of pipe 3


                for (int i = 0; i < samples; i++){
                    sleep(t_delay);
                    if (seq == 0){
                        system("clear");
                    }
                    samplesMemory(samples, t_delay, seq, i);

                    if(flag == 2||flag == 0){
                        //prints out users
                        userC_print(fd_users);
                        


                    }
                    if (flag ==1 || flag == 0){
                        

                        read(fd_mem[0], &ind_mem_arr, sizeof(ind_mem_arr));
                       // parent_print_memory(seq, graphics, pUsed, samples, i, ind_mem_arr);
                        // make into function if time seq, graphics, pUsed, samples, i, ind_mem_arr, 
                        printf("### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");
                        if (seq == 1){
                            for (int j = 0; j < i; j++){
                                printf("\n");
                            }
                            printf("%.2f GB / %.2f GB -- %.2f GB / %.2f GB ", ind_mem_arr[0], ind_mem_arr[1], ind_mem_arr[2], ind_mem_arr[3]);

                            if (graphics==1){
                                printf("|");
                                float difference = ind_mem_arr[0] - pUsed;
                                graphics_mem(difference, ind_mem_arr[0], pUsed);
                                //printf("| %.2f | difference %f", *pUsed, difference);
                                pUsed = ind_mem_arr[0];
                            }
                            else{
                                printf("\n");
                            }
                            



                            for (int j = 0; j < samples - i -1;j++){
                                printf("\n");
                            }
                        }
                        //if sequential flag is not present, stores previous memory information into an array 
                        else{ //stores current memory information
                            
                            *(((float*)mem_arr + i*4)+0) = ind_mem_arr[0];
                            *(((float*)mem_arr + i*4)+1) =  ind_mem_arr[1];
                            *(((float*)mem_arr + i*4)+2) =  ind_mem_arr[2];
                            *(((float*)mem_arr + i*4)+3) =  ind_mem_arr[3];
                            for (int j = 0; j < i+1; j++){ //prints previous memory
                                printf("%.2f GB / %.2f GB -- %.2f GB / %.2f GB    ", *(((float*)mem_arr + j*4)+0), *(((float*)mem_arr + j*4)+1), *(((float*)mem_arr + j*4)+2), *(((float*)mem_arr + j*4)+3));
                                
                                if (graphics== 1){
                                //printf("| %.2f | difference %f", *pUsed, difference);
                                    printf("|");
                                    if (j == 0){
                                        
                                        float difference = 0.0;
                                        graphics_mem(difference, *(((float*)mem_arr + j*4)+0), pUsed);
                                        pUsed = *(((float*)mem_arr + j*4)+0);
                                    }
                                    else{
                                        pUsed = *(((float*)mem_arr + (j-1)*4)+0);
                                        float difference = *(((float*)mem_arr + j*4)+0) - pUsed;
                                        graphics_mem(difference, *(((float*)mem_arr + j*4)+0), pUsed);
                                        
                                    }
                                }
                                else{
                                    printf("\n");
                                }
                            }
                            
                                
                            
                            for (int j = 0; j < samples - i -1;j++){//prints remaining spaces for the remaining iterations
                                    printf("\n");
                                }


                        
                        }
                        

                    

                        printf("------------------------------------------\n");



                        //cpu parent read

                        read(fd_cpu[0], &cpu, sizeof(cpu));
                        read(fd_cpu[0], &cpu_use, sizeof(cpu_use));
                        read(fd_cpu[0], &idle, sizeof(idle));
                        read(fd_cpu[0], &cores, sizeof(cores));
                        

                        printf("Number of cores: %d\n", cores);

                         printf(" total cpu use = %.2f %%\n", cpu);
                        if (graphics == 1){
                            graphics_cpu(cpu_graphics, cpu, i);
                        }



                        close(fd_cpu[0]);

                    }
                        

                    system_info();
                    

                }


                close(fd_users[0]); // close the read end of pipe 1
                close(fd_mem[0]); // close the read end of pipe 2
                close(fd_cpu[0]); // close the read end of pipe 3
            }
        }
    }

    return 0;

}
