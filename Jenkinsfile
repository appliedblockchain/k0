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
        sh 'for circuit in commitment transfer addition withdrawal example; do docker run -v /tmp/k0keys:/tmp/k0keys appliedblockchain/zktrading-setup $circuit 4 /tmp/k0keys/${circuit}_pk /tmp/k0keys/${circuit}_vk & done; wait'
        sh 'for circuit in commitment transfer addition withdrawal example; do docker run -v /tmp/k0keys:/tmp/k0keys appliedblockchain/zktrading-convert-vk /tmp/k0keys/${circuit}_vk /tmp/k0keys/${circuit}_vk_alt; done'
    }
    stage('lerna bootstrap') {
        sh 'set +ex && export NVM_DIR="$HOME/.nvm" && . ~/.nvm/nvm.sh && nvm use v8 && set -ex && cd js && lerna bootstrap --no-ci'
    }

    stage('Reset Docker') {
        sh 'docker stop $(docker ps -aq) || true'
        sh 'docker rm $(docker ps -aq) || true'
        sh 'docker rmi $(docker images --filter=reference="*k0chaincode*" -q) || true'
    }

    stage('Start server') {
        sh '''
            docker run -d --name k0server \
            -v /tmp/k0keys/:/tmp/k0keys/:ro \
            -p 4000:80 \
            appliedblockchain/zktrading-server 4 \
            /tmp/k0keys/commitment_pk /tmp/k0keys/commitment_vk \
            /tmp/k0keys/addition_pk /tmp/k0keys/addition_vk \
            /tmp/k0keys/transfer_pk /tmp/k0keys/transfer_vk \
            /tmp/k0keys/withdrawal_pk /tmp/k0keys/withdrawal_vk \
            /tmp/k0keys/example_pk /tmp/k0keys/example_vk \
            80
        '''
        sh '''
            set +ex
            export NVM_DIR="$HOME/.nvm"
            . ~/.nvm/nvm.sh && nvm use v8
            set -ex
            cd js/packages/k0-integration-tests-eth
            node wait-for-server
        '''
    }

    stage('Ethereum tests') {
        sh 'cd js/packages/k0-integration-tests-eth && docker-compose -f docker-compose-ci.yml down'
        sh 'cd js/packages/k0-integration-tests-eth && docker-compose -f docker-compose-ci.yml up -d'
        sh 'set +ex && export NVM_DIR="$HOME/.nvm" && . ~/.nvm/nvm.sh && nvm use v8 && set -ex && cd js/packages/k0-integration-tests-eth && npm test'
        sh 'cd js/packages/k0-integration-tests-eth && docker-compose -f docker-compose-ci.yml down'
    }
    stage('Fabric tests') {
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
        sh '''
            set +ex
            export NVM_DIR="$HOME/.nvm"
            . ~/.nvm/nvm.sh
            nvm use v8
            set -ex
            cd js/packages/k0-integration-tests-fabric/network
            CI=true ./start.sh
        '''
        sh '''
            cd js/packages/k0-integration-tests-fabric/network
            for org in alpha beta gamma bank
            do
                docker-compose run ${org}tools peer chaincode install /artefacts/k0chaincode.1.out
            done
        '''
        sh '''
            cd js/packages/k0-integration-tests-fabric
            set +ex
            export NVM_DIR="$HOME/.nvm"
            . ~/.nvm/nvm.sh
            nvm use v8
            set -ex
            CHAINCODE_ID=k0chaincode node instantiate
        '''
        sh '''
            cd js/packages/k0-integration-tests-fabric
            set +ex
            export NVM_DIR="$HOME/.nvm"
            . ~/.nvm/nvm.sh
            nvm use v8
            set -ex
            CHAINCODE_ID=k0chaincode node_modules/.bin/mocha test
        '''
        sh '''
            cd js/packages/k0-integration-tests-fabric/network
            CI=true ./stop.sh
        '''
    }
    stage('Stop server') {
        sh '''
            docker stop k0server
            docker rm k0server
        '''
    }


}
