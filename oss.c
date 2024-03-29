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
     int address[35];
     int frameNo[35];
       };
 
struct Stats{
     
     int memoryAccesses;
     int startTime;
     int endTime;
     int noFaults;
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
     struct FramesTable framestables[260];
     struct Stats stats[40];
     int index = 0;
     struct PageTable pagetable[40];
     int noProcesses = 0;     
     FILE *fp;
     int y;  int processNo = 0;   
     sem_t *sem;
      int milliseconds = 0;
     int childCount = 0;
     int value = 0;
     float speedSum = 0.0;
     pid_t childID;
     long int getrand = getpid();
     signal(SIGALRM, ALARMhandler);
     int RefPointer = 0;
     int ShmID;
     int boundmill = 0;
     int i,j;
     int found = 0;
     int printCount = 0;
     long long int nanoseconds = 0;
     bool full = true; //memory frames table full or not
     bool pageFound = false;


     for(i = 0; i < 256; i++){
        framestables[i].useBit = 0;
        framestables[i].address = 0;
        framestables[i].DirtyBit = 0;}

     for(i = 0; i < 40; i++){
        for(j = 0; j < 35; j++){
            pagetable[i].address[j] = 0;
            pagetable[i].frameNo[j] = -1;
            }
         stats[i].startTime = 0;
         stats[i].memoryAccesses = 0;
         stats[i].noFaults = 0;
         stats[i].endTime = 0;
         pagetable[i].pageIndex = 0;
        }

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
     sem = sem_open ("sem1130", O_CREAT | O_EXCL, 0644, 1);
      sem_close(sem);

       fp= fopen("logfile.txt", "a");
       if (fp == NULL) {
        fprintf(stderr, "Can't open output file %s!\n",
          "logfile.txt");
        exit(1);
       }

