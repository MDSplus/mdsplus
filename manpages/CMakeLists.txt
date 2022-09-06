
###
### share/man/*
###

install(
    FILES mdsplus.groff RENAME mdsplus.1
    DESTINATION share/man/man1
)

install(
    FILES traverser.groff RENAME traverser.1
    DESTINATION share/man/man1
)

install(
    FILES mdsip.hosts.groff RENAME mdsip.hosts.5
    DESTINATION share/man/man5
)

install(
    FILES mdsplus.conf.groff RENAME mdsplus.conf.5
    DESTINATION share/man/man5
)