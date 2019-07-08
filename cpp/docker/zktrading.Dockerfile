FROM appliedblockchain/k0-base:v0.0.7

COPY src /project/src
COPY test /project/test
COPY CMakeLists.txt /project/

RUN mkdir /project/build

RUN cd /project/build && \
    cmake -DCURVE=ALT_BN128 -DMULTICORE=ON -DUSE_PT_COMPRESSION=ON -DPERFORMANCE=ON -DUSE_ASM=ON -DWITH_PROCPS=OFF -DUSE_INSTALLED_LIBSNARK=ON .. && \
    cat src/CMakeFiles/server.dir/flags.make && \
    make -j 2 zktrade && make -j 2 setup server mtserver convert_vk
