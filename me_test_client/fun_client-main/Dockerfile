FROM ubuntu:20.04

RUN DEBIAN_FRONTEND="noninteractive" apt-get update && apt-get -y install tzdata

RUN apt-get update \
  && apt-get install -y ssh \
    build-essential \
    gcc \
    g++ \
    gdb \
    clang \
    cmake \
    rsync \
    tar \
    python \
    ninja-build
RUN apt-get install -y iproute2 tcpdump vim  
RUN apt-get install -y linux-tools-generic
RUN useradd -m user && yes password | passwd user
