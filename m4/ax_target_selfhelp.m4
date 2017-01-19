


AC_DEFUN([AX_TARGET_SELFHELP],[
  AC_PUSH_LOCAL([ax_target_selfhelp])
  SET_SELFHELP([uno],[due])
  AC_POP_LOCAL([ax_target_selfhelp])
])


AC_DEFUN_LOCAL([ax_target_selfhelp],[SET_SELFHELP],[
AS_VAR_READ([TARGET_SELFHELP],[

SH_GREEN  ?= \$(shell tput -Txterm setaf 2)
SH_WHITE  ?= \$(shell tput -Txterm setaf 7)
SH_YELLOW ?= \$(shell tput -Txterm setaf 3)
SH_RESET  ?= \$(shell tput -Txterm sgr0)

HELP_DESCRIPTION ?= Documented targets follow

ifndef SELFHELP_FUNC
SELFHELP_FUNC = \\\\
    %help; \\\\
    while(<>) { \\\\
	if(/^([[a-zA-Z0-9_-]]+):.*\\\\[#]\\\\[#](?:@(\\\\w+))?\\\\s(.*)\$\$/) { \\\\
	    push(@{\$\$help{\$\$[2]}}, @<:@\$\$[1], \$\$[3]@:>@); \\\\
	} \\\\
    }; \\\\
    print "\\\\n"; \\\\
    print "| \${HELP_DESCRIPTION}\\\\n"; \\\\
    print "| \\\\n"; \\\\
    print "| \${SH_GREEN}usage: make [target]\${SH_RESET}\\\\n"; \\\\
    print "| \\\\n"; \\\\
    for ( sort keys %help ) { \\\\
	print "| \${SH_YELLOW}\$\$_:\${SH_RESET}\\\\n"; \\\\
	printf("|   %-20s %-60s\\\\n", \$\$_->[[0]], \$\$_->[[1]]) for @{\$\$help{\$\$_}}; \\\\
	print "| \\\\n"; \\\\
    } \\\\
    print "\\\\n";

help:   ##@miscellaneous Show this help.
	@perl -e '\$(SELFHELP_FUNC)' \$(MAKEFILE_LIST)
endif

])
 AC_SUBST([TARGET_SELFHELP])
 m4_ifdef([AM_SUBST_NOTMAKE], [AM_SUBST_NOTMAKE([TARGET_SELFHELP])])
])


AC_DEFUN_LOCAL([ax_target_selfhelp],[AS_VAR_READ],[
read -d '' $1 << _as_read_EOF
$2
_as_read_EOF
])




dnl SELFHELP_FUNC = \
dnl    %help; \
dnl    while(<>) { \
dnl        if(/^([a-z0-9_-]+):.*\#\#(?:@(\w+))?\s(.*)$$/) { \
dnl 	   push(@{$$help{$$2}}, [$$1, $$3]); \
dnl        } \
dnl    }; \
dnl    print "${GREEN}usage: make [target]${WHITE}\n\n"; \
dnl    for ( sort keys %help ) { \
dnl        print "${YELLOW}$$_${WHITE}:\n"; \
dnl        printf("  %-20s %s\n", $$_->[0], $$_->[1]) for @{$$help{$$_}}; \
dnl        print "\n"; \
dnl    }
