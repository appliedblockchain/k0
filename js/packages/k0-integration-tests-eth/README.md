# K0 Ethereum integration tests

## Quickstart

The easiest way to see the code in action is to use the provided Docker Compose file to spin up some necessary server components and a single-node Ethereum test "network". However, first we need to run the "trusted setup", also using Docker (note that this might take a couple of minutes):

```
rm -rf /tmp/k0keys
mkdir /tmp/k0keys
for circuit in commitment transfer addition withdrawal example
do
  docker run -it -v /tmp/k0keys/:/tmp/k0keys/ appliedblockchain/zktrading-setup $circuit 4 /tmp/k0keys/${circuit}\_pk /tmp/k0keys/${circuit}_vk
  docker run -it -v /tmp/k0keys/:/tmp/k0keys/ appliedblockchain/zktrading-convert-vk /tmp/k0keys/${circuit}\_vk /tmp/k0keys/${circuit}\_vk_alt
done
```

Then, spin up the Docker containers:

```
docker-compose up
```

Among other things, this starts a proving server, which reads in the proving and verification keys generated in the setup. This takes a while. You should see messages like "Loading commitment proving key...". Once you see the message "All keys loaded" you can run the integration tests:

```
npm i
npm test
```