      while(1){if(signal_interrupt == true) break;

          if((milliseconds >= boundmill)&&(childCount < 18)){
            childCount++;  noProcesses++; srand(getrand++);
            processNo++;
            srand(getrand++); //fprintf(stderr, "child count is %d\n", shmPTR->processID);
            value = 1 + (rand()%500); //fork every 1 to 500 milliseconds
        boundmill = shmPTR->seconds*1000 + (int)(shmPTR->nanoseconds/1000000) + value;
            stats[processNo].startTime = shmPTR->seconds;
            if ((childID = fork()) == 0) { //fprintf(stderr,"%s","hello from child");
                char *args[]={"./user",NULL};
                execvp(args[0],args);


        perror("child failed to execvp the command");
        return 1;}} 


           else{ //add time and run second chance algorithm


           milliseconds = (1000*shmPTR->seconds) + (int)(shmPTR->nanoseconds/1000000);
          for(y = 0; y < shmPTR->termNum; y++){sem = sem_open("sem1130", 0); sem_wait(sem);
               index = shmPTR->TerminatedProc[y]; stats[index].endTime = shmPTR->seconds;
               fprintf(fp, "Process %d is exiting at time %d"":""%lld\n", shmPTR->TerminatedProc[y],shmPTR->seconds, shmPTR->nanoseconds);
               shmPTR->Release = -2; shmPTR->TerminatedProc[y] = -2;
               shmPTR->termNum = 0;sem_post(sem); sem_close(sem); childCount--;}

           if(shmPTR->Release != -2){  //if a child is in the critical region
               sem = sem_open("sem1130", 0); sem_wait(sem); 
               if(shmPTR->Release == 0){
                 printCount++; index = shmPTR->RequestID; stats[index].memoryAccesses++;
                 fprintf(fp,"Process %d requests %d at time %d"":""%lld \n", shmPTR->RequestID,shmPTR->Requests[1],shmPTR->seconds, shmPTR->nanoseconds);

                    for(i = 1; i < 40; i++){ //check page tables for request
                      if(i == shmPTR->RequestID){
                        for(j = 0; j < 32; j++){
                          if((pagetable[i].address[j] == shmPTR->Requests[1]/1000)&&(pagetable[i].frameNo[j] != -2)){
                               if(pagetable[i].frameNo[j] != -1){ 
                                 found = 1;
                                 if(shmPTR->Requests[2] == 1){framestables[pagetable[i].frameNo[j]].DirtyBit = 0;
                                   fprintf(fp,"Process %d is granted to read data at page %d frame %d\n",shmPTR->RequestID, shmPTR->Requests[1]/1000,pagetable[i].frameNo[j]);}
                                 else{ nanoseconds = nanoseconds*2; framestables[pagetable[i].frameNo[j]].DirtyBit = 1;
                                   fprintf(fp,"Process %d is granted to write data at page %d frame %d extra time added\n",shmPTR->RequestID, shmPTR->Requests[1]/1000,pagetable[i].frameNo[j]);}
 
                                 framestables[pagetable[i].frameNo[j]].useBit = 1; 
                                 nanoseconds = 0;

                                nanoseconds = nanoseconds + 10;


                                break;}}}}
                        if (found == 1) break;}
                    for(i = 0; i < 256; i++){  //put in memory frames if there's room
                      if((framestables[i].address == 0)&&(found == 0)){
                       index = shmPTR->RequestID; stats[index].noFaults++;
                        while(nanoseconds < 15000000){ //add 15 milliseconds
                          nanoseconds = nanoseconds + 150000;}       
                        framestables[i].address = shmPTR->Requests[1]/1000;
                        for(j =0; j < 32; j++){
                             if((pagetable[shmPTR->RequestID].frameNo[j] == -2) &&(shmPTR->Requests[1]/1000 == pagetable[shmPTR->RequestID].address[j]))
                               { pagetable[shmPTR->RequestID].frameNo[j] = i; pageFound = true;}}
                        if(pageFound == false){
                           pagetable[shmPTR->RequestID].address[pagetable[shmPTR->RequestID].pageIndex] = shmPTR->Requests[1]/1000;
                           pagetable[shmPTR->RequestID].frameNo[pagetable[shmPTR->RequestID].pageIndex] = i;
                           pagetable[shmPTR->RequestID].pageIndex++;}
                        if(shmPTR->Requests[2] == 1){ framestables[i].DirtyBit = 0;
                          fprintf(fp,"Page fault: Process %d page %d is reading data at frame %d\n", shmPTR->RequestID, framestables[i].address,i);}
                        else{
                          fprintf(fp,"Page fault: Process %d page %d is writing data at frame %d, extra time added.\n", shmPTR->RequestID, framestables[i].address,i);
                          nanoseconds = nanoseconds*2; framestables[RefPointer].DirtyBit = 1;}
                        framestables[i].useBit = 1;
                        RefPointer++; if(RefPointer == 256) RefPointer = 0;full = false; break;}}
                         if((full == true)&&(found == 0)){

                       while(true){
                          if(RefPointer == 256) RefPointer = 0;
                          if(framestables[RefPointer].useBit == 0){
                            index = shmPTR->RequestID; stats[index].noFaults++;
                            while(nanoseconds < 15000000){
                               nanoseconds = nanoseconds + 150000;}
                            for(i = 1; i < 40; i++){
                              for(j = 0; j < 32; j++){
                                if(RefPointer == pagetable[i].frameNo[j]){
                                  pagetable[i].frameNo[j] = -2;}
                             }}
                             for(j =0; j < 32; j++){
                             if((pagetable[shmPTR->RequestID].frameNo[j] == -2) &&(shmPTR->Requests[1]/1000 == pagetable[shmPTR->RequestID].address[j]))
                               { pagetable[shmPTR->RequestID].frameNo[j] = RefPointer; pageFound = true;}}
                             if(pageFound == false){
                                pagetable[shmPTR->RequestID].address[pagetable[shmPTR->RequestID].pageIndex] = shmPTR->Requests[1]/1000;
                                pagetable[shmPTR->RequestID].frameNo[pagetable[shmPTR->RequestID].pageIndex] = RefPointer;
                                pagetable[shmPTR->RequestID].pageIndex++;}

                             framestables[RefPointer].address = shmPTR->Requests[1]/1000;
                            if(shmPTR->Requests[2] == 1){framestables[RefPointer].DirtyBit = 0;
                              fprintf(fp,"Page fault: process %d page %d is swapped in to read data at frame %d\n", shmPTR->RequestID,shmPTR->Requests[1]/1000,RefPointer);}
                            else{
                               fprintf(fp,"Page fault: process %d page %d is swapped in to write data at frame %d extra time added\n", shmPTR->RequestID,shmPTR->Requests[1]/1000,RefPointer);
                               nanoseconds = nanoseconds*2; framestables[RefPointer].DirtyBit = 1;}
                            framestables[RefPointer].useBit = 1;  


                             RefPointer++; break;}

                           else{
                            framestables[RefPointer].useBit = 0;
                            RefPointer++;}
                        }}

            shmPTR->Release = -2; found = 0;  full = true; pageFound = false; sem_post(sem); sem_close(sem);}}

            if((printCount%100 == 0)&&(printCount > 0)){
              for(i = 0; i < 256; i++){
                 if(i%10 == 0) fprintf(fp,"%s","\n");
                 if(framestables[i].DirtyBit == 1)
                   fprintf(fp,"%s","D");
                 else if(framestables[i].address != 0)
                   fprintf(fp,"%s","O");
                 if(framestables[i].address == 0)
                   fprintf(fp,"%s",".");
                 fprintf(fp,"%s"," ");
                 }
               fprintf(fp,"%s","\n");
               for(i=0; i < 256; i++){
                 if(i%10 == 0) fprintf(fp,"%s","\n");
                 fprintf(fp,"%d ",framestables[i].useBit);}
           fprintf(fp,"%s","\n\n");}    
           shmPTR->nanoseconds = shmPTR->nanoseconds + nanoseconds;
           while(shmPTR->nanoseconds >= 1000000000){
              shmPTR->seconds++;
              shmPTR->nanoseconds = shmPTR->nanoseconds - 1000000000;}}
       }
             do{ if(signal_interrupt == true) break;
       printf("Clock ticking..\n");
       sleep(1);
      }while (true);
      fprintf(fp,"%s","\nProcess No \t");
      fprintf(fp,"%s","Accesses per second\t");
      fprintf(fp,"%s","Page faults per access\n");
     

