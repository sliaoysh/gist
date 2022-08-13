#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define QUEUE_DEPTH 64

int main(int argc, char **argv) {
  int fd = open(argv[1], O_WRONLY | O_CREAT, 0644);

  if (fd < 0) {
      fprintf(stderr, "File open error: %d", fd);
      exit(EXIT_FAILURE);
  }

  const u_int64_t total = 1e6;
  u_int64_t len = 0, req = 0;
  int ret;

  char *buff = nullptr;

  do {
    len = snprintf(nullptr, 0, "%lu: %s", req, "Hello, world! This is variable string length.\n");
    buff = static_cast<char*>(malloc(len+1));
    sprintf(buff, "%lu: %s", req, "Hello, world! This is variable string length.\n");
    ret = write(fd, buff, len);
    free(buff);
    if (ret < 0) {
        fprintf(stderr, "Error: %d", errno);
        exit(EXIT_FAILURE);
    }
  } while (++req < total);

  close(fd);

  return EXIT_SUCCESS;
}