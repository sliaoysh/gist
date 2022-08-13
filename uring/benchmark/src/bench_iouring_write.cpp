#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <liburing.h>
#include <chrono>

#define QUEUE_DEPTH 512

using Time = std::chrono::high_resolution_clock;
using ms = std::chrono::milliseconds;

int main(int argc, char **argv) {

  struct io_uring ring;
  struct io_uring_params params;
  memset(&params, 0, sizeof(params));
  params.flags |= IORING_SETUP_SQPOLL;
  params.features |= IORING_FEAT_SUBMIT_STABLE | IORING_FEAT_SINGLE_MMAP | IORING_FEAT_SQPOLL_NONFIXED | IORING_FEAT_NODROP;
  params.sq_thread_cpu = 0;

  struct io_uring_sqe *sqe;
  struct io_uring_cqe *cqe;

  int ret = io_uring_queue_init_params(QUEUE_DEPTH, &ring, &params);

  if (ret < 0) {
    fprintf(stderr, "queue_init: %d\n", ret);
    return ret;
  }
  int fd = open(argv[1], O_WRONLY | O_CREAT, 0644);

  if (fd < 0) {
      perror("open");
      return EXIT_FAILURE;
  }

  const u_int64_t total = 1e6;

  u_int64_t offset = 0, len = 0;

  char *buff = nullptr;

  uint64_t req = 0, submitted = 0, count = 0;

  auto process_cq = [&](){
        ret = io_uring_wait_cqe_nr(&ring, &cqe, submitted); 

        do {
          if (ret < 0) {
            fprintf(stderr, "Failed to wait on cqe\n");
            exit(EXIT_FAILURE);
          }
          void* data = io_uring_cqe_get_data(cqe);
          free(data);

          io_uring_cqe_seen(&ring, cqe);

          ret = io_uring_peek_cqe(&ring, &cqe);

        } while (--submitted);
  };

  char msg[] = "Hello, world! This is a variable string length.\n";

  const auto begin = Time::now();

  do {
    sqe = io_uring_get_sqe(&ring);

    len = snprintf(nullptr, 0, "%lu: %s", req, msg);

    buff = static_cast<char*>(malloc(len+1));

    sprintf(buff, "%lu: %s", req, msg);

    io_uring_prep_write(sqe, fd, buff, len, offset);
    io_uring_sqe_set_data(sqe, buff);

    offset += len;

    ret = io_uring_submit(&ring);
    ++submitted;

    if (ret < 0) {
      fprintf(stderr, "Failed to submit\n");
      return EXIT_FAILURE;
    }
    if (submitted >= QUEUE_DEPTH) {
      process_cq();
    }

  } while (++req < total);

  process_cq();

  auto elapsed = Time::now() - begin;

  printf("Time elapsed: %ld\n", std::chrono::duration_cast<ms>(elapsed).count());

  close(fd);
  io_uring_queue_exit(&ring);

  return EXIT_SUCCESS;
}