      for(i = 0; i < processNo; i++){
          if(stats[i].endTime == 0)
            stats[i].endTime = shmPTR->seconds;
          if(stats[i].memoryAccesses != 0){
            fprintf(fp, "Memory accesses is %d", stats[i].memoryAccesses);
            fprintf(fp,"     %d\t\t",i);
            speedSum = speedSum + (float)stats[i].memoryAccesses/((float)(stats[i].endTime - stats[i].startTime));
            fprintf(fp, "%f\t\t", (float)stats[i].memoryAccesses/(float)(stats[i].endTime - stats[i].startTime));
            fprintf(fp, "%f\n",(float)stats[i].noFaults/stats[i].memoryAccesses);}}
       fprintf(fp,"Average memory access speed is %f", speedSum/(processNo - 1));
       shmdt((void *) shmPTR);
       sem_close(sem);

       sem_unlink("sem1130");

       fclose(fp);
       shmctl(ShmID, IPC_RMID, NULL);
       killpg(getpgid(getpid()), SIGTERM);                //                         printf("Server has removed its shared memory...\n");
      //                                               printf("Server exits...\n");
      //                                                     //
      //
                                                                                                                               exit(EXIT_SUCCESS);
      
                                                                                                                                                                                                          }
      
      //                                                                                                                                                                                                    
