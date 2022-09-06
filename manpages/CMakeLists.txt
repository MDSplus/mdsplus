
###
### share/man/*
###

INSTALL(
    FILES mdsplus.groff RENAME mdsplus.1
    DESTINATION share/man/man1
)

INSTALL(
    FILES traverser.groff RENAME traverser.1
    DESTINATION share/man/man1
)

INSTALL(
    FILES mdsip.hosts.groff RENAME mdsip.hosts.5
    DESTINATION share/man/man5
)

INSTALL(
    FILES mdsplus.conf.groff RENAME mdsplus.conf.5
    DESTINATION share/man/man5
)