def OSList = [          
    ['Ubuntu 18.04 (amd64)',            'ubuntu-18-amd64',  'docker && linux-amd64'],
    ['Ubuntu 20.04 (amd64)',            'ubuntu-20-amd64',  'docker && linux-amd64'],
    ['Ubuntu 22.04 (amd64)',            'ubuntu-22-amd64',  'docker && linux-amd64'],
    ['Ubuntu 24.04 (amd64)',            'ubuntu-24-amd64',  'docker && linux-amd64'],
    ['Ubuntu 24.04 (arm64)',            'ubuntu-24-arm64',  'docker && linux-aarch64'],
    ['Ubuntu 26.04 (amd64)',            'ubuntu-26-amd64',  'docker && linux-amd64'],
    ['RHEL 7 (x86_64)',                 'rhel-7-x86_64',    'docker && linux-amd64'],
    ['RHEL 8 (x86_64)',                 'rhel-8-x86_64',    'docker && linux-amd64'],
    ['RHEL 9 (x86_64)',                 'rhel-9-x86_64',    'docker && linux-amd64'],
    ['Debian 10 (amd64)',               'debian-10-amd64',  'docker && linux-amd64'],
    ['Debian 11 (amd64)',               'debian-11-amd64',  'docker && linux-amd64'],
    ['Debian 12 (amd64)',               'debian-12-amd64',  'docker && linux-amd64'],
    ['Debian 12 (arm64)',               'debian-12-arm64',  'docker && linux-aarch64'],
    ['Windows (x86)',                   'windows-x86',      'docker && linux-amd64'],
    ['Windows (x64)',                   'windows-x64',      'docker && linux-amd64'],
    ['MacOSX (homebrew)',               'macosx-homebrew',  'macosx'],
    ['MacOSX (macports)',               'macosx-macports',  'macosx'],
    // Disabled until https://github.com/MDSplus/mdsplus/issues/2605 is fixed
    // ['Address Sanitizer',               'test-asan',        'docker && linux-amd64'],
    ['Thread Sanitizer',                'test-tsan',        'docker && linux-amd64'],
    ['Undefined Behavior Sanitizer',    'test-ubsan',       'docker && linux-amd64'],
    ['Helgrind',                        'test-helgrind',    'docker && linux-amd64'],
    ['Memcheck',                        'test-memcheck',    'docker && linux-amd64'],
]

def getNumThreads() {
    if (env.THREADS) {
        return env.THREADS;
    }
    return "8";
}

def setupStage() {
    return {
        stage("Setup") {
            echo "Building on ${NODE_NAME}"
            
            // Useful for debugging
            sh 'printenv'
            
            // This shouldn't be needed, but just in case
            cleanWs disableDeferredWipeout: true, deleteDirs: true
            
            unstash 'source'

            // HACK: This should be done before stashing the source, but it causes issues with create_github_release
            // so instead each distribution tags separately
            sh "git tag ${env.RELEASE_TAG} || true"
        }
    }
}

def testStage(os) {

    def extraArgs = ""

    if (os.startsWith("macosx-")) { //  || OS.startsWith("windows-")
        // Required to isolate runs on systems that don't have docker networks to do the isolation
        def offset = (EXECUTOR_NUMBER as int) * 1000
        extraArgs += "-DTEST_PORT_OFFSET=${offset}"
    }
    else {
        extraArgs += " --dockerpull --dockernetwork=jenkins-${EXECUTOR_NUMBER}"
    }

    return {
        stage("Build & Test (Debug)") {
            try {
                def threads = getNumThreads()
                sh "deploy/build.py -j${threads} --os=${os} -DCMAKE_BUILD_TYPE=Debug -DRELEASE_TAG=${env.RELEASE_TAG} --build --test --output-junit ${extraArgs}"
            }
            finally {
                junit skipPublishingChecks: true, testResults: "workspace-${os}/mdsplus-junit.xml", keepLongStdio: true
            }
        }

        stage("Build & Test (Release)") {
            try {
                def threads = getNumThreads()
                sh "deploy/build.py -j${threads} --os=${os} -DCMAKE_BUILD_TYPE=RelWithDebInfo -DRELEASE_TAG=${env.RELEASE_TAG} --build --test --output-junit --junit-suite-name '${os}-release' ${extraArgs}"
            }
            finally {
                junit skipPublishingChecks: true, testResults: "workspace-${os}/mdsplus-junit.xml", keepLongStdio: true
            }
        }
    }
}

def packageStage(os) {
    return {
        stage("Build & Package") {
            def threads = getNumThreads()
            sh "deploy/build.py -j${threads} --os=${os} -DCMAKE_BUILD_TYPE=Release -DRELEASE_TAG=${env.RELEASE_TAG} --build --package --verify-packages"
            dir("workspace-${os}") {
                stash name: "packages-${os}", includes: "packages/**/*"
                stash name: "dist-${os}", includes: "mdsplus-publish.json,dist/**/*"
                
                archiveArtifacts artifacts: "packages/*.tgz,packages/*.exe", followSymlinks: false
            }
        }
    }
}

