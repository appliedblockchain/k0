FROM appliedblockchain/k0-base

COPY src /project/src
COPY test /project/test
COPY CMakeLists.txt /project/

RUN mkdir /project/build

RUN cd /project/build && cmake -DUSE_INSTALLED_LIBSNARK=ON .. && make -j 4 setup server mtserver convert_vk
