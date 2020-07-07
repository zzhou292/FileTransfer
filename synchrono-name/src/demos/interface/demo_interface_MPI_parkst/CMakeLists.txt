# ------------------------------------------------------------------------------
# List of demos according their depencies
# ------------------------------------------------------------------------------

# List FSI demos that do not have other dependencies

#--------------------------------------------------------------

if(NOT ENABLE_MODULE_INTERFACE)
	return()
# elseif(SENSOR_MODULE_ENABLED)
#   message("Synchrono_Interface demos requires Chrono::Sensor")
#   message(STATUS "Synchrono_interface demos build disabled")
#   set(BUILD_DEMOS_INTERFACE OFF CACHE BOOL "Enable the Synchrono Interface module" FORCE)
#   return()
endif()

IF(NOT Chrono_HAS_sensor)
  message("Synchrono_Interface demos requires Chrono::Sensor")
  message(STATUS "Synchrono_interface demos build disabled")
  set(BUILD_DEMOS_INTERFACE OFF CACHE BOOL "Enable the Synchrono Interface module" FORCE)
  return()
ELSE()

ENDIF()

INCLUDE_DIRECTORIES(${SYN_INCLUDES} ${CH_INCLUDES})

message(STATUS "synchrono includes: ${SYN_INCLUDES}")
message(STATUS "chrono includes: ${CH_INCLUDES}")

set(SYN_LIBS  Synchrono_agent Synchrono_Interface)

message(STATUS "synchrono libs: ${SYN_LIBS}")

SET(COMPILER_FLAGS "${SYN_CXX_FLAGS}")
SET(LINKER_FLAGS "${SYN_LINKERFLAG_EXE}")
LIST(APPEND LIBS "")


#--------------------------------------------------------------

MESSAGE(STATUS "Synchrono demos -- Park st... ${CHRONO_LINKER_FLAGS}")

add_executable(demo_interface_MPI_parkst demo_interface_MPI_parkst.cpp)

set_target_properties(demo_interface_MPI_parkst PROPERTIES
	    COMPILE_FLAGS "${SYNCHRONO_CXX_FLAGS} ${EXTRA_COMPILE_FLAGS} ${MPI_COMPILE_FLAGS}"
	    COMPILE_DEFINITIONS "CHRONO_DATA_DIR=\"${CHRONO_DATA_DIR}\";CHRONO_VEHICLE_DATA_DIR=\"${CHRONO_VEHICLE_DATA_DIR}\""
			LINK_FLAGS "${SYN_LINKERFLAG_EXE} ${CHRONO_LINKER_FLAGS} ${MPI_LINK_FLAGS}")

target_link_libraries(demo_interface_MPI_parkst ${CHRONO_LIBRARIES} ${SYN_LIBS} ${MPI_LIBRARIES})
