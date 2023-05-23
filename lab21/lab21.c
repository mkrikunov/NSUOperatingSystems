#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int cnt = 0;


int main() {

   void sigcatch(int);

   sigset(SIGINT, sigcatch);
   sigset(SIGQUIT, sigcatch);

   while(1) {
      pause();
   }

}


void sigcatch(int sig) {

   if (sig == SIGINT) {
      write(1, "\a", sizeof(char));
      cnt++;
   }
   else {
      char buff[35];
      sprintf(buff, "\nNumber of signals: %d\n", cnt);
      write(1, buff, strlen(buff));
      exit(0);
   }

}

