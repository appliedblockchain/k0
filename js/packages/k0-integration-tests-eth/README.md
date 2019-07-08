## Ethereum integration tests

## ZKP setup
```
rm -rf /tmp/k0keys
mkdir /tmp/k0keys
for circuit in commitment transfer addition withdrawal example
do
  ../../../cpp/build/src/server $circuit 4 /tmp/k0keys/${circuit}\_pk /tmp/k0keys/${circuit}_vk
  ../../../cpp/build/src/convert_vk /tmp/k0keys/${circuit}\_vk /tmp/k0keys/${circuit}\_vk_alt
done
```

## Start proving and Merkle tree servers

In separate terminal windows, start the following servers:

Proving server:

```
../../../cpp/build/src/server 7 /tmp/k0keys/commitment_pk /tmp/k0keys/commitment_vk /tmp/k0keys/addition_pk /tmp/k0keys/addition_vk /tmp/k0keys/transfer_pk /tmp/k0keys/transfer_vk /tmp/k0keys/withdrawal_pk /tmp/k0keys/withdrawal_vk /tmp/k0keys/example_pk /tmp/k0keys/example_vk 4000
```

Merkle tree server for Alice:
```
../../../cpp/build/src/mtserver 4 4100
```

Merkle tree server for Bob:
```
../../../cpp/build/src/mtserver 4 5100
```

Merkle tree server for Carol:
```
cpp/build/src/mtserver 4 6100
```

## Start an Ethereum testnet

For example:
```
docker run -p 8546:8546 appliedblockchain/parity-solo-instant
```

## Run tests
```
npm test
```