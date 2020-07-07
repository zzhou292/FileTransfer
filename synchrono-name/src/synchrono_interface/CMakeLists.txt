option(ENABLE_MODULE_INTERFACE "Enable the Synchrono Interface module" OFF)

IF(NOT ENABLE_MODULE_INTERFACE)
  return()
ENDIF()

message(STATUS "==== Synchrono Interface Component ====")

# ----- BOOST -- required for communication -----

set(BOOST_ROOT "" CACHE PATH "Where is Boost located?")
find_package(Boost REQUIRED)

if (Boost_FOUND)
  message(STATUS "Boost include dir: ${Boost_INCLUDE_DIRS}")
else()
  mark_as_advanced(CLEAR BOOST_ROOT)
endif()

set(SYN_INTERFACE_INCLUDES
	${SYN_WORLD_INCLUDES}
	${Boost_INCLUDE_DIRS}
  ${Flatbuffers_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/transport
	PARENT_SCOPE
)

set(SYN_INTERFACE_INCLUDES "${SYN_INTERFACE_INCLUDES}" PARENT_SCOPE)

set(SYN_INTERFACE_FILES
    SynInterface.h
    SynInterface.cpp
    SynInterfaceData.h
    SynInterfaceData.cpp
    SynInterfaceDriver.h
    SynInterfaceDriver.cpp
    SynComponent.h
)
source_group("interface" FILES ${SYN_INTERFACE_FILES})

set(SYN_INTERFACE_TRANSPORT_FILES
    transport/SynTransportManager.h
    transport/SynTransportManager.cpp
)
source_group("transport" FILES ${SYN_INTERFACE_TRANSPORT_FILES})


set(SYN_INTERFACE_FLATBUFFERS_FILES
    # flatbuffers/SynFlatBuffersManager.h
    # flatbuffers/SynFlatBuffersManager.cpp
)
source_group("flatbuffers" FILES ${SYN_INTERFACE_FLATBUFFERS_FILES})

add_library(Synchrono_Interface SHARED
    ${SYN_INTERFACE_FILES}
    ${SYN_INTERFACE_TRANSPORT_FILES}
    ${SYN_INTERFACE_FLATBUFFERS_FILES}
)

# set(LIBRARIES "${LIBRARIES}")


message(status "linker flag: ${SYN_LINKERFLAG_SHARED}")

set_target_properties(Synchrono_Interface PROPERTIES
                      COMPILE_FLAGS "${SYN_CXX_FLAGS}"
                      LINK_FLAGS "${SYN_LINKERFLAG_SHARED} ${CHRONO_LINKER_FLAGS}"
				  COMPILE_DEFINITIONS "SYN_API_COMPILE_INTERFACE")

target_include_directories(Synchrono_Interface PUBLIC ${CHRONO_INCLUDE_DIRS})

target_link_libraries(Synchrono_Interface ${CHRONO_LIBRARIES} ${FLATBUFFER_LIBRARY} ${LIBRARIES})
#
# install(TARGETS Synchrono_Interface
#     RUNTIME DESTINATION bin
#     LIBRARY DESTINATION lib
#     ARCHIVE DESTINATION lib)
#
# install(FILES ${ChronoEngine_FSI_HEADERS}
# 	DESTINATION include/chrono_interface)
