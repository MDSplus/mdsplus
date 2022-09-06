
IF(ENABLE_JAVA)

    MACRO(FAT_JAR _target _source_jar_path)
        GET_PROPERTY(_target_jar TARGET ${_target} PROPERTY JAR_FILE)

        # Macro parameters aren't real variables, CMAKE_PATH needs real variables
        SET(_source_jar_stem "${_source_jar_path}")
        CMAKE_PATH(GET _source_jar_stem STEM _source_jar_stem)

        SET(_tmp_dir ${CMAKE_CURRENT_BINARY_DIR}/jar-sources/${_source_jar_stem})
        FILE(MAKE_DIRECTORY "${_tmp_dir}")

        ADD_CUSTOM_COMMAND(
            TARGET ${_target} POST_BUILD
            COMMENT "Adding ${_source_jar_stem}.jar to ${_target}.jar"
            COMMAND ${CMAKE_COMMAND} -E tar xf ${_source_jar_path}
            COMMAND ${Java_JAR_EXECUTABLE} umf ${_manifest} ${_target_jar} .
            WORKING_DIRECTORY ${_tmp_dir}
        )

    ENDMACRO()

    ###
    ### Jars
    ###

    SET(JSCH ${CMAKE_CURRENT_BINARY_DIR}/jsch-0.1.54.jar)
    IF(NOT EXISTS ${JSCH})
        FILE(DOWNLOAD
            "http://sourceforge.net/projects/jsch/files/jsch.jar/0.1.54/jsch-0.1.54.jar/download"
            ${JSCH}
        )
    ENDIF()

    ADD_SUBDIRECTORY(mdsplus)
    ADD_SUBDIRECTORY(mdsplus-api)
    ADD_SUBDIRECTORY(mdsobjects)
    ADD_SUBDIRECTORY(jscope)
    ADD_SUBDIRECTORY(jtraverser)
    ADD_SUBDIRECTORY(jdispatcher)
    ADD_SUBDIRECTORY(devicebeans)
    ADD_SUBDIRECTORY(jdevices)
    ADD_SUBDIRECTORY(jtraverser2)

    IF(mvn_EXECUTABLE)

        INCLUDE(ProcessorCount)
        ProcessorCount(NPROC)

        IF(NOT NPROC EQUAL 0)
            SET(_mvn_threads -T 1)
        ENDIF()

        ###
        ### Maven
        ###

        SET(MVN ${mvn_EXECUTABLE}
            # -Dmaven.repo.local=/workspace/.m2/repository
            ${_mvn_threads}
            -DsourceDirectory=${CMAKE_SOURCE_DIR}/java
        )

        ADD_CUSTOM_TARGET(
            maven-build
            COMMAND ${MVN} package -DskipTests -f ${CMAKE_CURRENT_SOURCE_DIR}/pom.xml
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )

        # FILE(GLOB_RECURSE
        #     _pom_xml_list
        #     "pom.xml"
        # )

        ADD_CUSTOM_TARGET(
            maven-deploy
            COMMAND ${MVN} versions:set -DgenerateBackupPoms=false -DnewVersion=${RELEASE_VERSION} -DartifactId=*
            COMMAND ${MVN} -s /sign_keys/.m2/settings.xml -Dsettings.security=/sign_keys/.m2/settings-security.xml -DskipTests deploy
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )

        ADD_CUSTOM_TARGET(
            maven-tests
            COMMAND ${MVN} test
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )

        ADD_CUSTOM_TARGET(
            maven-clean
            COMMAND ${MVN} clean
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )

    ENDIF()

ENDIF()