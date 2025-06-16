def OSList = [          
    ['Ubuntu 18.04 (amd64)',            'ubuntu-18-amd64',  'docker && linux-amd64'],
    ['Ubuntu 20.04 (amd64)',            'ubuntu-20-amd64',  'docker && linux-amd64'],
    ['Ubuntu 22.04 (amd64)',            'ubuntu-22-amd64',  'docker && linux-amd64'],
    ['Ubuntu 24.04 (amd64)',            'ubuntu-24-amd64',  'docker && linux-amd64'],
    ['Ubuntu 24.04 (arm64)',            'ubuntu-24-arm64',  'docker && linux-aarch64'],
    ['RHEL 7 (x86_64)',                 'rhel-7-x86_64',    'docker && linux-amd64'],
    ['RHEL 8 (x86_64)',                 'rhel-8-x86_64',    'docker && linux-amd64'],
    ['RHEL 9 (x86_64)',                 'rhel-9-x86_64',    'docker && linux-amd64'],
    ['Debian 10 (amd64)',               'debian-10-amd64',  'docker && linux-amd64'],
    ['Debian 11 (amd64)',               'debian-11-amd64',  'docker && linux-amd64'],
    ['Debian 12 (amd64)',               'debian-12-amd64',  'docker && linux-amd64'],
    ['Windows (x86)',                   'windows-x86',      'docker && linux-amd64'],
    ['Windows (x64)',                   'windows-x64',      'docker && linux-amd64'],
    // ['MacOSX (brew)',                   'macosx-brew',      'macosx'],
    // ['MacOSX (macports)',               'macosx-macports',  'macosx'],
    // ['Address Sanitizer',               'test-asan',        'docker && linux-amd64'],
    // ['Thread Sanitizer',                'test-tsan',        'docker && linux-amd64'],
    // ['Undefined Behavior Sanitizer',    'test-ubsan',       'docker && linux-amd64'],
    // ['Helgrind',                        'test-helgrind',    'docker && linux-amd64'],
    // ['Memcheck',                        'test-memcheck',    'docker && linux-amd64'],
]


def setupStage() {
    return {
        stage("Setup") {
            echo "Building on ${NODE_NAME}"
            
            // Useful for debugging
            sh 'printenv'
            
            // This shouldn't be needed, but just in case
            cleanWs disableDeferredWipeout: true, deleteDirs: true
            
            unstash 'source'
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
        extraArgs += "--dockernetwork=jenkins-${EXECUTOR_NUMBER}"
    }

    return {
        stage("Build & Test") {
            try {
                sh "deploy/build.py -j --os=${os} --build --test -DCMAKE_BUILD_TYPE=Debug --output-junit ${extraArgs}"
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
            sh "deploy/build.py -j --os=${os} --build --package -DCMAKE_BUILD_TYPE=Release"
            dir("workspace-${os}") {
                stash name: "packages-${os}", includes: "packages/**/*"
                stash name: "dist-${os}", includes: "mdsplus-publish.json,dist/**/*"
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
                        packageStage(os).call()
                    }
                    finally {
                        cleanStage().call()
                    }
                }
            }
        }
    }]
}

def localTest(name, testStages) {
    return {
        stage(name) {
            node('linux-amd64') { // TODO: Improve
                ansiColor('xterm') {
                    try {
                        setupStage().call()
                        
                        stage("Build") {
                            sh "deploy/build.py -j --build --install -DCMAKE_BUILD_TYPE=Debug"
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

distributions['IDL'] = localTest('IDL', {
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

distributions['MATLAB'] = localTest('MATLAB', {
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

pipeline {
    agent {
        label 'built-in'
    }
    
    stages {
        
        stage('Setup') {
            steps {
                sh 'printenv'
                
                // This shouldn't be needed, but just in case
                cleanWs disableDeferredWipeout: true, deleteDirs: true

                retry(3) {
                    checkout scm;
                }
                
                
                script {
                    def new_version = sh(
                        script: "/usr/bin/python3 deploy/get_new_version.py",
                        returnStdout: true
                    ).trim()
                    
                    if (new_version != '0.0.0') {
                        def new_tag = "${BRANCH_NAME}_release-" + new_version.replaceAll("\\.", "-")

                        echo "Calculated new version to be ${new_version}"

                        sh "git tag ${new_tag} || true"
                    }   
                }

                // By default it excludes .git/
                stash name: 'source', includes: '**', useDefaultExcludes: false
            }
        }
        
        stage('Distributions') {
            steps {
                script {
                    parallel distributions
                }
            }
        }
        
        stage('Test Publish') {
            steps {
                script {
                    ansiColor('xterm') {
                        for (info in OSList) {
                            def (name, os, label) = info

                            unstash "packages-${os}"
                            unstash "dist-${os}"

                            sh "deploy/publish.py --distdir=/opt/fakedist --certdir=/mdsplus/certs --publish-info=mdsplus-publish.json"
                        }
                        
                        dir("packages") {
                            sh "ls"
                            archiveArtifacts artifacts: "*.tgz,*.exe", followSymlinks: false
                        }
                        
                        cleanWs disableDeferredWipeout: true, deleteDirs: true
                    }
                }
            }
        }
    }
    
    // TODO: UPDATE ALL DEVELOPERS
    post {
        failure {
            // if alpha/stable
            mail subject: 'Build is failing',
                body: "Build is failing: ${BUILD_URL}",
                to: 'slwalsh@psfc.mit.edu,heidcamp@mit.edu'
        }
    }

}