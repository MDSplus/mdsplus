
if(ENABLE_JAVA)

    ###
    ### Jars
    ###

    set(JSCH ${CMAKE_CURRENT_BINARY_DIR}/jsch-0.1.54.jar)
    if(NOT EXISTS ${JSCH})
        file(DOWNLOAD
            "http://sourceforge.net/projects/jsch/files/jsch.jar/0.1.54/jsch-0.1.54.jar/download"
            ${JSCH}
        )
    endif()

    set(JAVAX_JSON "${CMAKE_CURRENT_SOURCE_DIR}/mdsobjects/src/main/resources/javax.json-1.0.4.jar")

    add_subdirectory(mdsplus)
    add_subdirectory(mdsplus-api)
    add_subdirectory(mdsobjects)
    add_subdirectory(jscope)
    add_subdirectory(jtraverser)
    add_subdirectory(jdispatcher)
    add_subdirectory(devicebeans)
    add_subdirectory(jdevices)
    add_subdirectory(jtraverser2)

    if(mvn_EXECUTABLE)

        include(ProcessorCount)
        ProcessorCount(NPROC)

        if(NOT NPROC EQUAL 0)
            set(_mvn_threads -T 1)
        endif()

        ###
        ### Maven
        ###

        set(MVN ${mvn_EXECUTABLE}
            # -Dmaven.repo.local=/workspace/.m2/repository
            ${_mvn_threads}
            -DsourceDirectory=${CMAKE_SOURCE_DIR}/java
        )

        add_custom_target(
            maven-build
            COMMAND ${MVN} package -DskipTests -f ${CMAKE_CURRENT_SOURCE_DIR}/pom.xml
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )

        # FILE(GLOB_RECURSE
        #     _pom_xml_list
        #     "pom.xml"
        # )

        add_custom_target(
            maven-deploy
            COMMAND ${MVN} versions:set -DgenerateBackupPoms=false -DnewVersion=${RELEASE_VERSION} -DartifactId=*
            COMMAND ${MVN} -s /sign_keys/.m2/settings.xml -Dsettings.security=/sign_keys/.m2/settings-security.xml -DskipTests deploy
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )

        add_custom_target(
            maven-tests
            COMMAND ${MVN} test
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )

        add_custom_target(
            maven-clean
            COMMAND ${MVN} clean
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )

    endif()

endif()