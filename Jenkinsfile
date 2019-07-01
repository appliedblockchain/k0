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
        sh 'for circuit in commitment transfer addition withdrawal example; do docker run -v /tmp/k0keys:/tmp/k0keys appliedblockchain/zktrading-setup $circuit 7 /tmp/k0keys/${circuit}_pk /tmp/k0keys/${circuit}_vk; done'
        sh 'for circuit in commitment transfer addition withdrawal example; do docker run -v /tmp/k0keys:/tmp/k0keys appliedblockchain/zktrading-convert-vk /tmp/k0keys/${circuit}_vk /tmp/k0keys/${circuit}_vk_alt; done'
    }
    stage('lerna bootstrap') {
        sh 'cd js && lerna bootstrap'
    }
}
