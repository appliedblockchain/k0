node {
    stage('Get sources') {
        checkout scm
    }
    stage('Build main image') {
        sh 'cd cpp && sudo docker build -f docker/zktrading.Dockerfile -t zktrading .'
    }
    stage('Build app images') {
        sh 'cd cpp && for IMAGE in setup server mtserver convert-vk; do docker build -f docker/$IMAGE.Dockerfile -t appliedblockchain/zktrading-$IMAGE .; done'
    }
}