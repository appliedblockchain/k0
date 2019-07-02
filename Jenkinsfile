node {
    stage('Get sources') {
        checkout scm
    }
    stage('Build main image') {
        sh 'cd cpp && docker build -f docker/zktrading.Dockerfile -t zktrading .'
    }
    stage('Build app images') {
        sh 'cd cpp && for IMAGE in setup server mtserver convert-vk; do docker build -f docker/$IMAGE.Dockerfile -t appliedblockchain/zktrading-$IMAGE .; done'
    }
    stage('ZKP setup') {
        sh 'rm -rf /tmp/k0keys && mkdir /tmp/k0keys'
        sh 'for circuit in commitment transfer addition withdrawal example; do docker run -v /tmp/k0keys:/tmp/k0keys appliedblockchain/zktrading-setup $circuit 4 /tmp/k0keys/${circuit}_pk /tmp/k0keys/${circuit}_vk; done'
        sh 'for circuit in commitment transfer addition withdrawal example; do docker run -v /tmp/k0keys:/tmp/k0keys appliedblockchain/zktrading-convert-vk /tmp/k0keys/${circuit}_vk /tmp/k0keys/${circuit}_vk_alt; done'
    }
    stage('lerna bootstrap') {
        sh 'set +ex && export NVM_DIR="$HOME/.nvm" && . ~/.nvm/nvm.sh && nvm use v8 && set -ex && cd js && lerna bootstrap --no-ci'
    }
    stage('ETH: Run docker servers') {
        sh 'cd js/packages/k0-integration-tests-eth/network && docker-compose down'
        sh 'cd js/packages/k0-integration-tests-eth/network && docker-compose up -d'
    }
    stage('ETH: Run integration tests') {
        sh 'set +ex && export NVM_DIR="$HOME/.nvm" && . ~/.nvm/nvm.sh && nvm use v8 && set -ex && cd js/packages/k0-integration-tests-eth && npm test'
    }
    stage('ETH: Shut down docker containers') {
        sh 'cd js/packages/k0-integration-tests-eth/network && docker-compose down'
    }
    stage('Fabric: Package chaincode') {
        sh '''
            rm -f js/packages/k0-integration-tests-fabric/network/artefacts/*
            docker run -v $PWD/js/packages/k0-integration-tests-fabric/network/artefacts:/artefacts \
                -v ~/go/src/github.com/hyperledger/fabric:/opt/gopath/src/github.com/hyperledger/fabric:ro \
                -v $PWD/go:/opt/gopath/src/github.com/appliedblockchain/zktrading/go:ro \
                hyperledger/fabric-tools:1.2.0 \
                peer chaincode package \
                    -n k0chaincode -v 1 \
                    -p github.com/appliedblockchain/zktrading/go/chaincode/cash \
                    /artefacts/k0chaincode.1.out
        '''
    }
}
