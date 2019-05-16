FROM zktrading-builder

COPY src /project/src
COPY test /project/test
COPY depends /project/depends
COPY CMakeLists.txt /project/

RUN mkdir /project/build

RUN cd /project/build && cmake .. && make