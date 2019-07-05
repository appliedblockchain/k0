node {
    stage('Get sources') {
        checkout scm
    }
    stage('lerna publish') {
        sh 'cd js && lerna publish from-git --yes'
    }
}
