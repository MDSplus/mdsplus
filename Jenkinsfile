@Library('camunda-community') _

def OSList = [
    'windows',
    'ubuntu18',
    'ubuntu20',
    'ubuntu22',
    'rhel7',
    'rhel8',
    'rhel9',
    // 'alpine3.9-armhf',
    // 'alpine3.9-x86_64',
    // 'alpine3.9-x86',
    'debian9-64',
    'debian10-64',
    'debian11-64',
    'test-asan',
    'test-tsan',
    'test-ubsan',
]

def AdminList = [
    'AndreaRigoni',
    'GabrieleManduchi',
    'joshStillerman',
    'mwinkel-dev',
    'santorofer',
    'tfredian',
    'WhoBrokeTheBuild',
    'zack-vii',
]

def schedule = "";
if (BRANCH_NAME == "alpha") {
    schedule = "0 18 * * *";
}
if (BRANCH_NAME == "stable") {
    schedule = "0 19 * * *";
}

pipeline {
    agent any
    options {
        skipDefaultCheckout()
        timeout(time: 1, unit: 'HOURS')
    }
    triggers {
        cron(schedule)
        issueCommentTrigger('(?i).*retest\\s+this\\s+please.*')
    }

    stages {
        stage('Setup') {
            steps {
                sh 'printenv'

                script {
                    
                    // is PR
                    if (env.CHANGE_ID) {
                        // This is safe because untrusted PRs will use Jenkinsfile from the target branch
                        if (env.GITHUB_COMMENT_AUTHOR) {
                            if (!AdminList.contains(env.GITHUB_COMMENT_AUTHOR)) {
                                currentBuild.result = 'ABORTED'
                                error 'This user does not have permission to trigger builds.'
                            }
                            else {
                                echo("Build was started by ${GITHUB_COMMENT_AUTHOR}, who wrote: \"${GITHUB_COMMENT}\", which matches the trigger pattern.")
                            }
                        }
                        else if (!AdminList.contains(env.CHANGE_AUTHOR)) {
                            currentBuild.result = 'ABORTED'
                            error 'This user does not have permission to trigger builds.'
                        }
                    }
                }

                // This shouldn't be needed, but just in case
                cleanWs disableDeferredWipeout: true, deleteDirs: true
            }
        }
        stage('Distributions') {
            steps {
                dynamicMatrix([
                    failFast: false,
                    axes: [
                        OS: OSList
                    ],
                    actions: {
                        ws("${WORKSPACE}/${OS}") {

                            stage("${OS} Clone") {
                                checkout scm;
                            }

                            stage("${OS} Bootstrap") {
                                sh "GIT_BRANCH=${BRANCH_NAME} ./deploy/build.sh --os=bootstrap"

                                if (env.OS.endsWith("armhf")) {
                                    sh "docker run --rm --privileged multiarch/qemu-user-static:register --reset"
                                }
                            }

                            stage("${OS} Test") {
                                EVENT_PORT = OSList.indexOf(OS) + 4100
                                sh "./deploy/build.sh --os=${OS} --test --eventport=${EVENT_PORT}"
                            }
                        }
                    }
                ])
            }
        }
    }
    post {
        always {

            for (OS in OSList) {
                ws("${WORKSPACE}/${OS}") {
                    sh "./deploy/tap-to-junit.py --junit-suite-name=${OS}"
                }
            }
            
            junit skipPublishingChecks: true, testResults: '**/mdsplus-junit.xml'

            // TODO: Why does this hang on windows?
            // archiveArtifacts artifacts: '**/tests/**/*.log,**/tests/**/test-suite.tap,**/tests/**/core' excludes: 'windows/**/*'

            cleanWs disableDeferredWipeout: true, deleteDirs: true
        }
    }
}

