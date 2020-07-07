# ------------------------------------------------------------------------------
# List of demos according their depencies
# ------------------------------------------------------------------------------

# List FSI demos that do not have other dependencies

#--------------------------------------------------------------

if(NOT ENABLE_MODULE_INTERFACE)
	return()
endif()

IF(NOT Chrono_HAS_sensor)
  message("Synchrono_Interface demos requires Chrono::Sensor")
  message(STATUS "Synchrono_interface demos build disabled")
  set(BUILD_DEMOS_INTERFACE OFF CACHE BOOL "Enable the Synchrono Interface module" FORCE)
  return()
ELSE()

ENDIF()

INCLUDE_DIRECTORIES(${SYN_INCLUDES})

message(STATUS "synchrono includes: ${SYN_INCLUDES}")
message(STATUS "chrono includes: ${CH_INCLUDE_DIRS}")

set(SYN_LIBS  Synchrono_World Synchrono_Models Synchrono_Interface)

message(STATUS "synchrono libs: ${SYN_LIBS}")

SET(COMPILER_FLAGS "${SYN_CXX_FLAGS}")
SET(LINKER_FLAGS "${SYN_LINKERFLAG_EXE}")
LIST(APPEND LIBS "")


#--------------------------------------------------------------
set(SYNCHRONO_DATA_DIR "/home/aaron/chrono/data/")
set(SYNCHRONO_VEHICLE_DATA_DIR "/home/aaron/chrono/data/vehicle/")
set(SYNCHRONO_INTERFACE_DATA_DIR "/home/aaron/chrono/data/interface/")
#--------------------------------------------------------------

MESSAGE(STATUS "Demo programs for testing if Synchrono builds...")

add_executable(demo_interface demo_interface.cpp)

set_target_properties(demo_interface PROPERTIES
	    COMPILE_FLAGS "${CHRONO_CXX_FLAGS} ${EXTRA_COMPILE_FLAGS}"
	    COMPILE_DEFINITIONS "CHRONO_DATA_DIR=\"${CHRONO_DATA_DIR}\""
	    LINK_FLAGS "${CHRONO_LINKER_FLAGS} ${CHRONO_LINKER_FLAGS}")

TARGET_COMPILE_DEFINITIONS(demo_interface
    PUBLIC "SYNCHRONO_DATA_DIR=\"${SYNCHRONO_DATA_DIR}\""
    PUBLIC "SYNCHRONO_VEHICLE_DATA_DIR=\"${SYNCHRONO_VEHICLE_DATA_DIR}\""
    PUBLIC "SYNCHRONO_INTERFACE_DATA_DIR=\"${SYNCHRONO_INTERFACE_DATA_DIR}\""
)

target_link_libraries(demo_interface ${CHRONO_LIBRARIES} ${SYN_LIBS})
