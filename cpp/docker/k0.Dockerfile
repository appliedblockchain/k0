FROM appliedblockchain/k0-base:v0.0.8

COPY src /project/src
COPY test /project/test
COPY depends /project/depends
COPY CMakeLists.txt /project/

RUN mkdir /project/build && cd /project/build && \
    cmake -DMULTICORE=ON .. && \
    cat src/CMakeFiles/server.dir/flags.make && \
    make -j$(nproc) k0 && make -j$(nproc) setup server mtserver convert_vk
