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

    parallel {
        stage('ZKP setup') {
            sh 'rm -rf /tmp/k0keys && mkdir /tmp/k0keys'
            sh 'for circuit in commitment transfer addition withdrawal example; do docker run -v /tmp/k0keys:/tmp/k0keys appliedblockchain/zktrading-setup $circuit 4 /tmp/k0keys/${circuit}_pk /tmp/k0keys/${circuit}_vk; done'
            sh 'for circuit in commitment transfer addition withdrawal example; do docker run -v /tmp/k0keys:/tmp/k0keys appliedblockchain/zktrading-convert-vk /tmp/k0keys/${circuit}_vk /tmp/k0keys/${circuit}_vk_alt; done'
        }
        stage('lerna bootstrap') {
            sh 'set +ex && export NVM_DIR="$HOME/.nvm" && . ~/.nvm/nvm.sh && nvm use v8 && set -ex && cd js && lerna bootstrap --no-ci'
        }
   }
   parallel {
   stage('ETH') {
   stage('ETH: Run Docker servers') {
        sh 'cd js/packages/k0-integration-tests-eth/network && docker-compose down'
        sh 'cd js/packages/k0-integration-tests-eth/network && docker-compose up -d'
    }
    stage('ETH: Run integration tests') {
        sh 'set +ex && export NVM_DIR="$HOME/.nvm" && . ~/.nvm/nvm.sh && nvm use v8 && set -ex && cd js/packages/k0-integration-tests-eth && npm test'
    }
    stage('ETH: Shut down Docker containers') {
        sh 'cd js/packages/k0-integration-tests-eth/network && docker-compose down'
    }
    }
    stage('Fabric') {
    stage('Fabric: Package chaincode') {
        sh '''
            sudo rm -rf js/packages/k0-integration-tests-fabric/network/artefacts/*
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
    stage('Fabric: Spin up network') {
        sh '''
            docker rmi $(docker images --filter=reference="*k0chaincode*" -q) || true
            cd js/packages/k0-integration-tests-fabric/network
            CI=true ./stop.sh
            set +ex
            export NVM_DIR="$HOME/.nvm"
            . ~/.nvm/nvm.sh
            nvm use v8
            set -ex
            CI=true ./start.sh
        '''
    }
    stage('Fabric: Install chaincode') {
        sh '''
            cd js/packages/k0-integration-tests-fabric/network
            for org in alpha beta gamma bank
            do
                docker-compose run ${org}tools peer chaincode install /artefacts/k0chaincode.1.out
            done
        '''
    }
    stage('Fabric: Instantiate chaincode') {
        sh '''
            cd js/packages/k0-integration-tests-fabric
            set +ex
            export NVM_DIR="$HOME/.nvm"
            . ~/.nvm/nvm.sh
            nvm use v8
            set -ex
            CHAINCODE_ID=k0chaincode node instantiate
        '''
    }
    stage('Fabric: Run integration tests') {
        sh '''
            cd js/packages/k0-integration-tests-fabric
            set +ex
            export NVM_DIR="$HOME/.nvm"
            . ~/.nvm/nvm.sh
            nvm use v8
            set -ex
            CHAINCODE_ID=k0chaincode node_modules/.bin/mocha test
        '''
    }
    stage('Fabric: Shut down Docker containers') {
        sh '''
            cd js/packages/k0-integration-tests-fabric/network
            CI=true ./stop.sh
        '''
    }
    }
    }
}
