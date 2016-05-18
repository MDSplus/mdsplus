

AC_DEFUN([AX_TEST_LOCAL_NAMESPACE],[
 AC_PUSH_LOCAL([my_unique_namespace_name]) 
 PRINT_HELLO_WORLD
 AC_POP_LOCAL([my_unique_namespace_name])
])

AC_DEFUN_LOCAL([my_unique_namespace_name],[AS_BANNER],[
                AS_ECHO 
                AS_BOX([// LOCAL!! $1 //////], [\/])
                AS_ECHO 
               ])

AC_DEFUN_LOCAL([my_unique_namespace_name],[PRINT_HELLO],
               [AS_BANNER(["hello $1"])])

AC_DEFUN_LOCAL([my_unique_namespace_name],[PRINT_HELLO_WORLD],
               [PRINT_HELLO(world)])


