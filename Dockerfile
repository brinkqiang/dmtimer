FROM ubuntu:24.04

RUN apt-get update -yqq
RUN apt-get install -yqq gcc-13 g++-13 cmake git && \
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 100 && \
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-13 100

ENV PROJECT_NAME=dmtimer

WORKDIR /git
RUN git clone --depth 1 --branch latest https://hub.fastgit.org/brinkqiang/dmtimer.git

WORKDIR /git/dmtimer
RUN ./build.sh

EXPOSE 8080

CMD ./bin/relwithdebinfo/dmtimertest
