
AC_DEFUN([DK_CHECK_DOCKER_ARG],[
   AS_VAR_SET_IF([HAVE_DOCKER],,
      AC_CHECK_PROG([HAVE_DOCKER],[docker],[yes],[no]))
      
   AC_ARG_WITH(docker-image,
               [AS_HELP_STRING([--with-docker-image],[specify docker images])],  
               [],
               [AS_VAR_SET_IF([DOCKER_IMAGE],
                              [AS_VAR_SET([with_docker_image], ["${DOCKER_IMAGE}"])],
                              [])])
   AS_VAR_SET_IF([with_docker_image],
   AS_IF( [test x"${with_docker_image}" != x"no"],
          [AS_VAR_SET([DOCKER_IMAGE],["${with_docker_image}"])],
          [AS_UNSET([DOCKER_IMAGE])]
          ))
  
   AC_ARG_WITH(docker-container,
               [AS_HELP_STRING([--with-docker-container],[specify docker container])],  
               [],
               [AS_VAR_SET_IF([DOCKER_CONTAINER],
                              [AS_VAR_SET([with_docker_container], ["${DOCKER_CONTAINER}"])],
                              [])])
   AS_VAR_SET_IF([with_docker_container],
   AS_IF( [test x"${with_docker_container}" != x"no"],
          [AS_VAR_SET([DOCKER_CONTAINER],["${with_docker_container}"])],
          [AS_UNSET([DOCKER_CONTAINER])]
          ))
])



AC_DEFUN([AS_VAR_READ],[
read -d '' $1 << _as_read_EOF
$2
_as_read_EOF
])

AC_DEFUN([AS_CONTAINS],[
          AS_VAR_SET([string],"$1")
          AS_VAR_SET([substring],"$2")
          AS_IF([test "${string#*$substring}" != "$string"], [eval $3], [eval $4])])


AC_DEFUN([DK_CMD_CNTRUN], m4_normalize([
          docker run -d -it --entrypoint=/bin/sh
          -e DISPLAY=${DISPLAY} 
          -e http_proxy=${http_proxy}
          -e https_proxy=${https_proxy}
          -v /tmp/.X11-unix:/tmp/.X11-unix 
          -v /etc/passwd:/etc/passwd 
          -v /etc/group:/etc/group 
          -v /etc/shadow:/etc/shadow 
          -v /home:/home 
          -v $(pwd):$(pwd)
          -w $(pwd)
          --name $2
          $1
         ]))

