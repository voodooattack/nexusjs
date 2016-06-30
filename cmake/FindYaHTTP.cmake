# Locate YaHTTP
# This module defines
# YAHTTP_LIBRARY
# YAHTTP_FOUND, if false, do not try to link to YaHTTP
# YAHTTP_INCLUDE_DIR, where to find the headers

FIND_PATH(YAHTTP_INCLUDE_DIR yahttp
    ${YAHTTP_DIR}/yahttp
    $ENV{YAHTTP_DIR}/yahttp
    $ENV{YAHTTP_DIR}
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local/include
    /usr/include
    /sw/include # Fink
    /opt/local/include # DarwinPorts
    /opt/csw/include # Blastwave
    /opt/include
    /usr/freeware/include
    /devel
)

IF(YAHTTP_INCLUDE_DIR)
  SET(YAHTTP_INCLUDE_DIR ${YAHTTP_INCLUDE_DIR}/yahttp/)
ENDIF(YAHTTP_INCLUDE_DIR)

FIND_LIBRARY(YAHTTP_LIBRARY
    NAMES libyahttp yahttp
    PATHS
    ${YAHTTP_DIR}
    ${YAHTTP_DIR}/lib
    $ENV{YAHTTP_DIR}
    $ENV{YAHTTP_DIR}/lib
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local/lib
    /usr/lib
    /sw/lib
    /opt/local/lib
    /opt/csw/lib
    /opt/lib
    /usr/freeware/lib64
)

SET(YAHTTP_FOUND "NO")
IF(YAHTTP_LIBRARY AND YAHTTP_INCLUDE_DIR)
    SET(YAHTTP_FOUND "YES")
ENDIF(YAHTTP_LIBRARY AND YAHTTP_INCLUDE_DIR)


