# option(ENABLE_MODULE_R "Enable the Synchrono R module" OFF)
#
# IF(NOT ENABLE_MODULE_R)
#   return()
# ENDIF()

message(STATUS "==== Synchrono R Component ====")


# set(SYN_R_INCLUDES "${CUDA_TOOLKIT_ROOT_DIR}/include")
set(SYN_R_INCLUDES "${SYN_R_INCLUDES}" PARENT_SCOPE)

set(SYN_R_SOURCES
	testFunctions.cpp
)

set(SYN_R_HEADERS
	testFunctions.h
)
source_group("R" FILES
    ${SYN_R_SOURCES}
    ${SYN_R_HEADERS}
)

add_library(Synchrono_R SHARED
    ${SYN_R_SOURCES}
    ${SYN_R_HEADERS}
)

set_target_properties(Synchrono_R PROPERTIES
                      COMPILE_FLAGS "${SYN_CXX_FLAGS}"
                      LINK_FLAGS "${SYN_LINKERFLAG_SHARED} ${CHRONO_LINKER_FLAGS}"
				  COMPILE_DEFINITIONS "SYN_API_COMPILE_R")

target_link_libraries(Synchrono_R ${CHRONO_LIBRARIES})
