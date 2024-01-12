
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
                script {
                    parallel OSList.collectEntries {
                        OS -> [ "${OS}": {
                            stage("${OS} Build & Test") {
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
                                        try {
                                            def network = "jenkins-${EXECUTOR_NUMBER}-${OS}"
                                            sh "./deploy/build.sh --os=${OS} --test --dockernetwork=${network}"
                                        }
                                        finally {
                                            sh "./deploy/tap-to-junit.py --junit-suite-name=${OS}"
                                            junit skipPublishingChecks: true, testResults: 'mdsplus-junit.xml', keepLongStdio: true

                                            echo "Testing complete"
                                        }
                                    }

                                    if (!env.VERSION && !OS.startsWith("test-")) {
                                        stage("${OS} Test Packaging") {
                                            sh "./deploy/build.sh --os=${OS} --release"
                                        }
                                    }
                                }
                            }
                        }]
                    }
                }
            }
        }

        stage('Additional Testing') {
            parallel {
                stage("Test IDL") {
                    steps {
                        // The IDL tests have to be run with the same OS as the builder
                        ws("${WORKSPACE}/ubuntu22") {
                            withEnv(["MDSPLUS_DIR=${WORKSPACE}/tests/64/buildroot"]) {
                                sh """
                                    set +x
                                    . \$MDSPLUS_DIR/setup.sh
                                    set -x
                                    ./idl/testing/run_tests.py
                                """
                            }
                        }
                    }
                }

                stage("Test MATLAB") {
                    steps {
                        echo "Testing MATLAB"
                        // TODO
                    }
                }
            }
        }

        // TODO: Only publish when the $VERSION is newer than the last release
        stage('Publish') {
            // when {
            //     allOf {
            //         anyOf {
            //             branch 'alpha';
            //             branch 'stable';
            //         }

            //         triggeredBy 'TimerTrigger'
            //     }
            // }
            steps {
                script {
                    stage("Calculate Version") {
                        ws("${WORKSPACE}/publish") {
                            script {
                                checkout scm;

                                def new_version = sh(
                                    script: "./deploy/get_new_version.py",
                                    returnStdout: true
                                ).trim()

                                echo "Calculated new version to be ${new_version}"
                            }
                        }
                    }

                    parallel OSList.findAll{ (!var.startsWith("test-")) }.collectEntries {
                        OS -> [ "${OS}": {
                            stage("${OS} Release & Publish") {
                                ws("${WORKSPACE}/${OS}") {
                                    stage("${OS} Release") {
                                        sh "./deploy/build.sh --os=${OS} --release=${new_version}"
                                    }

                                    stage("${OS} Publish") {
                                        sh "./deploy/build.sh --os=${OS} --publish=${new_version} --publishdir=/tmp/publish"
                                    }
                                }
                            }
                        }]
                    }

                    stage("Publish Version") {
                        ws("${WORKSPACE}/publish") {
                            def tag = "${BRANCH_NAME}_release-" + new_version.replaceAll(".", "-")
                            sh "git tag ${tag}"

                            echo "Publishing tag ${tag}"
                            // git push --tags
                        }
                    }
                }
            }
        }
    }
    post {
        always {
            archiveArtifacts artifacts: "**/test-suite.tap,**/core", followSymlinks: false

            cleanWs disableDeferredWipeout: true, deleteDirs: true
        }
    }
}

