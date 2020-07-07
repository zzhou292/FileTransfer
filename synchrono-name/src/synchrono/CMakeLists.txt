# ------------------------------------------------------------------------------
# Additional include paths and libraries
# ------------------------------------------------------------------------------

message(STATUS "==== Synchrono core module ====")

# ------------
# Common Files
# ------------

set(SYN_BASE_FILES
	SynApi.h
)
source_group("base" FILES ${SYN_BASE_FILES})

set(SYN_AGENT_FILES
	SynAgent.h
	SynAgent.cpp
	SynVehicleAgent.h
	SynVehicleAgent.cpp

	agent/SynCityBusAgent.h
	agent/SynCityBusAgent.cpp
	agent/SynHMMWVAgent.h
	agent/SynHMMWVAgent.cpp
	agent/SynFullHMMWVAgent.h
	agent/SynFullHMMWVAgent.cpp
	agent/SynSedanAgent.h
	agent/SynSedanAgent.cpp
	agent/SynTrafficLightAgent.h
	agent/SynTrafficLightAgent.cpp
)
source_group("agent" FILES ${SYN_AGENT_FILES})

set(SYN_BRAIN_FILES
	SynBrain.h
	SynBrain.cpp
	SynVehicleBrain.h
	SynVehicleBrain.cpp

	brain/ACCBrain.h
	brain/ACCBrain.cpp
	brain/NOOPBrain.h
	brain/NOOPBrain.cpp
	brain/TrafficLightBrain.h
	brain/TrafficLightBrain.cpp
	brain/TBrain.h
	brain/TBrain.cpp
)
source_group("brain" FILES ${SYN_BRAIN_FILES})

set(SYN_CLI_FILES
	SynCLI.h
	SynCLI.cpp

	cli/cxxopts.hpp
)
source_group("cli" FILES ${SYN_CLI_FILES})

set(SYN_FLATBUFFER_FILES
	SynFlatBuffersManager.h
	SynFlatBuffersManager.cpp

	flatbuffer/SynMessage.h
	flatbuffer/SynMessage.cpp
	flatbuffer/SynMessageFactory.h
	flatbuffer/SynMessageFactory.cpp
	flatbuffer/SynFlatBuffers_generated.h

	flatbuffer/message/SynMAPMessage.h
	flatbuffer/message/SynMAPMessage.cpp
	flatbuffer/message/SynPointerMessage.h
	flatbuffer/message/SynPointerMessage.cpp
	flatbuffer/message/SynSPATMessage.h
	flatbuffer/message/SynSPATMessage.cpp
	flatbuffer/message/SynTrafficLightMessage.h
	flatbuffer/message/SynTrafficLightMessage.cpp
	flatbuffer/message/SynVehicleMessage.h
	flatbuffer/message/SynVehicleMessage.cpp
	flatbuffer/message/SynSensorMessage.h
	flatbuffer/message/SynSensorMessage.cpp
)
if(SYN_SCM)
    list(APPEND SYN_FLATBUFFER_FILES
        flatbuffer/message/SynSCMMessage.h
        # flatbuffer/message/SynSCMMessage.cpp
    )
endif()
source_group("flatbuffer" FILES ${SYN_FLATBUFFER_FILES})

set(SYN_FRAMEWORK_FILES
	SynFramework.h
	SynFramework.cpp

	# TODO: MORE TO BE DONE HERE
)
source_group("frameworks" FILES ${SYN_FRAMEWORK_FILES})

set(SYN_MPI_FILES
	SynMPIManager.h
	SynMPIManager.cpp
)
source_group("mpi" FILES ${SYN_BASE_FILES})

set(SYN_TERRAIN_FILES
	SynTerrain.h
	SynTerrain.cpp

	terrain/SynRigidTerrain.h
	terrain/SynRigidTerrain.cpp
)
if(SYN_SCM)
    list(APPEND SYN_TERRAIN_FILES
        terrain/SynSCMTerrain.h
        terrain/SynSCMTerrain.cpp
    )
endif()
source_group("terrain" FILES ${SYN_TERRAIN_FILES})

set(SYN_VISUALIZATION_FILES
	SynVis.h
	SynVis.cpp
	SynVisManager.h
	SynVisManager.cpp

)
if(SYN_SENSOR)
	list(APPEND SYN_VISUALIZATION_FILES
		visualization/SynSenVehicleVis.h
		visualization/SynSenVehicleVis.cpp
	)
endif()
if(SYN_IRRLICHT)
	list(APPEND SYN_VISUALIZATION_FILES
		visualization/SynIrrVehicleVis.h
		visualization/SynIrrVehicleVis.cpp
	)
endif()
source_group("visualization" FILES ${SYN_VISUALIZATION_FILES})

# -------------------------
# Add the Synchrono library
# -------------------------

add_library(Synchrono SHARED
	${SYN_BASE_FILES}
	${SYN_AGENT_FILES}
	${SYN_BRAIN_FILES}
	${SYN_CLI_FILES}
	${SYN_FLATBUFFER_FILES}
	${SYN_FRAMEWORK_FILES}
	${SYN_MPI_FILES}
	${SYN_TERRAIN_FILES}
	${SYN_VISUALIZATION_FILES}
)

set_target_properties(Synchrono PROPERTIES
                      COMPILE_FLAGS "${SYN_CXX_FLAGS}"
											LINK_FLAGS "${SYN_LINKERFLAG_SHARED} ${CHRONO_LINKER_FLAGS}"
											COMPILE_DEFINITIONS "CHRONO_DATA_DIR=\"${CHRONO_DATA_DIR}\";CHRONO_VEHICLE_DATA_DIR=\"${CHRONO_VEHICLE_DATA_DIR}\";SYN_API_COMPILE")

target_include_directories(Synchrono PUBLIC ${CHRONO_INCLUDE_DIRS})

target_link_libraries(Synchrono ${CHRONO_LIBRARIES} ${LIBRARIES} ${MPI_LIBRARIES})

# -----------
# OS specific
# -----------

if(XCODE_VERSION)
    set_target_properties(ChronoEngine PROPERTIES XCODE_ATTRIBUTE_ENABLE_OPENMP_SUPPORT ${ENABLE_OPENMP})
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	if(MSVC AND MSVC_VERSION GREATER_EQUAL 1900)
	    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /bigobj")
	endif()
endif()
