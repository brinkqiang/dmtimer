FROM ubuntu:24.04

RUN apt-get update -yqq
RUN apt-get install -yqq gcc-13 g++-13 cmake git

ENV PROJECT_NAME=dmtimer

WORKDIR /git
RUN git clone https://github.com/brinkqiang/dmtimer.git

WORKDIR /git/dmtimer

RUN git checkout latest

WORKDIR /git/dmtimer
RUN ./build.sh

EXPOSE 8080

CMD ./bin/relwithdebinfo/dmtimertest
