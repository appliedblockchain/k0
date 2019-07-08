# K0

## Quickstart

The easiest way to see the code in action is to run the Ethereum integration using the provided Docker Compose file for the server components. However, first we need to run the "trusted setup", also using Docker (this might take a couple of minutes):

```
rm -rf /tmp/k0keys
mkdir /tmp/k0keys
for circuit in commitment transfer addition withdrawal example
do
  docker run -it -v /tmp/k0keys/:/tmp/k0keys/ appliedblockchain/zktrading-setup:v0.0.27 $circuit 4 /tmp/k0keys/${circuit}\_pk /tmp/k0keys/${circuit}_vk
  docker run -it -v /tmp/k0keys/:/tmp/k0keys/ appliedblockchain/zktrading-convert-vk:v0.0.27 /tmp/k0keys/${circuit}\_vk /tmp/k0keys/${circuit}\_vk_alt
done
```

Then, spin up the Docker containers:

```
docker-compose up
```

Among other things, this starts a proving server, which reads in the proving and verification keys generated in the setup. This takes a while. You should see messages like "Loading commitment proving key...". Once you see the message "All keys loaded." you can run the integration tests (in another terminal window):

```
npm i
npm test
```

For instructions on how to build and run the C++ components please read the README files in the subdirectories [cpp](cpp), [js/packages/k0-integration-tests-eth](js/packages/k0-integration-tests-eth),  [js/packages/k0-integration-tests-fabric](js/packages/k0-integration-tests-fabric).