## ////////////////////////////////////////////////////////////////////////// //
##
## This file is part of the autoconf-bootstrap project.
## Copyright 2018 Andrea Rigoni Garola <andrea.rigoni@igi.cnr.it>.
##
## This program is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>.
##
## ////////////////////////////////////////////////////////////////////////// //




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
	if(/^([[a-zA-Z0-9_\\\\-\\\\.]]+):.*\\\\[#]\\\\[#](?:@(\\\\w+))?\\\\s(.*)\$\$/) { \\\\
	    push(@{\$\$help{\$\$[2]}}, @<:@\$\$[1], \$\$[3]@:>@); \\\\
	} \\\\
    }; \\\\
    print "\\\\n"; \\\\
    print "| \${HELP_DESCRIPTION}\\\\n"; \\\\
    print "| \\\\n"; \\\\
    print "| \${SH_GREEN}usage: make [target]\${SH_RESET}\\\\n"; \\\\
    print "| \\\\n"; \\\\
    for ( sort keys %help ) { \\\\
	print "| \${SH_YELLOW}\$\$_\${SH_RESET}:\\\\n"; \\\\
	printf("|   %-20s %-60s\\\\n", \$\$_->[[0]], \$\$_->[[1]]) for @{\$\$help{\$\$_}}; \\\\
	print "| \\\\n"; \\\\
    } \\\\
    print "\\\\n";

SELFHELP_ADVANCED_FUNC = \\\\
    %help; \\\\
    while(<>) { \\\\
	if(/^([[a-zA-Z0-9_\\\\-\\\\.]]+):.*\\\\[#]\\\\[#](?:@(\\\\w+))?\\\\s(.*)\$\$/) { \\\\
	    push(@{\$\$help{\$\$[2]}}, @<:@\$\$[1], \$\$[3]@:>@); \\\\
	} \\\\
	if(/^([[a-zA-Z0-9_\\\\-\\\\.]]+):.*\\\\[#]\\\\[#](?:@@(\\\\w+))?\\\\s(.*)\$\$/) { \\\\
	    push(@{\$\$help{\$\$[2]}}, @<:@\$\$[1], \$\$[3]@:>@); \\\\
	} \\\\
    }; \\\\
    print "\\\\n"; \\\\
	print ", \${SH_GREEN}ADVANCED TARGETS \${SH_RESET}\\\\n"; \\\\
	print "| \\\\n"; \\\\
	for ( sort keys %help ) { \\\\
	print "| \${SH_YELLOW}\$\$_\${SH_RESET}:\\\\n"; \\\\
	printf("|   %-20s %-60s\\\\n", \$\$_->[[0]], \$\$_->[[1]]) for @{\$\$help{\$\$_}}; \\\\
	print "| \\\\n"; \\\\
	} \\\\
    print "\\\\n";

SELFHELP_PRINT_TARGET_LIST_FUNC = \\\\
    %help; \\\\
    while(<>) { \\\\
    if(/^([[a-zA-Z0-9_\\\\-\\\\.]]+):.*\\\\[#]\\\\[#](?:@(\\\\w+))?\\\\s(.*)\$\$/) { \\\\
    	push(@{\$\$help{\$\$[2]}}, @<:@\$\$[1], \$\$[3]@:>@); } \\\\
    if(/^([[a-zA-Z0-9_\\\\-\\\\.]]+):.*\\\\[#]\\\\[#](?:@@(\\\\w+))?\\\\s(.*)\$\$/) { \\\\
    	push(@{\$\$help{\$\$[2]}}, @<:@\$\$[1], \$\$[3]@:>@); } \\\\
    }; \\\\
    for ( sort keys %help ) { \\\\
    printf("%s ", \$\$_->[[0]]) for @{\$\$help{\$\$_}}; \\\\
    } \\\\

SELFHELP_TARGETS = \$(shell perl -e '\$(SELFHELP_PRINT_TARGET_LIST_FUNC)' \$(MAKEFILE_LIST))

help:         ##@miscellaneous Show this help.
help-more:    ##@miscellaneous get help on advanced targets.
help-targets: ##@@miscellaneous get list of target with help signature

help-targets:
	@ \$(info \$(SELFHELP_TARGETS)):;

help:
	@perl -e '\$(SELFHELP_FUNC)' \$(MAKEFILE_LIST)

help-more:
	@perl -e '\$(SELFHELP_ADVANCED_FUNC)' \$(MAKEFILE_LIST)

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
