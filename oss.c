 //Jenifer Wasson
 //Operating Systems
 //Project 5
 
 //Sources used:  rosettacode.org-banker's algorithm: modified for use in my program
 
#include <semaphore.h>
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
  #include <fcntl.h>  




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

struct FramesTable{
     int useBit;
     int address;
     int DirtyBit;
     };
struct PageTable{
     int pageIndex;
     int address[12];
     int frameNo[12];
     };
struct Memory  *shmPTR;
bool signal_interrupt = false;


void  ALARMhandler(int sig)
{ if(signal_interrupt == false)
  write (STDOUT_FILENO,"Alarm went off\n",16);
        signal(SIGALRM, SIG_IGN);
   signal_interrupt = true;
  signal(SIGALRM, ALARMhandler); 
  
}

 int main(int argc, char* argv[])
  {
     struct FramesTable framestables[12];
     int framesno = 0;
     struct PageTable pagetable[12];
     int noProcesses = 0;     
     FILE *fp;
     int y;     
     sem_t *sem;
     int milliseconds = 0;
     int childCount = 0;
     int value = 0;
     pid_t childID;
     long int getrand = getpid();
     signal(SIGALRM, ALARMhandler);
     int x;
     int RefPointer = 1;
     int ShmID;
     int boundmill = 0;
     int i,j;
     int found = 0;

     for(i = 0; i < 11; i++){
        for(j = 0; j < 11; j++){
            pagetable[i].address[j] = 0;
            pagetable[i].frameNo[j] = 0;}
        pagetable[i].pageIndex = 0;
        framestables[i].useBit = 0;
        framestables[i].address = 0;
        framestables[i].DirtyBit = 0;}
       
     key_t ShmKEY;
     alarm(2); //program can only run 2 seconds;
      ShmKEY = ftok(".", 'x');
     ShmID = shmget(ShmKEY, sizeof(struct Memory), IPC_CREAT | 0666);
     if (ShmID < 0) {
          printf("*** shmget error (server) ***\n");
          exit(1);
     }
    
    shmPTR = (struct Memory *) shmat(ShmID, NULL, 0);
    if (shmPTR == NULL) {
      printf("*** shmat error (server) ***\n");
      exit(1);
    } 
    shmPTR->Release = -2;
    shmPTR->seconds = 1;
    shmPTR->nanoseconds = 0;
    shmPTR->processID = 0;
     sem = sem_open ("sem1115", O_CREAT | O_EXCL, 0644, 1);
      sem_close(sem);
       
       fp= fopen("logfile.txt", "a");
       if (fp == NULL) {
        fprintf(stderr, "Can't open output file %s!\n",
          "logfile.txt");
        exit(1);
       }               
        
      while(1){if(signal_interrupt == true) break;
          
          //if(noProcesses > 18) break;
          if((milliseconds >= boundmill)&&(noProcesses < 1)){
            childCount++;  noProcesses++; srand(getrand++);
            
            srand(getrand++); //fprintf(stderr, "child count is %d\n", shmPTR->processID);
            value = 1 + (rand()%500); //fork every 1 to 500 milliseconds
        boundmill = shmPTR->seconds*1000 + (int)(shmPTR->nanoseconds/1000000) + value;
       
            if ((childID = fork()) == 0) {
                char *args[]={"./user",NULL};
                execvp(args[0],args);
         
        
        perror("child failed to execvp the command");
        return 1;}} 
      

           else{ //add time and run second chance algorithm
             
    
           milliseconds = (1000*shmPTR->seconds) + (int)(shmPTR->nanoseconds/1000000);
          for(y = 0; y < shmPTR->termNum; y++){sem = sem_open("sem1115", 0); sem_wait(sem);
           //see if children have exited
              fprintf(stderr, "Process %d is exiting at time %d"":""%lld\n", shmPTR->TerminatedProc[y],shmPTR->seconds, shmPTR->nanoseconds);
               shmPTR->Release = -2; shmPTR->TerminatedProc[y] = -2;
               shmPTR->termNum = 0;sem_post(sem); sem_close(sem); childCount--;}

          //fprintf(stderr, "release is %d\n", shmPTR->Release);
          if(shmPTR->Release != -2){  //if a child is in the critical region
               sem = sem_open("sem1115", 0); sem_wait(sem); 
               if(shmPTR->Release == 0){
                 //fprintf(stderr,"Process %d requests %d at time %d"":""%lld \n", shmPTR->RequestID,shmPTR->Requests[1],shmPTR->seconds, shmPTR->nanoseconds);
                 if(framesno <= 11){
                 for(i = 1; i  < 11; i++){
                    if(i == shmPTR->RequestID){
                      for(j = 0; j < 12; j++){
                       if(pagetable[i].address[j] == shmPTR->Requests[1]/1000){ 
                           found = 1; framestables[pagetable[i].frameNo[j]].useBit = 1; break;}}}
                      if (found == 1) break;}
                       
                   if((framestables[framesno].address == 0)&&(found == 0)){ 
                       framestables[framesno].address = shmPTR->Requests[1]/1000;
                       pagetable[shmPTR->RequestID].address[pagetable[shmPTR->RequestID].pageIndex] = shmPTR->Requests[1]/1000;
                       pagetable[shmPTR->RequestID].frameNo[pagetable[shmPTR->RequestID].pageIndex] = framesno;
                       pagetable[shmPTR->RequestID].pageIndex++;
                       fprintf(stderr,"Frames address is %d\n", framestables[framesno].address);
                       framestables[framesno].useBit = 1;
                       framesno++; RefPointer++;}}}
             
            shmPTR->Release = -2; found = 0;  sem_post(sem); sem_close(sem);}
                       
            long long int nanoseconds = 0;
           while(nanoseconds <  10){
            nanoseconds = nanoseconds + 1;

           }
            shmPTR->nanoseconds = shmPTR->nanoseconds + nanoseconds;
           while(shmPTR->nanoseconds >= 1000000000){
              shmPTR->seconds++;
              shmPTR->nanoseconds = shmPTR->nanoseconds - 1000000000;}}
       }
             do{ if(signal_interrupt == true) break;
       printf("Clock ticking..\n");
       sleep(1);
      }while (true);
       
       
      shmdt((void *) shmPTR);
       sem_close(sem);
      
       sem_unlink("sem1115");
         
       fclose(fp);
       shmctl(ShmID, IPC_RMID, NULL);
       killpg(getpgid(getpid()), SIGTERM);                //                         printf("Server has removed its shared memory...\n");
      //                                               printf("Server exits...\n");
      //                                                    
                                                               
                                                                    exit(EXIT_SUCCESS);
                                                                         
                                                                           }
      //
