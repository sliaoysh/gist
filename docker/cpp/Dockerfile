FROM fedora:33

RUN useradd -m fed && \
    dnf update -y && \
    dnf install -y net-tools man nano cmake git gdb valgrind cppcheck gcc-c++ clang && \
    mkdir /home/fed/cpp

WORKDIR /home/fed

USER fed

CMD ["/bin/bash"]

# docker image build --tag fed_img .
# docker container run -it --name fed_cont --mount type=bind,source=/home/sam/cpp,target=/home/fed/cpp fed_img
# docker container start -i fed_cont
