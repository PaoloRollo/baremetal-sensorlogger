#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <errno.h>
#include "serial_linux.h"

#define BUFFER_SIZE 256

typedef struct {
    int fd;
    int running;
} ARG_T;

void* display_routine(void* arg) {
  int ret, fd=((ARG_T*)arg)->fd;
  int* run=&((ARG_T*)arg)->running;
  char* buf=(char*)malloc(BUFFER_SIZE);
  uint16_t bytes_read;
  sleep(2);  //wait 2 sec before starting display functionality..
  while(*run) {
    memset(buf, 0, BUFFER_SIZE);
    bytes_read=0;
    while (*run) {
      ret=read(fd, buf+bytes_read, 1);
      if (ret == -1 && errno == EAGAIN) continue;
      if (ret<0) {
        fprintf(stderr, "Error reading on descriptor: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
      }
      if (*(buf+bytes_read) == '\n' || *(buf+bytes_read) == '\0') break;
      ++bytes_read;
    }
    *(buf+bytes_read)='\0';
    if (*run && *buf!='\0') printf("\n[From Sensor Logger]\n%s\n\n", buf);
    sleep(5);
  }
  printf("\n[Display Routine exiting..]\n\n");
  free(buf);
  pthread_exit(NULL);
}

void mesg_send(void* arg, char* buf) {
  int ret, fd=((ARG_T*)arg)->fd, dim=sizeof(buf);
  uint16_t wrote_bytes=0;
  while (wrote_bytes<dim) {
    ret=write(fd, buf+wrote_bytes, 1);
    if (ret == -1 && errno == EINTR) continue;
    if (ret<0) {
      fprintf(stderr, "Error writing on descriptor: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }
    ++wrote_bytes;
  }
}

int main(int argc, char** argv){
  //serial port initialization
  int fd=serial_open("/dev/ttyACM0");
  if (fd<0) {
    fprintf(stderr, "Failed opening the serial port!\n");
    exit(EXIT_FAILURE);
  }
  if (serial_set_interface_attribs(fd, 115200, 0) <0) {  //set serial port attributes [speed, parity]
    fprintf(stderr, "Failed setting serial port attributes: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  serial_set_blocking(fd, 0);  //set serial port to be non blocking
  if (!fd) {
    fprintf(stderr, "Failed setting the attributes of the serial port!\n");
    exit(EXIT_FAILURE);
  }
  //display routine thread launch..
  ARG_T* arg=(ARG_T*)malloc(sizeof(ARG_T));
  arg->fd=fd;
  arg->running=1;
  pthread_t t;
  if (pthread_create(&t, NULL, display_routine, (void*)arg) < 0) {
    fprintf(stderr, "Error creating the message display thread: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  //
  printf("Wellcome!\n");
  char msg[BUFFER_SIZE];
  while(1) {
    memset(msg, 0, BUFFER_SIZE);
    printf("Choose one of the following operations, by entering the corrispondent character:\n");
    printf("  -[a] --> to Request the logs registrated by the Sensor Logger till now.\n");
    printf("  -[b] --> to Set the period after which the Sensor Logger shall register a log.\n");
    printf("  -[q] --> to Quit.\n");
    //Read a line from stdin
    //memset(&command, 0, sizeof(char));  //clear buffer each time
    scanf("%s", msg);
    mesg_send((void*)arg, msg);
    if (msg[0]=='q') {
      printf("Exiting..\n");
      arg->running=0;
      break;
    }
    else if (msg[0]=='b') {
      while(1) {
        memset(msg, 0, BUFFER_SIZE);
        printf("Choose the new period of the Timer:\n");
        printf("  -[a] --> 30 secons\n");
        printf("  -[b] --> 1 minute\n");
        printf("  -[c] --> 2 minute\n");
        scanf("%s", msg);
        if (msg[0]=='a' || msg[0]=='b' || msg[0]=='c') break;
        else printf("Input does not match! Please try again..\n");
      }
      mesg_send((void*)arg, msg);
    }
    else if (msg[0]!='a') printf("Command not recognised! Please try again..\n");
    sleep(5);  //wait 5 seconds before asking again for command
  }
  //join on the display routine thread
  if (pthread_join(t, NULL) < 0) {
    fprintf(stderr, "Error joining the message display thread: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  //exit operations
  if (close(fd) < 0) {
    fprintf(stderr, "Error closing the descriptor: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  free(arg);
  printf("Bye Bye!\n");
  return 0;
}
