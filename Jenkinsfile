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
                                sh "GIT_BRANCH=\$BRANCH_NAME ./deploy/build.sh --os=bootstrap"

                                if (OS.endsWith("armhf")) {
                                    sh "docker run --rm --privileged multiarch/qemu-user-static:register --reset"
                                }
                            }

                            stage("${OS} Test") {
                                network="jenkins-${EXECUTOR_NUMBER}-${OS}"
                                sh "./deploy/build.sh --os=${OS} --test --dockernetwork=${network}"
                            }

                            // The IDL/MATLAB tests need to be run the same OS as the build server
                            if (OS == "ubuntu22") {
                                stage("IDL") {
                                    env.MDSPLUS_DIR = "${WORKSPACE}/tests/64/buildroot"
                                    sh ". \$MDSPLUS_DIR/setup.sh; ./idl/testing/run_tests.py"
                                }

                                stage("MATLAB") {
                                    // TODO
                                }
                            }
                        }
                    }
                ])
            }
        }
    }
    post {
        always {

            script {
                for (OS in OSList) {
                    ws("${WORKSPACE}/${OS}") {
                        sh "./deploy/tap-to-junit.py --junit-suite-name=${OS}"
                        junit skipPublishingChecks: true, testResults: 'mdsplus-junit.xml', keepLongStdio: true
                    }
                }
            }

            // Collect TAP results, valgrind core dumps
            archiveArtifacts artifacts: "**/test-suite.tap,**/core", followSymlinks: false

            cleanWs disableDeferredWipeout: true, deleteDirs: true
        }
    }
}

