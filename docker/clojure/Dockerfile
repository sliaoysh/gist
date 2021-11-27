FROM fedora:35

RUN useradd -m clj && \
    dnf update -y && \
    dnf install -y wget net-tools nano man java && \
    wget https://raw.githubusercontent.com/technomancy/leiningen/stable/bin/lein && \
    mv ./lein /usr/local/bin/ && \
    chmod 755 /usr/local/bin/lein && \
    mkdir /home/clj/ws

WORKDIR /home/clj

USER clj

RUN lein

CMD /bin/bash

# docker image build --tag clojure_img .
# docker container run -it --name clojure_cont --mount type=bind,source=/home/sam/clj,target=/home/clj/ws clojure_img
# docker container start -i clojure_cont
# lein repl
