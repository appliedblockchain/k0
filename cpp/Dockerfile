FROM ubuntu:18.04 as builder

RUN apt-get update && apt-get install -y \
    build-essential cmake git libprocps-dev python-markdown \
    libboost-all-dev libssl-dev libjsonrpccpp-tools libjsonrpccpp-dev \
    libgmp3-dev && \
    ln -s /usr/include/jsoncpp/json/ /usr/include/json

RUN apt-get install -y curl

RUN curl -LO https://github.com/Kitware/CMake/releases/download/v3.13.4/cmake-3.13.4-Linux-x86_64.tar.gz && \
    tar -C /usr/local --strip-components 1 -xf cmake-3.13.4-Linux-x86_64.tar.gz && \
    rm cmake-3.13.4-Linux-x86_64.tar.gz 

COPY src /workbench/src
COPY depends /workbench/depends
COPY CMakeLists.txt /workbench/

RUN apt-get install -y libgtest-dev

RUN mkdir /workbench/build

RUN cd /workbench/build && cmake .. && make server

ENTRYPOINT ["/workbench/build/src/server"]