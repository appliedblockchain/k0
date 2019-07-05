FROM appliedblockchain/k0-base:v0.0.3

COPY src /project/src
COPY test /project/test
COPY CMakeLists.txt /project/

RUN mkdir /project/build

RUN cd /project/build && cmake -DUSE_INSTALLED_LIBSNARK=ON .. && make -j 2 zktrade && make -j 2 setup server mtserver convert_vk
