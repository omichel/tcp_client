#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "tcp_client.h"

int main(int argc, char *argv[]) {
  const char *host = "localhost";
  const char *robot_name = "MyBot";
  int port = 1234;

  int fd = tcp_client_open();
  int trial = 0;
  while (true) {
    int connect = tcp_client_connect(fd, host, port);
    if (connect == 1) // success
      break;
    if (connect == 0) { // failed to connect
      if (trial == 10) {
        fprintf(stderr, "Giving up.\n");
        tcp_client_close(fd);
        exit(EXIT_FAILURE);
      }
      trial++;
      fprintf(stderr,
              "Could not connect to Webots instance at %s:%d, retrying in %d "
              "second%s...\n",
              host, port, trial, trial > 1 ? "s" : "");
      sleep(trial);
    } else { // failed to lookup host
      tcp_client_close(fd);
      exit(EXIT_FAILURE);
    }
  }
  int length = strlen("EXT TCP ") + strlen(robot_name);
  char *buffer = malloc(length);
  sprintf(buffer, "EXT TCP %s", robot_name);
  int n = tcp_client_send(fd, buffer, length);
  printf("sent %d bytes: %s\n", n, buffer);
  free(buffer);

  tcp_client_close(fd);
}