m4_define([DK_CMD_MAKE],m4_normalize([[
 dmake \(\) \{ docker exec -t --user \$\{USER\} mingw /bin/sh -c \"cd `pwd`; \$\{MAKE\} \$\$\{*\}\"\; \}\; dmake
]])) 

AC_DEFUN([DK_CONFIGURE],[

         dnl remove docker related options in configure args
         AS_VAR_SET([dk_configure_args])         
         for arg in ${ac_configure_args}; do
           AS_CONTAINS([${arg}],["--with-docker-"],[:],[AS_VAR_APPEND([dk_configure_args],["${arg}"])]);
           AS_VAR_APPEND([dk_configure_args],[" "]);
         done;
                  
         m4_pushdef([dk_configure_cmd], m4_normalize([
           docker exec -t
           --user ${USER}
           ${DOCKER_CONTAINER} /bin/sh
           -c \"cd $(pwd)\; ${0} ${dk_configure_args} DOCKER_CONTAINER='${DOCKER_CONTAINER}' DOCKER_IMAGE='${DOCKER_IMAGE}' HAVE_DOCKER='no' \";
           exit 0;
         ]))
                  
         
         AS_ECHO(" ------------------------- ") 
         AS_ECHO(" DOCKER CONFIGURE COMMAND: ")
         AS_ECHO(" dk_configure_cmd          ")
         AS_ECHO(" ------------------------- ")

         dnl execute configuration inside docker container         
         eval dk_configure_cmd
         
         m4_popdef([dk_configure_cmd])
])



dnl sets DOCKER_CONTAINER var if not set to a unique name based on pwd dir.
AC_DEFUN([DK_SET_DOCKER_CONTAINER], [
         AS_VAR_SET_IF([DOCKER_CONTAINER],,AS_VAR_SET([DOCKER_CONTAINER],
         [build_$(echo $(pwd) | md5sum | awk '{print $[]1}')]))
])



dnl test_docker_container [cnt_name] [status] [action_if_yes] [action_if_no] 
AC_DEFUN([if_docker_container_string],[         
          AS_VAR_SET([dk_cnt_status], $(docker ps -a -f name=$1 --format "{{.Status}}"))           
          AS_CONTAINS([${dk_cnt_status}],[$2],[$3],[$4])
         ])

dnl test_docker_container_status [cnt_name] [status] [action_if_yes] [action_if_no] 
dnl status =  created, restarting, running, paused, exited
AC_DEFUN([if_docker_container_status],[
         AS_VAR_SET([id_cnt_status], 
         $(docker ps -a -f name=$1 -f status=$2 --format "{{.ID}}"))
         AS_IF([test -n "${id_cnt_status}"],[eval $3], [eval $4])
])

AC_DEFUN([test_docker_container_status],[
         if_docker_container_status([$1],[$2],
            AS_SET_STATUS(0),AS_SET_STATUS(1))
])

AC_DEFUN([get_docker_container_status],[
         if_docker_container_status([$2],[created],AS_VAR_SET($1,[created]))
         if_docker_container_status([$2],[restarting],AS_VAR_SET($1,[restarting]))
         if_docker_container_status([$2],[running],AS_VAR_SET($1,[running]))
         if_docker_container_status([$2],[paused],AS_VAR_SET($1,[paused]))
         if_docker_container_status([$2],[exited],AS_VAR_SET($1,[exited]))
])


AC_DEFUN([get_docker_image_id],[
         AS_VAR_SET([$1], $(docker images -a -q $2))         
])

AC_DEFUN([get_docker_container_id],[
         AS_VAR_SET([$1], $(docker ps -a -f name=$2 --format "{{.ID}}"))
])

AC_DEFUN([get_docker_container_image],[
         AS_VAR_SET([$1], $(docker ps -a -f name=$2 --format "{{.Image}}"))         
])


AC_DEFUN([if_docker_image_exist],[
AS_VAR_SET([id_img_exist], $(docker images -a -q $1 ))
AS_IF([test -n "${id_img_exist}"],[eval $2], [eval $3])
])


dnl start existing [container]
AC_DEFUN([DK_START_CNT],[
  AS_VAR_SET([DOCKER_CONTAINER],$1)  
  get_docker_container_status([dk_status],[${DOCKER_CONTAINER}])
  AS_CASE([${dk_status}],
        [running], [AS_ECHO("RUNNING")],
        [paused],  [AS_ECHO("UNPAUSE")];[eval docker unpause ${DOCKER_CONTAINER}],
        [exited],  [AS_ECHO("RESTART")];[eval docker restart ${DOCKER_CONTAINER}],
        [AC_MSG_ERROR("container not found.")])                   
        
  get_docker_container_status([dk_status],[${DOCKER_CONTAINER}])
  AS_CASE([${dk_status}],
      [running], [AS_ECHO("docker container: ${DOCKER_CONTAINER} is running.")],
      [AS_ECHO("could not start docker container")])
])


dnl start a container form [image]
AC_DEFUN([DK_START_IMGCNT], [
   DK_SET_DOCKER_CONTAINER
   
   AS_ECHO("Starting container from image: $1")         
   get_docker_image_id([dk_image],[$1])
   AS_IF([test -n "${dk_image}"],
         AS_ECHO("using docker image: ${dk_image}"),
         [eval docker pull $1
          get_docker_image_id([dk_image],[$1])
          AS_IF([test -n "${dk_image}"],
                [AC_MSG_NOTICE("pulled docker image: ${dk_image}")],
                [AC_MSG_ERROR("Could not pull the requested image")])
         ])
   
   dnl find if container exists and belongs to selected image   
   get_docker_container_id([dk_id],${DOCKER_CONTAINER})
   get_docker_container_image([dk_img],${DOCKER_CONTAINER})
   AS_ECHO("id: ${dk_id} img: ${dk_img}")
   AS_IF([test -n "${dk_id}" -a -n "${dk_img}"],
         [AS_IF([test x"${dk_img}" == x"$1"],
         [AC_MSG_NOTICE("container found of the correct image")],
         [AC_MSG_ERROR("container does not belong to the correct image")])])

   
   
   dnl find if container exists or start it
   AS_IF([test -n "${dk_id}"],,
         [AS_ECHO("CREATES")];[eval DK_CMD_CNTRUN($1,${DOCKER_CONTAINER})])
   DK_START_CNT(${DOCKER_CONTAINER})
])



  AC_DEFUN([_dk_set_docker_build_debug],[
    AS_ECHO(" --------------------------------- ")
    AS_ECHO(" docker-image      = ${DOCKER_IMAGE}")
    AS_ECHO(" docker-container  = ${DOCKER_CONTAINER}")
    AS_ECHO(" configure args    = ${*}")
    AS_ECHO(" ac_configure_args = ${ac_configure_args}")
    AS_ECHO(" configure command = ${0}")
    AS_ECHO(" configure name    = $(basename ${0})")  
    AS_ECHO(" function args     = $*")
    AS_ECHO(" top_srcdir        = ${abs_top_srcdir}")
    AS_ECHO(" --------------------------------- ")
  ])


AC_DEFUN([DK_SET_DOCKER_BUILD],[

  DK_CHECK_DOCKER_ARG  

  AS_VAR_IF([HAVE_DOCKER],[yes], [
  AS_VAR_SET_IF([DOCKER_IMAGE], 
    [AS_ECHO
     AS_BOX([//// STARTING CONTAINER IN DOCKER IMAGE: ${DOCKER_IMAGE}   //////], [\/])
       DK_START_IMGCNT(${DOCKER_IMAGE})
     AS_ECHO
    ],
    [AS_VAR_SET_IF([DOCKER_CONTAINER],
      [AS_ECHO
       AS_BOX([//// STARTING CONTAINER: ${DOCKER_CONTAINER}   //////], [\/])
         DK_START_CNT(${DOCKER_CONTAINER})
       AS_ECHO
      ])
    ])
  
  AS_VAR_SET_IF([DOCKER_CONTAINER],[
    AS_ECHO
    AS_BOX([//// EXECUTING CONFIGURE IN DOCKER CONTAINER: ${DOCKER_CONTAINER}   //////], [\/])
      _dk_set_docker_build_debug
    AS_ECHO          
    AS_ECHO(" --- ")
    AS_ECHO(" docker configure command: ")
       echo " DK_CMD_CONFIGURE "
    AS_ECHO(" --- ")
    AS_ECHO
    DK_CONFIGURE
   ])
   
  ],
  AS_VAR_SET_IF([DOCKER_CONTAINER],[
    DK_WRITE_DMAKEFILE
    DK_SET_TARGETS
    ]))
  
])  


AC_DEFUN([DK_SET_TARGETS],[
AS_VAR_READ([DK_DOCKER_TARGETS],[

# //////////////////////////////////////////////////////////////////////////// #
# //// DOCKER TARGETS  /////////////////////////////////////////////////////// #
# //////////////////////////////////////////////////////////////////////////// #

ifeq (docker,\$(filter docker,\$(MAKECMDGOALS)))
 
 \$(info ------ )
 \$(info DOCKER MAKECMDGOALS = \$(MAKECMDGOALS) )
 \$(info DOCKER MAKE_COMMAND = \$(MAKE_COMMAND) )
 \$(info DOCKER SHELL = \$(SHELL) )
 \$(info ------ )

.PHONY: docker docker-start
docker:
	@echo "Opening docker session:";
	@echo "selected targets: \$(filter-out \$@,\$(MAKECMDGOALS))";

start:
	@echo "Starting docker container:";	
	@eval docker restart ${DOCKER_CONTAINER}	

stop:
	@echo "Stopping docker container:";
	@eval docker stop ${DOCKER_CONTAINER};

shell:
	@echo "Starting docker shell";
	@eval docker exec -ti --user ${USER} ${DOCKER_CONTAINER} bash


endif
])
AC_SUBST([DK_DOCKER_TARGETS])
AM_SUBST_NOTMAKE([DK_DOCKER_TARGETS])
])






AC_DEFUN([DK_WRITE_DMAKEFILE],[
AS_VAR_READ([DK_DMAKEFILE],m4_escape([
#!/bin/sh
# //////////////////////////////////////////////////////////////////////////// #
# //// DOCKER MAKE  ////////////////////////////////////////////////////////// #
# //////////////////////////////////////////////////////////////////////////// #

# requotes #
for x in "\${@}" ; do
    # try to figure out if quoting was required for the $x
    if [[ "\$x" != "\${x%=*}" ]]; then
        x=\${x%=*}"=\\""\${x#*=}"\\""
    elif [[ "\$x" != "\${x%[[:space:]]*}" ]]; then
        x="\\\""\$x"\\\""
    fi
    echo \$x
    _args=\$_args" "\$x
done


MAKE=make
dmake () { 
echo 
echo " *** BUILDING IN DOCKER CONTAINER *** ";
echo
docker exec -t --user \${USER} ${DOCKER_CONTAINER} sh -c "cd \$(pwd); \${MAKE} -e MAKE_COMMAND='\${MAKE}' \${_args}";
} 

dmake \${@}

]))

AS_ECHO(" Writing dmake file: ")
AS_ECHO("${DK_DMAKEFILE}") > dmake
chmod +x dmake

])
 







 
AC_DEFUN([DK_TEST],[
 
dnl   if_docker_image_exist(["mdsplus/docker:rhel6"],AS_ECHO(YESS),AS_ECHO(NOO))
  
dnl   get_docker_container_status([status],[ciao])
dnl   AS_CASE([${status}],
dnl           [running],AS_ECHO("RUNNING"),
dnl           [paused],AS_ECHO("PAUSED"),
dnl           [exited],AS_ECHO("EXITED")
dnl           )
dnl  AS_VAR_SET(DOCKER_CONTAINER, [rhel6])
dnl  DK_START_IMGCNT([mdsplus/docker:build_rhel6])
dnl  DK_START_CNT([rhel6])

dnl   AS_VAR_SET([MAKE_COMMAND],["'DK_CMD_MAKE'"])
dnl   AS_ECHO("COMMAND: ${MAKE_COMMAND}")
 
dnl  DK_SET_DOCKER_BUILD

dnl DK_WRITE_DMAKEFILE

])
 
