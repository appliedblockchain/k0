# K0
K0 is an implementation of [Zerocash](http://zerocash-project.org/) for smart contract platforms. Currently Ethereum and Hyperledger Fabric are supported. The protocol has been modified slightly to enable an additional use case, namely the use of a predefined private payment in a secondary smart contract. For more information please read our [technical description](https://appliedblockchain.com/k0.pdf).

__WARNING! This is alpha-stage software. It has not been security-audited. DO NOT USE IN PRODUCTION!__

A quickstart guide to run some integration tests is provided [below](#quickstart). For instructions on how to build and run the C++ components please read the README files in the following subdirectories:
- [cpp](cpp)
- [js/packages/k0-integration-tests-eth](js/packages/k0-integration-tests-eth)
- [js/packages/k0-integration-tests-fabric](js/packages/k0-integration-tests-fabric)

## Quickstart

The easiest way to see the code in action is to run the Ethereum integration tests using the provided Docker Compose file for the server components. However, first we need to run the "trusted setup", also using Docker (this might take a couple of minutes):

```
rm -rf /tmp/k0keys
mkdir /tmp/k0keys
for circuit in commitment transfer addition withdrawal example
do
  docker run -it -v /tmp/k0keys/:/tmp/k0keys/ appliedblockchain/k0-setup:v0.0.30 $circuit 4 /tmp/k0keys/${circuit}\_pk /tmp/k0keys/${circuit}_vk
  docker run -it -v /tmp/k0keys/:/tmp/k0keys/ appliedblockchain/k0-convert-vk:v0.0.30 /tmp/k0keys/${circuit}\_vk /tmp/k0keys/${circuit}\_vk_alt
done
```

Then, spin up the Docker containers. In the directory `js/packages/k0-integration-tests-eth`:

```
docker-compose up
```

Among other things, this starts a proving server, which reads in the proving and verification keys generated in the setup. This takes a while. You should see messages like "Loading commitment proving key...". Once you see the message "All keys loaded." you can run the integration tests.

In the directory `js/packages/k0-integration-tests-eth` (in another terminal window):

```
npm i
npm test
```