def cleanStage() {
    return {
        stage("Clean") {
            // Collect valgrind core dumps
            archiveArtifacts artifacts: "**/core", followSymlinks: false, allowEmptyArchive: true
            
            cleanWs disableDeferredWipeout: true, deleteDirs: true
        }
    }
}

def distributions = OSList.collectEntries {
    info -> [ "${info[0]}": {
        def (name, os, label) = info
        
        node (label) {
            stage(name) {
                ansiColor('xterm') {
                    try {
                        setupStage().call()
                        testStage(os).call()
                        if (!os.startsWith("test-")) {
                            packageStage(os).call()
                        }
                    }
                    finally {
                        cleanStage().call()
                    }
                }
            }
        }
    }]
}

def localTest(name, label, testStages) {
    return {
        stage(name) {
            node(label) {
                ansiColor('xterm') {
                    try {
                        setupStage().call()
                        
                        stage("Build") {
                            def threads = getNumThreads()
                            sh "deploy/build.py -j${threads} -DCMAKE_BUILD_TYPE=Debug -DRELEASE_TAG=${env.RELEASE_TAG} --build --install "
                        }
                        
                        testStages.call()
                    }
                    finally {
                        cleanStage().call()
                    }
                }
            }
        }
    }
}

distributions['IDL'] = localTest('IDL', 'linux-amd64', {
    stage("Test") {
        try {
            withEnv(["MDSPLUS_DIR=${WORKSPACE}/workspace/install/usr/local/mdsplus"]) {
                sh """
                    set +x
                    . \$MDSPLUS_DIR/setup.sh
                    export PYTHONPATH=\$MDSPLUS_DIR/python/
                    set -x
                    ./idl/testing/run_tests.py
                """
            }
        }
        finally {
            // junit skipPublishingChecks: true, testResults: "mdsplus-junit.xml", keepLongStdio: true
        }
    }
})

distributions['MATLAB'] = localTest('MATLAB', 'linux-amd64', {
    stage("Test") {
        withEnv(["MDSPLUS_DIR=${WORKSPACE}/workspace/install/usr/local/mdsplus"]) {
            sh """
                set +x
                . \$MDSPLUS_DIR/setup.sh
                export PYTHONPATH=\$MDSPLUS_DIR/python/
                set -x
                echo "Testing MATLAB"
            """
        }
    }
})

def AdminList = [
    'dgarnier',
    'GabrieleManduchi',
    'heidthecamp',
    'joshStillerman',
    'mwinkel-dev',
    'santorofer',
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

def new_version = null;
def new_tag = null;

pipeline {
    agent {
        label 'built-in'
    }
    
    options {
        skipDefaultCheckout()
        timeout(time: 1, unit: 'HOURS')
        buildDiscarder(logRotator(daysToKeepStr: '90', artifactNumToKeepStr: '90'))
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
                
                retry(3) {
                    checkout scm;
                }

                script {
                    new_version = sh(
                        script: "/usr/bin/python3 deploy/get_new_version.py",
                        returnStdout: true
                    ).trim()

                    new_tag = "${BRANCH_NAME}_release-" + new_version.replaceAll("\\.", "-")
                    echo "Calculated new version to be ${new_version}"
                }

                // By default it excludes .git/ and things like .DS_Store
                stash name: 'source', includes: '**', useDefaultExcludes: false
            }
        }
        
        stage('Distributions') {
            steps {
                // NOTE: To avoid confusing create_github_release, we cannot create the tag now
                // so instead we pass it along and tag it during the setup stage of each distribution
                withEnv(["RELEASE_TAG=${new_tag}"]) {
                    script {
                        parallel distributions
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
                    if (new_version == '0.0.0') {
                        error 'Unable to compute version for publishing'
                    }

                    ansiColor('xterm') {
                        for (info in OSList) {
                            def (name, os, label) = info

                            if (os.startsWith("test-")) {
                                continue;
                            }

                            unstash "packages-${os}"
                            unstash "dist-${os}"

                            sh "deploy/publish.py --dist-dir=/mnt/mdsplus_staging/dist --cert-dir=/mnt/mdsplus_staging/certs --publish-info=mdsplus-publish.json"
                        }

                        // Create a package containing only the MATLAB code, primarily for use with the mdsthin bridge
                        tar(file: "packages/mdsplus_${BRANCH_NAME}_${new_version}_matlab.tgz", archive: true, compress: true, dir: "matlab/")

                        def release_file_list = [];
                        
                        dir("packages") {
                            sh "ls"
                            
                            def prefix = pwd()
                            findFiles(glob: "*.tgz,*.exe").each {
                                file -> release_file_list.add("${prefix}/${file.path}")
                            }
                        }

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
                        
                        cleanWs disableDeferredWipeout: true, deleteDirs: true
                    }
                }
            }
        }
    }
    
    post {
        failure {
            // if alpha/stable
            mail subject: 'Build is failing',
                body: "Build is failing: ${BUILD_URL}",
                to: 'mdsplus-jenkins-alerts@lists.psfc.mit.edu'
        }
    }

}
