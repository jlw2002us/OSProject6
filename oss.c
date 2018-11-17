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
     int address[15];
     int frameNo[15];
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
     
     struct PageTable pagetable[20];
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
     int RefPointer = 0;
     int ShmID;
     int boundmill = 0;
     int i,j;
     int found = 0;
     long long int nanoseconds = 0;
     bool full = true; //memory frames table full or not
     bool pageFound = false;

     for(i = 0; i < 12; i++){
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
     sem = sem_open ("sem1120", O_CREAT | O_EXCL, 0644, 1);
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
          for(y = 0; y < shmPTR->termNum; y++){sem = sem_open("sem1120", 0); sem_wait(sem);
           //see if children have exited
              fprintf(stderr, "Process %d is exiting at time %d"":""%lld\n", shmPTR->TerminatedProc[y],shmPTR->seconds, shmPTR->nanoseconds);
               shmPTR->Release = -2; shmPTR->TerminatedProc[y] = -2;
               shmPTR->termNum = 0;sem_post(sem); sem_close(sem); childCount--;}

          //fprintf(stderr, "release is %d\n", shmPTR->Release);
          if(shmPTR->Release != -2){  //if a child is in the critical region
               sem = sem_open("sem1120", 0); sem_wait(sem); 
               if(shmPTR->Release == 0){
                 
                 //fprintf(stderr,"Process %d requests %d at time %d"":""%lld \n", shmPTR->RequestID,shmPTR->Requests[1],shmPTR->seconds, shmPTR->nanoseconds);
                 
                    for(i = 1; i < 13; i++){ //check page tables for request
                      if(i == shmPTR->RequestID){
                        for(j = 0; j < 10; j++){
                          if((pagetable[i].address[j] == shmPTR->Requests[1]/1000)&&(pagetable[i].frameNo[j] != -2)){ 
                             found = 1; 
                             framestables[pagetable[i].frameNo[j]].useBit = 1; 
                              nanoseconds = 0;
                               while(nanoseconds <  10){
                                nanoseconds = nanoseconds + 1;

                                }
                                break;}}}
                        if (found == 1) break;}
                    for(i = 0; i < 10; i++){  //put in memory frames if there's room
                      if((framestables[i].address == 0)&&(found == 0)){
                        while(nanoseconds < 15000000){
                          nanoseconds = nanoseconds + 500;}       
                        framestables[i].address = shmPTR->Requests[1]/1000;
                        
                           pagetable[shmPTR->RequestID].address[pagetable[shmPTR->RequestID].pageIndex] = shmPTR->Requests[1]/1000;
                           pagetable[shmPTR->RequestID].frameNo[pagetable[shmPTR->RequestID].pageIndex] = i;
                           pagetable[shmPTR->RequestID].pageIndex++;
                        //fprintf(stderr,"Frames address is %d\n", framestables[i].address);
                        framestables[i].useBit = 1;
                        RefPointer++; if(RefPointer == 10) RefPointer = 0;full = false; break;}}

                    //swap out a process
                    if((full == true)&&(found == 0)){
                       
                       while(true){
                          if(RefPointer == 10) RefPointer = 0;
                          if(framestables[RefPointer].useBit == 0){
                            while(nanoseconds < 15000000){
                               nanoseconds = nanoseconds + 500;}
                            for(i = 1; i < 13; i++){
                              for(j = 0; j < 10; j++){
                                if(RefPointer == pagetable[i].frameNo[j]){
                                  pagetable[i].frameNo[j] = -2;}//fprintf(stderr,"%s", "page entry changed");}
                             }}
                             for(j =0; j < 10; j++){
                             if((pagetable[shmPTR->RequestID].frameNo[j] == -2) &&(shmPTR->Requests[1]/1000 == pagetable[shmPTR->RequestID].address[j]))
                               { pagetable[shmPTR->RequestID].frameNo[j] = RefPointer; pageFound = true;}}
                             if(pageFound == false){
                                pagetable[shmPTR->RequestID].address[pagetable[shmPTR->RequestID].pageIndex] = shmPTR->Requests[1]/1000;
                                pagetable[shmPTR->RequestID].frameNo[pagetable[shmPTR->RequestID].pageIndex] = RefPointer;
                                pagetable[shmPTR->RequestID].pageIndex++;}

                             framestables[RefPointer].address = shmPTR->Requests[1]/1000;
                            //fprintf(stderr,"Process %d is swapped in at frame %d\n", shmPTR->RequestID,RefPointer);
                             framestables[RefPointer].useBit = 1;  
                            
                                  
                             RefPointer++; break;}
                          
                           else{
                            framestables[RefPointer].useBit = 0;
                            RefPointer++;}
                        }}
                    
            shmPTR->Release = -2; found = 0;  full = true; pageFound = false; sem_post(sem); sem_close(sem);}}
                       
            
           
            shmPTR->nanoseconds = shmPTR->nanoseconds + nanoseconds;
           while(shmPTR->nanoseconds >= 1000000000){
              shmPTR->seconds++;
              shmPTR->nanoseconds = shmPTR->nanoseconds - 1000000000;}}
       }
             do{ if(signal_interrupt == true) break;
       printf("Clock ticking..\n");
       sleep(1);
      }while (true);
       
      for(i = 0; i < 10; i++){
        fprintf(stderr,"Page frame is %d and page address is %d\n", pagetable[1].frameNo[i], pagetable[1].address[i]);}
      shmdt((void *) shmPTR);
       sem_close(sem);
      
       sem_unlink("sem1120");
         
       fclose(fp);
       shmctl(ShmID, IPC_RMID, NULL);
       killpg(getpgid(getpid()), SIGTERM);                //                         printf("Server has removed its shared memory...\n");
      //                                               printf("Server exits...\n");
      //                                                    
                                                               
                                                                    exit(EXIT_SUCCESS);
                                                                         
                                                                           }
      //
