FROM appliedblockchain/k0-base

COPY src /project/src
COPY test /project/test
COPY CMakeLists.txt /project/

RUN mkdir /project/build

RUN cd /project/build && cmake -DUSE_INSTALLED_LIBSNARK=ON -DUSE_DUMMYHASH=ON .. && make setup server mtserver convert_vk
