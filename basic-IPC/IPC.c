#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

char alphabet[] ="ZYXWVUTSRQPONMLKJIHGFEDCBA"; // char array of the alphabet

void runReceiver(int pipe[]) {

  char outputBuffer[sizeof(alphabet)]; // output buffer of equal size to input buffer (alphabet)

 // close write 
   close(pipe[1]);

   // while we can read, write the output char to stdout
   while((read(pipe[0], outputBuffer, 1)) == 1) {
     write(1, outputBuffer, 1);
       }
   write(1,"\n", 1); // new line after characters

   // close read 
    close(pipe[0]);
    printf("Receiver process %d has terminated.\n", getpid());
    exit(0);
}

void runSender(int pipe[]) {
  int i = 0; // iteration int
  char * output; // char to push to write pipe

   close(pipe[0]);
   for(i; i < sizeof(alphabet); i++){
     *output = alphabet[i];
      write(pipe[1], output, 1);
      sleep(1);
   }
    close(pipe[1]);
    printf("Sender process %d has terminted.\n", getpid());
    exit(0);
}

int main() {

  int pid_sender; // id of sender process
  int pid_receiver; // id of receiver process

  int RWpipe[2]; // initialize pipe int array
  pipe(RWpipe); // create pipe with array 

  // fire sender with pipe
  if(pid_sender = fork() == 0) {
    /* child process */
    runSender(RWpipe);
  } else if(pid_sender < 0) {
    printf("Error: could not fork sender");
    exit(1); // exit with code 1
  } 

  // fire receiver with pipe
  if(pid_receiver = fork() == 0) {
    /* child process */
    runReceiver(RWpipe);
  } else if(pid_receiver < 0) { 
    printf("Error: could not fork receiver.");
    exit(1); // exit with code 1
  }
   
  // close pipe ends
  close(RWpipe[0]);
  close(RWpipe[1]);

  // sleep for 30 sec
  sleep(30);
  
  // print
  printf("Main process %d has terminated.\n", getpid());

  return 0;
}
