FROM fedora:36

RUN useradd -m fed && \
    dnf update -y && \
    dnf install -y net-tools man nano cmake git gdb valgrind cppcheck gcc-c++ clang libubsan clang-tools-extra && \
    mkdir /home/fed/gist

WORKDIR /home/fed

USER fed

CMD ["/bin/bash"]

# docker image build --tag cpp_img .
# docker container run -it --name cpp_cont --mount type=bind,source=/home/sam/gist,target=/home/fed/gist cpp_img
# docker container start -i cpp_cont
