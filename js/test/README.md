Faster zk setup for tests:

```

rm -rf /tmp/k0keys && \
mkdir /tmp/k0keys && \
for circuit in commitment transfer addition withdrawal example; do \
cpp/build/src/setup $circuit 5 /tmp/k0keys/${circuit}_pk /tmp/k0keys/${circuit}_vk && \
cpp/build/src/convert_vk /tmp/k0keys/${circuit}_vk /tmp/k0keys/${circuit}_vk_alt; \
done
```

Running the demo(note: you will need 7 terminals, install iterm2 on mac for ease of use):

Run the proving servers(one terminal each):
```
cpp/build/src/server 5 /tmp/k0keys/commitment_pk /tmp/k0keys/commitment_vk /tmp/k0keys/addition_pk /tmp/k0keys/addition_vk /tmp/k0keys/transfer_pk /tmp/k0keys/transfer_vk /tmp/k0keys/withdrawal_pk /tmp/k0keys/withdrawal_vk /tmp/k0keys/example_pk /tmp/k0keys/example_vk 4000

cpp/build/src/server 5 /tmp/k0keys/commitment_pk /tmp/k0keys/commitment_vk /tmp/k0keys/addition_pk /tmp/k0keys/addition_vk /tmp/k0keys/transfer_pk /tmp/k0keys/transfer_vk /tmp/k0keys/withdrawal_pk /tmp/k0keys/withdrawal_vk /tmp/k0keys/example_pk /tmp/k0keys/example_vk 5000

cpp/build/src/server 5 /tmp/k0keys/commitment_pk /tmp/k0keys/commitment_vk /tmp/k0keys/addition_pk /tmp/k0keys/addition_vk /tmp/k0keys/transfer_pk /tmp/k0keys/transfer_vk /tmp/k0keys/withdrawal_pk /tmp/k0keys/withdrawal_vk /tmp/k0keys/example_pk /tmp/k0keys/example_vk 6000
```

Run the Merkle tree servers(1 terminal each):

```
cpp/build/src/mtserver 5 4100
cpp/build/src/mtserver 5 5100
cpp/build/src/mtserver 5 6100
```

tree height, paths to commitment pk, commitment vk, addition pk, addition vk, transfer pk, transfer vk, withdrawal pk, withdrawal vk, example pk, example vk, port


for circuit in example; do \
cpp/build/src/setup $circuit 7 /tmp/k0keys/${circuit}_pk /tmp/k0keys/${circuit}_vk && \
cpp/build/src/convert_vk /tmp/k0keys/${circuit}_vk /tmp/k0keys/${circuit}_vk_alt; \
done


# Build docker images locally, create the keys with docker

```
cd cpp
docker build -f docker/builder.Dockerfile -t zktrading-builder .
docker build -f docker/zktrading.Dockerfile -t zktrading .
for IMAGE in setup server mtserver convert-vk pack unpack
do
    docker build -f docker/$IMAGE.Dockerfile -t zktrading-$IMAGE .
done
```

mkdir /tmp/k0keys
for circuit in commitment transfer addition withdrawal example
do
    docker run -v /tmp/k0keys:/tmp/k0keys zktrading-setup $circuit 4 /tmp/k0keys/${circuit}_pk /tmp/k0keys/${circuit}_vk
    docker run -v /tmp/k0keys:/tmp/k0keys zktrading-convert-vk /tmp/k0keys/${circuit}_vk /tmp/k0keys/${circuit}_vk_alt
done
