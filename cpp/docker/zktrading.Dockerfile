FROM appliedblockchain/snarkapps-base

COPY / /project/

WORKDIR /project

RUN mkdir build && cd build && cmake .. && make
