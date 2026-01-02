project(LightSPQ)

set(HEADER_FILES
        Common.h
        SupervisorZone.h
        ProcessingNode.h
)

set(SOURCE_FILES
        ProcessingNode.cpp
        SupervisorZone.cpp
)

add_library(LightSPQ STATIC ${SOURCE_FILES} ${HEADER_FILES})
