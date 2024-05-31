
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
    // 'windows-x86',
    // 'windows-x86_64',
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
    'dgarnier',
]

def schedule = "";
if (BRANCH_NAME == "alpha") {
    schedule = "0 18 * * *";
}
if (BRANCH_NAME == "stable") {
    schedule = "0 19 * * *";
}

def new_version = '0.0.0';
def new_tag = null;

def release_file_list = [];

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

        stage("Calculate Version") {
            when {
                anyOf {
                    branch 'alpha';
                    branch 'stable';
                }
            }
            steps {
                script {
                    ws("${WORKSPACE}/publish") {
                        checkout scm;

                        new_version = sh(
                            script: "./deploy/get_new_version.py",
                            returnStdout: true
                        ).trim()

                        if (new_version == '0.0.0') {
                            error "Failed to calculate new version"
                        }
                        
                        new_tag = "${BRANCH_NAME}_release-" + new_version.replaceAll("\\.", "-")

                        echo "Calculated new version to be ${new_version}"
                    }
                }
            }
        }

        stage('Distributions') {
            steps {
                script {
                    parallel OSList.collectEntries {
                        OS -> [ "${OS} Build & Test": {
                            stage("${OS} Build & Test") {
                                ws("${WORKSPACE}/${OS}") {
                                    stage("${OS} Clone") {
                                        checkout scm;
                                    }

                                    stage("${OS} Test") {
                                        def network = "jenkins-${EXECUTOR_NUMBER}-${OS}"
                                        sh "./deploy/build.py -j --os=${OS} --test --output-junit --dockernetwork=${network} -DCMAKE_BUILD_TYPE=Debug"
                                    }

                                    if (!OS.startsWith("test-")) {
                                        stage("${OS} Release") {
                                            sh "./deploy/build.py -j --os=${OS} -DCMAKE_BUILD_TYPE=Release"
                                            
                                            findFiles(glob: "packages/*.tgz").each {
                                                file -> release_file_list.add(WORKSPACE + "/" + file.path)
                                            }

                                            findFiles(glob: "packages/*.exe").each {
                                                file -> release_file_list.add(WORKSPACE + "/" + file.path)
                                            }
                                        }
                                    }
                                }
                            }
                        }]
                    }
                }
            }
        }

        stage('Publish') {
            when {
                allOf {
                    anyOf {
                        branch 'alpha';
                        branch 'stable';
                    }

                    triggeredBy 'TimerTrigger'
                }
            }
            steps {
                script {


                    parallel OSList.findAll{ OS -> (!OS.startsWith("test-")) }.collectEntries {
                        OS -> [ "${OS} Publish": {
                            stage("${OS} Publish") {
                                ws("${WORKSPACE}/${OS}") {
                                    sh "./deploy/build.sh --os=${OS} --publish --branch=${BRANCH_NAME} --version=${new_version} --keys=/mdsplus/certs --publishdir=/mdsplus/dist"
                                }
                            }
                        }]
                    }

                    stage("Publish to GitHub") {
                        ws("${WORKSPACE}/publish") {
                            echo "Creating GitHub Release and Tag for ${new_tag}"
                            withCredentials([
                                usernamePassword(
                                    credentialsId: 'MDSplusJenkins',
                                    usernameVariable: 'GITHUB_APP',
                                    passwordVariable: 'GITHUB_ACCESS_TOKEN'
                                )]) {

                                // TODO: Protect against spaces in filenames
                                def release_file_list_arg = release_file_list.join(" ")
                                sh "./deploy/create_github_release.py --tag ${new_tag} --api-token \$GITHUB_ACCESS_TOKEN ${release_file_list_arg}"
                            }

                        }
                    }
                }
            }
        }
    }
    post {
        always {
            
            junit skipPublishingChecks: true, testResults: '**/mdsplus-junit.xml', keepLongStdio: true

            // Collect valgrind core dumps
            archiveArtifacts artifacts: "**/core", allowEmptyArchive: true

            cleanWs disableDeferredWipeout: true, deleteDirs: true
        }
    }
}

