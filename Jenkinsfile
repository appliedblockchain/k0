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
    stage('Run docker servers') {
        sh 'cd js/packages/k0-integration-tests-eth/network && docker-compose up -d'
    }
    stage('Run integration tests') {
        sh 'set +ex && export NVM_DIR="$HOME/.nvm" && . ~/.nvm/nvm.sh && nvm use v8 && set -ex && cd js/packages/k0-integration-tests-eth && npm test'
    }
    stage('Shut down docker containers') {
        sh 'cd js/packages/k0-integration-tests-eth/network && docker-compose down'
    }
}
