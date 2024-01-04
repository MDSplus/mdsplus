@Library('camunda-community') _

def OSList = [
    'test-asan',
    'test-tsan',
    'test-ubsan',
    'test-helgrind',
    'test-memcheck',
    'ubuntu-18-x86_64',
    'ubuntu-20-x86_64',
    'ubuntu-22-x86_64',
    // 'rhel-8-x86_64',
    'rhel-9-x86_64',
    'alpine-3.14-x86_64',
    // 'alpine-3.14-arm64',
    'debian-11-x86_64',
    'debian-12-x86_64',
    'amazonlinux-2-x86_64',
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

                                // if (env.OS.endsWith("armhf")) {
                                //     sh "docker run --rm --privileged multiarch/qemu-user-static:register --reset"
                                // }
                            }

                            stage("${OS} Test") {
                                sh "./deploy/build.py -j --os=${OS} --test"

                                archiveArtifacts artifacts: 'tests/**/*.log,tests/**/test-suite.tap,tests/**/core'
                            }
                        }
                    }
                ])
            }
        }
    }
    post {
        always {
            cleanWs disableDeferredWipeout: true, deleteDirs: true
        }
    }
}

