#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

constexpr int BUF_SIZE = 512;
constexpr const char* SV_SOCK_PATH = "/tmp/sam_unix";

int main()
{
    struct sockaddr_un addr;
    ssize_t numRead;
    char buf[BUF_SIZE]{};

    // Create a new client socket with domain: AF_UNIX, type: SOCK_STREAM, protocol: 0
    int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    printf("Client socket fd = %d\n", sfd);

    if (sfd == -1) {
      printf("socket");
      return 1;
    }

    // Construct server address, and make the connection.
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SV_SOCK_PATH, sizeof(addr.sun_path) - 1);

    // Connects the active socket referred to be sfd to the listening socket
    // whose address is specified by addr.
    if (connect(sfd, (struct sockaddr *) &addr,
                sizeof(struct sockaddr_un)) == -1) {
      printf("connect");
      return 1;
    }

    // Copy stdin to socket.
    // Read at most BUF_SIZE bytes from STDIN into buf.
    while ((numRead = read(STDIN_FILENO, buf, BUF_SIZE)) > 0) {
      // Then, write those bytes from buf into the socket.
      if (write(sfd, buf, numRead) != numRead) {
        printf("partial/failed write");
      }
    }

    if (numRead == -1) {
      printf("read");
    }
    exit(EXIT_SUCCESS);
}
