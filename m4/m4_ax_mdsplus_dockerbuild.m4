
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

   AC_ARG_WITH(docker-url,
               [AS_HELP_STRING([--with-docker-url],[specify docker url])],  
               [],
               [AS_VAR_SET_IF([DOCKER_URL],
                              [AS_VAR_SET([with_docker_url], ["${DOCKER_URL}"])],
                              [])])
   AS_VAR_SET_IF([with_docker_url],
   AS_IF( [test x"${with_docker_url}" != x"no"],
          [AS_VAR_SET([DOCKER_URL],["${with_docker_url}"])],
          [AS_UNSET([DOCKER_URL])]
          ))
])



AC_DEFUN([DK_GET_CONFIGURE_ARGS],[
AX_CONFIGURE_ARGS
push_IFS(["'"])
 for x in ${ac_configure_args}; do
    # try to figure out if quoting was required for the $x
    AS_IF([test "$x" == " "],,[
    AS_IF([test "$x" != "${x%=*}"],
          [AS_VAR_SET([x],[${x%=*}"=\""${x#*=}"\""])],
          [AS_IF([test "$x" != "${x%\[\[:space:\]\]*}"],
                 [AS_VAR_SET([x],["\""$x"\""])],
                 [AS_VAR_SET([x],["$x"])])
          ])    
    AS_VAR_SET([$1],["$$1 '$x'"])
    ])
 done
pop_IFS
])


AC_DEFUN([DK_GET_CONFIGURE_ARGS_WITHOUT_DOCKER],[
DK_GET_CONFIGURE_ARGS([_args])
push_IFS(["'"])
 for x in ${_args}; do    
    AS_IF([test "$x" == " "],[:],
          AS_IF([test "$x" != "${x%--with-docker-*}"],[:],
          AS_VAR_SET([$1],["$$1 $x"])))
 done 
pop_IFS
])



AC_DEFUN([dk_set_user_env], [
AS_VAR_SET([user_entry], [$(awk -F: "{if (\$[]1 == \"${USER}\") {print \$[]0} }" /etc/passwd)])
AS_VAR_SET([user_id],    [$(echo ${user_entry} | awk -F: '{print $[]3}')])
AS_VAR_SET([user_group], [$(echo ${user_entry} | awk -F: '{print $[]4}')])
AS_VAR_SET([user_home],  [$(echo ${user_entry} | awk -F: '{print $[]6}')])
AS_VAR_SET([group_entry],[$(awk -F: "{if (\$[]3 == \"${user_group}\") {print \$[]0} }" /etc/group)])
AS_VAR_SET([user_groups],[$(id -G ${USER} | sed 's/ /,/g')])
AS_VAR_SET([abs_builddir],[$(pwd)])
AS_VAR_SET([abs_srcdir],[$(cd ${srcdir}; pwd)])

])


AC_DEFUN([DK_CMD_CNTRUN], [
  AS_VAR_SET([user_entry], [$(awk -F: "{if (\$[]1 == \"${USER}\") {print \$[]0} }" /etc/passwd)])
  AS_VAR_SET([user_id],    [$(echo ${user_entry} | awk -F: '{print $[]3}')])
  AS_VAR_SET([user_group], [$(echo ${user_entry} | awk -F: '{print $[]4}')])
  AS_VAR_SET([user_home],  [$(echo ${user_entry} | awk -F: '{print $[]6}')])
  AS_VAR_SET([group_entry],[$(awk -F: "{if (\$[]3 == \"${user_group}\") {print \$[]0} }" /etc/group)])
  AS_VAR_SET([user_groups],[$(id -G ${USER} | sed 's/ /,/g')])
  AS_VAR_SET([abs_srcdir],[$(cd ${srcdir}; pwd)])


  m4_normalize([ docker run -d -it --entrypoint=/bin/sh \
                     -e USER=${USER} \
                     -e DISPLAY=${DISPLAY} \
                     -e http_proxy=${http_proxy} \
                     -e https_proxy=${https_proxy} \
                     -v /tmp/.X11-unix:/tmp/.X11-unix \
                     -v /etc/resolv.conf:/etc/resolv.conf \
                     -v ${abs_srcdir}:${abs_srcdir} \
                     -v ${user_home}:${user_home} \
                     -v $(pwd):$(pwd) \
                     -w $(pwd) \
                     --name $2 \
                     $1;
               ])
                       
  m4_normalize([ docker exec --user root $2 sh -c "
                          echo ${user_entry}  >> /etc/passwd; 
                          echo ${group_entry} >> /etc/group;
                         ";
                       ])
])



AC_DEFUN([DK_CONFIGURE],[

         dnl remove docker related options in configure args
         AS_VAR_SET([dk_configure_args])
         DK_GET_CONFIGURE_ARGS_WITHOUT_DOCKER([dk_configure_args])
         AS_VAR_APPEND([dk_configure_args],[" "])
         AS_VAR_SET_IF([DOCKER_IMAGE],AS_VAR_APPEND([dk_configure_args],["DOCKER_IMAGE=\"${DOCKER_IMAGE}\" "]));
         AS_VAR_SET_IF([DOCKER_CONTAINER],AS_VAR_APPEND([dk_configure_args],["DOCKER_CONTAINER=\"${DOCKER_CONTAINER}\" "]));
         AS_VAR_SET_IF([DOCKER_FILE],AS_VAR_APPEND([dk_configure_args],["DOCKER_FILE=\"${DOCKER_FILE}\" "]));
    
         m4_pushdef([dk_configure_cmd], m4_normalize([
           docker exec -t
           --user ${USER}
           ${DOCKER_CONTAINER} /bin/sh
           -c \"cd $(pwd)\; ${0} DK_ADD_ESCAPE(${dk_configure_args}) DK_ADD_ESCAPE([HAVE_DOCKER=\"no\"]) \";
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

AC_DEFUN([DK_SET_DOCKER_IMAGE], [
         AS_VAR_SET_IF([DOCKER_IMAGE],,AS_VAR_SET([DOCKER_IMAGE],
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
         AS_VAR_SET([$1], $(docker images -a | ${AWK} -v _img=$2 {if ($1 ":" $2 == _img) {print $3}} ))
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
   DK_GET_CONFIGURE_ARGS([dk_configure_args])


   AS_ECHO("Pulling image: $1")
   docker pull $1;
      
   AS_ECHO("Starting container from image: $1")         
   
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
         [AS_ECHO("CREATE ")];[DK_CMD_CNTRUN($1,${DOCKER_CONTAINER})])
   DK_START_CNT(${DOCKER_CONTAINER})
])


AC_DEFUN([DK_START_URL], [   
   AS_VAR_SET([dk_build_args])
   AS_VAR_SET_IF([http_proxy], [AS_VAR_APPEND([dk_build_args],["--build-arg http_proxy=\"${http_proxy}\" "])])
   AS_VAR_SET_IF([https_proxy],[AS_VAR_APPEND([dk_build_args],["--build-arg https_proxy=\"${https_proxy}\" "])])
   
   dnl TODO: check status
   AS_VAR_SET_IF([DOCKER_URL],
                 [echo docker build ${dk_build_args} -t $2 $1];
                 [eval docker build ${dk_build_args} -t $2 $1])
                 
])







AC_DEFUN([_dk_set_docker_build_debug],[
         AS_ECHO(" --------------------------------- ")
         AS_ECHO(" docker-image      = ${DOCKER_IMAGE}")
         AS_ECHO(" docker-container  = ${DOCKER_CONTAINER}")
         AS_ECHO(" ac_configure_args = ${ac_configure_args}")
         AS_ECHO(" --------------------------------- ")
         ])


AC_DEFUN([DK_SET_DOCKER_BUILD],[
  DK_CHECK_DOCKER_ARG  

  AS_VAR_IF([HAVE_DOCKER],[yes], [



  AS_VAR_SET_IF([DOCKER_URL],    
    [AS_BANNER(["BUILDING IMAGE FOR URL: ${DOCKER_URL}"])
     DK_SET_DOCKER_IMAGE
     DK_START_URL(${DOCKER_URL},${DOCKER_IMAGE})])
     
  AS_VAR_SET_IF([DOCKER_IMAGE],
    [AS_BANNER(["STARTING CONTAINER IN DOCKER IMAGE: ${DOCKER_IMAGE}"])
     DK_START_IMGCNT(${DOCKER_IMAGE})],
    [AS_VAR_SET_IF([DOCKER_CONTAINER],
     [AS_BANNER(["STARTING CONTAINER: ${DOCKER_CONTAINER}"])
      DK_START_CNT(${DOCKER_CONTAINER})])
    ])

  AS_VAR_SET_IF([DOCKER_CONTAINER],[
                  dk_set_user_env
                  DK_WRITE_DSHELLFILE
                  DK_SET_TARGETS
                 ])
  
  AS_VAR_SET_IF([DOCKER_CONTAINER],[
    AS_BANNER(["EXECUTING CONFIGURE IN DOCKER CONTAINER: ${DOCKER_CONTAINER}"])
    _dk_set_docker_build_debug
    
    dk_set_user_env
    DK_WRITE_DSHELLFILE
    DK_CONFIGURE
    dnl and exit
   ])
   ],[
   AS_VAR_SET_IF([DOCKER_CONTAINER],[
                   dk_set_user_env
                   DK_SET_TARGETS
                  ])   
  ])
  
])  





AC_DEFUN([DK_SET_TARGETS],[
AS_VAR_READ([DK_DOCKER_TARGETS],[

# //////////////////////////////////////////////////////////////////////////// #
# //// DOCKER TARGETS  /////////////////////////////////////////////////////// #
# //////////////////////////////////////////////////////////////////////////// #

DOCKER_CONTAINER = ${DOCKER_CONTAINER}
DOCKER_IMAGE     = ${DOCKER_IMAGE}

user_id     = ${user_id}
user_group  = ${user_group}
user_groups = ${user_groups}
user_home   = ${user_home}

# if MAKESHELL is not defined use local dshell to enter docker container
docker_SHELL := \$(if \${MAKESHELL},\${MAKESHELL},\${abs_top_builddir}/dshell)
local_SHELL  := \$(if \${SHELL},${SHELL},/bin/sh)
export SHELL = \${docker_SHELL}

.PHONY: docker
ifeq (docker,\$(MAKECMDGOALS))
docker:
	@ \
	echo ; \
	echo " This build was set to work with Docker: "; \
	echo " ---------------------------------------------------------------- "; \
	echo " This means that you should see a running docker container named: "; \
	echo " ${DOCKER_CONTAINER}. 	"                                        ; \
	echo " Using a map of the current srcdir and builddir and some envs the "; \
	echo " build system should be able to launch make inside the container. "; \
	echo " Use the usual make command and targets to build inside docker.   "; \
	echo ; \
	echo " Additional targets: "                                           ; \
	echo " make docker start   <- run the docker container "               ; \
	echo " make docker stop    <- stop and remove the docker container "   ; \
	echo " make docker shell   <- launch a shell inside the container "    ; \
	echo " make docker shell USER=root <- launch the shell as root "       ; \
	echo " make docker inspect <- get container info (such as ipaddr)"     ; \
	echo ;
else
docker:
	@ \
	echo " --- docker build setup --- ";
endif

ifeq (docker,\$(filter docker,\$(MAKECMDGOALS)))
export SHELL = \${local_SHELL}

.PHONY: info start stop shell inspect run
inspect:
	@echo "info:";
	docker inspect \${DOCKER_CONTAINER}

start:
	@echo "Starting docker container:";	
	m4_normalize( docker run -d -it --entrypoint=/bin/sh 
	                     -e USER=\${USER}
			     -e DISPLAY=\${DISPLAY}
			     -e http_proxy=\${http_proxy} 
			     -e https_proxy=\${https_proxy}			     
			     -v /tmp/.X11-unix:/tmp/.X11-unix     
			     -v /etc/resolv.conf:/etc/resolv.conf 
			     -v \${abs_top_srcdir}:\${abs_top_srcdir} 
			     -v \${abs_top_builddir}:\${abs_top_builddir} 
			     -v \${user_home}:\${user_home}   
			     -w \${abs_top_builddir}         
			     --name \${DOCKER_CONTAINER} 
			     \${DOCKER_IMAGE}; )
	m4_normalize( docker exec --user root \${DOCKER_CONTAINER} 
	                          sh -c "
	                            echo ${user_entry}  >> /etc/passwd; 
				    echo ${group_entry} >> /etc/group;
				  ";)

stop:
	@echo "Stopping docker container:";
	docker stop \${DOCKER_CONTAINER};
	docker rm \${DOCKER_CONTAINER};

shell:
	@echo "Starting docker shell";
	docker exec -ti --user \${USER} \${DOCKER_CONTAINER} \
	 sh -c "cd \$(shell pwd); export MAKESHELL=/bin/sh; bash"
	
endif
])
AC_SUBST([DK_DOCKER_TARGETS])
m4_ifdef([AM_SUBST_NOTMAKE], [AM_SUBST_NOTMAKE([DK_DOCKER_TARGETS])])
])




AC_DEFUN([DK_WRITE_DSHELLFILE],[
AS_VAR_READ([DK_DSHELLFILE],m4_escape([
#!/bin/sh
# //////////////////////////////////////////////////////////////////////////// #
# //// DOCKER SHELL  ///////////////////////////////////////////////////////// #
# //////////////////////////////////////////////////////////////////////////// #

DOCKER_CONTAINER=${DOCKER_CONTAINER}
DOCKER_IMAGE=${DOCKER_IMAGE}

abs_srcdir=${abs_srcdir}
abs_builddir=${abs_builddir}
user_id=${user_id}
user_group=${user_group}
user_groups=${user_groups}
user_home=${user_home}

>&2 echo "Docker: Entering container \${DOCKER_CONTAINER} ";
quoted_args="\$(printf " %q" "\$\@")"
if [ -n "\${MAKESHELL}" ]; then
 \${MAKESHELL} \${quoted_args};
else
 docker exec -t --user \${USER} \${DOCKER_CONTAINER} bash -c "cd \$(pwd); export MAKESHELL=/bin/sh; export MAKEFLAGS=\${MAKEFLAGS}; export MFLAGS=\${MFLAGS}; sh \${quoted_args}";
fi
]))

AS_ECHO(" Writing dshell file: ${abs_builddir}/dshell ")
AS_ECHO("${DK_DSHELLFILE}") > ${abs_builddir}/dshell
chmod +x dshell

])
 



dnl ////////////////////////////////////////////////////////////////////////////
dnl ////////////////////////////////////////////////////////////////////////////
dnl ////////////////////////////////////////////////////////////////////////////
dnl // Utility functions

 
AC_DEFUN([AS_BANNER],[
          AS_ECHO 
          AS_BOX([// $1 //////], [\/])
          AS_ECHO 
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


AC_DEFUN([push_IFS],[
AS_VAR_SET([_save_IFS],[$IFS])
AS_VAR_SET([IFS],[$1])
])

AC_DEFUN([pop_IFS],[
AS_VAR_SET([IFS],[${_save_IFS}])
])


AC_DEFUN([DK_ADD_ESCAPE],$([
dnl TODO: make this for all quote char in sh using awk
echo $1 | sed 's/\\/\\\\/g' | sed 's/\"/\\\"/g';
]))
