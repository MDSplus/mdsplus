#!/bin/bash
#
# Check commit title format of all commits since original commit.
#
# The original commit is passed in as an argument ("origin" for pull requests and
# the last release tag for release builds.
# 
# The second argument to this script should be the filespec of the contents of an
# email to be sent to the person who created a commit with an invalid title.
#

git log $1..HEAD --no-merges --decorate=short --pretty=format:"%<(80,trunc)%s%n%ce" |
awk -v EMAILMSG="$2" -F: '{ IGNORECASE=1
               TITLE=$0
               switch ($1) {
               case "Feature":
	       case "Revert \"Feature":
                 VERSION="MINOR"
                 OK="1"
                 break
               case "Fix":
	       case "Revert \"Fix":
                 if ( VERSION == "SAME" ) {
                   VERSION="PATCH"
                 }
                 OK="1"
                 break 
               case "Tests":
	       case "Revert \"Tests":
               case "Docs": 
	       case "Revert \"Docs":
               case "Build":
	       case "Revert \"Build":
                 
                 OK="1"
                 break
               default:
                 OK="0"
                 FAIL="TRUE"
                 break
               }
               getline
               EMAIL=$0
               if (OK != "1") {
                 system("mail -s \"" TITLE "\" "  EMAIL " < " EMAILMSG )
               }
            }
            END {
              if ( VERSION == "" ) VERSION="SAME";
              if ( FAIL == "TRUE" && VERSION == "SAME" ) {
                print("BADCOMMIT")
              } else {
                print(VERSION)
              }
            }'

