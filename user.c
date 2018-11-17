#include <string.h>
  #include <ctype.h>
  #include  <sys/shm.h>
  #include <sys/types.h>
  #include <sys/wait.h>
  #include <unistd.h>
  #include <time.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <signal.h>
  #include <stdbool.h>
  #include <time.h>
  #include <semaphore.h>

struct Memory {
     int MaxClaims;
     int  seconds;
     int Release;
     long long int  nanoseconds;
     int TerminatedProc[2];
     int processID;
     int RequestID;
     int Requests[4];
     int termNum;
};

struct Memory  *shmPTR;
int main() {
  key_t ShmKEY;
  int value = 0;
   int Requests = 0;
   int termValue = 0;
   long int getrand = getpid();
   int boundmil = 0;
   int milliseconds = 0;
   
   int ShmID;
          sem_t *sem;
   //          long long int x = 0;
     int processID = 0;
               
                              ShmKEY = ftok(".",'x');
                                                   ShmID = shmget(ShmKEY, sizeof(struct Memory), 0666);
                                                                           if (ShmID < 0){
                                                                                                        printf("*** shmget error (client) ***\n");
                                                                                                                                         exit(1);
                                                                                                                                                                              }
                                                                                                                                                                                                                       shmPTR = (struct Memory *) shmat(ShmID, NULL, 0);
                                                                                                                                                                                                                                                                   if(ShmID == -1){
                                                                                                                                                                                                                                                                                                                    printf("*** shmat error(client) ***\n");
                                                                                                                                                                                                                                                                                                                                                                          exit(1);
                                                                                                                                                                                                                                                                                                                                                                                                                                      }
     shmPTR->processID++;
     //fprintf(stderr, "Release Value is %d\n", shmPTR->Release);
     srand(getrand++);
     value = 1 + (rand()%100); 
      boundmil =  100*shmPTR->seconds + (int)(shmPTR->nanoseconds/1000000) + value;
     processID = shmPTR->processID;
      
     termValue =  300 + value; 
     while(true){  
            milliseconds = 1000*shmPTR->seconds + (int)(shmPTR->nanoseconds/1000000);
     if((shmPTR->Release == -2)&&(milliseconds >= boundmil)) {
                sem = sem_open("sem1122", 0); sem_wait(sem);
                srand(getrand++); value = 1001 + (rand()%32000);   //fprintf(stderr, "Value is %d\n",value);
                shmPTR->Requests[1] = value; //memory location that it will read/write to 
                shmPTR->Release = 0;
                srand(getrand++); value = 1 + (rand()%100);
                if (value >= 20)
                  shmPTR->Requests[2] = 1; //read
                else shmPTR->Requests[2] = 0; //write
               Requests++;                                                                                                                                                              
               boundmil =   997*shmPTR->seconds + (int)(shmPTR->nanoseconds/1000000) + value;
               shmPTR->RequestID = processID; sem_post(sem); sem_close(sem);
            
       if(Requests > termValue)  break;} //if it's rolled the chance to terminate
         }
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
      sem = sem_open("sem1122", 0); sem_wait(sem);
      //fprintf(stderr,"%s", "child terminated");
      shmPTR->TerminatedProc[shmPTR->termNum] = processID;
      shmPTR->termNum++; sem_post(sem); sem_close(sem);
       shmdt((void *) shmPTR);
    exit(0);
}
