FROM fedora:33

RUN useradd -m jup && \
    dnf update -y && \
    dnf install -y wget net-tools man nano

WORKDIR /home/jup

USER jup

RUN wget https://repo.anaconda.com/archive/Anaconda3-2020.11-Linux-x86_64.sh && \
    bash ./Anaconda3-2020.11-Linux-x86_64.sh -b && \
    source ./anaconda3/bin/activate && \
    conda init && \
    conda install -y -c conda-forge jupyterlab

CMD /home/jup/anaconda3/bin/jupyter lab --no-browser --ip=0.0.0.0 --port=8888

# docker image build --tag jupyter_img .
# docker container run --name jupyter_cont -p 8888:8888 --mount type=bind,source=/home/sam/python,target=/home/jup/src jupyter_img
# docker container start -i jupyter_cont
