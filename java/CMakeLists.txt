
if(ENABLE_JAVA)

    function(fat_jar _target _source_jar_path)
        get_property(_target_jar TARGET ${_target} PROPERTY JAR_FILE)

        cmake_path(GET _source_jar_path STEM _source_jar_stem)

        set(_tmp_dir ${CMAKE_CURRENT_BINARY_DIR}/jar-sources/${_source_jar_stem})
        file(MAKE_DIRECTORY "${_tmp_dir}")

        file(ARCHIVE_EXTRACT
            INPUT ${_source_jar_path}
            DESTINATION ${_tmp_dir}
        )

        file(GLOB_RECURSE
            _extra_manifests
            "${_tmp_dir}/**/MANIFEST.MF"
        )

        if (_extra_manifests)
            file(REMOVE ${_extra_manifests})
        endif()

        # TODO: Fix this causing a rebuild every other build
        # TODO: Fix this always running and throwing a warning
        add_custom_command(
            TARGET ${_target} POST_BUILD
            COMMENT "Adding ${_source_jar_stem}.jar to ${_target_jar}"
            # COMMAND ${CMAKE_COMMAND} -E tar xf ${_source_jar_path}
            COMMAND ${Java_JAR_EXECUTABLE} uf ${_target_jar} .
            WORKING_DIRECTORY ${_tmp_dir}
        )

    endfunction()

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