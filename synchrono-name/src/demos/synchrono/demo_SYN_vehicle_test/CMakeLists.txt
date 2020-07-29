# ------------------------------------------------------------------------------
# List of demos according their dependencies
# ------------------------------------------------------------------------------

INCLUDE_DIRECTORIES(${SYN_INCLUDES} ${CH_INCLUDES})

message(STATUS "synchrono includes: ${SYN_INCLUDES}")
message(STATUS "chrono includes: ${CH_INCLUDES}")

set(SYN_LIBS Synchrono)
message(STATUS "synchrono libs: ${SYN_LIBS}")

set(DEMOS
	demo_SYN_vehicles_test
)

SET(COMPILER_FLAGS "${SYN_CXX_FLAGS}")
SET(LINKER_FLAGS "${SYN_LINKERFLAG_EXE}")
LIST(APPEND LIBS "")

#--------------------------------------------------------------

FOREACH(PROGRAM ${DEMOS})
	MESSAGE(STATUS "Synchrono demos -- ${PROGRAM}... ${CHRONO_LINKER_FLAGS}")

	# jht --- again, a hack, think we should just find a way to have MPI not set these. CI seems uneffected by the setting of these
	set(MPI_CXX_COMPILE_OPTIONS "-pthread")
	set(MPI_C_COMPILE_OPTIONS "-pthread")
	set(MPI_COMPILE_FLAGS "-pthread")

	add_executable(${PROGRAM} ${PROGRAM}.cpp)

	set_target_properties(${PROGRAM} PROPERTIES
		    COMPILE_FLAGS "${SYN_CXX_FLAGS} ${EXTRA_COMPILE_FLAGS} ${MPI_COMPILE_FLAGS}"
		    COMPILE_DEFINITIONS "CHRONO_DATA_DIR=\"${CHRONO_DATA_DIR}\";CHRONO_VEHICLE_DATA_DIR=\"${CHRONO_VEHICLE_DATA_DIR}\""
				LINK_FLAGS "${SYN_LINKERFLAG_EXE} ${CHRONO_LINKER_FLAGS} ${MPI_LINK_FLAGS}")

	target_link_libraries(${PROGRAM} ${CHRONO_LIBRARIES} ${SYN_LIBS} ${MPI_LIBRARIES})
ENDFOREACH()
