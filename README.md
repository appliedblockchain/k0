## Set up dependencies etc
```
sudo apt-get install -y \
build-essential cmake git libgmp3-dev libprocps-dev python-markdown \
libboost-all-dev libssl-dev libjsonrpccpp-dev libjsonrpccpp-tools
sudo ln -s /usr/include/jsoncpp/json/ /usr/include/json
git submodule update --init --recursive
cd js
npm i
cd ..
```

## Build
```
cd cpp
mkdir build
cd build
```
Then, on Linux:
```
cmake ..
```
Or, on Mac OS X:
```
CPPFLAGS="-I/usr/local/opt/openssl/include -I/usr/local/include/sodium" \
LDFLAGS="-L/usr/local/opt/openssl/lib -L/usr/local/lib" \
PKG_CONFIG_PATH=/usr/local/opt/openssl/lib/pkgconfig \
cmake -DWITH_PROCPS=OFF -DWITH_SUPERCOP=OFF -DMULTICORE=OFF ..
```
Then:
```
make
cd ../..
```

## ZKP setup
```
cd cpp/build
src/examples/merkle_tree/merkle_tree_addition_setup 2 /tmp/mt_addition_pk  /tmp/mt_addition_vk
src/examples/merkle_tree/merkle_tree_inclusion_setup 2 /tmp/mt_inclusion_pk /tmp/mt_inclusion_vk
src/convert_vk /tmp/mt_addition_vk /tmp/mt_addition_vk_alt
src/convert_vk /tmp/mt_inclusion_vk /tmp/mt_inclusion_vk_alt
```

## Run server
```
cd cpp/build
src/examples/merkle_tree/merkle_tree_server 2 /tmp/mt_addition_pk /tmp/mt_addition_vk \
                                              /tmp/mt_inclusion_pk /tmp/mt_inclusion_vk  
```

## Run tests
```
cd js
MOCHA_MERKLE_TREE_HEIGHT=2 node_modules/.bin/mocha test/mixer.js
```
