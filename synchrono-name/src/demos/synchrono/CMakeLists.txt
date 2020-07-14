#--------------------------------------------------------------
# OS-specific settings

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  SET (SYN_LINKERFLAG_EXE  "${SYN_LINKERFLAG_EXE} -framework IOKit -framework Cocoa -framework OpenGL")
endif()

# jht : $ENV{USER} might be linux (Arch?) specific
if (DEFINED ENV{PARKST_DIRECTORY})
  set(PARKST_COL_MESH_PATH "$ENV{PARKST_DIRECTORY}/park_street_col_2.obj"
  	CACHE FILEPATH "Location on disk of the Park St. collision mesh")
  set(PARKST_VIS_MESH_PATH "$ENV{PARKST_DIRECTORY}/park_street_vis_split.obj"
  	CACHE FILEPATH "Location on disk of the Park St. visualization mesh")
else()
  set(PARKST_COL_MESH_PATH "$ENV{HOME}/Continental/MeshExport/park_street_col_2.obj"
  	CACHE FILEPATH "Location on disk of the Park St. collision mesh")
  set(PARKST_VIS_MESH_PATH "$ENV{HOME}/Continental/MeshExport/park_street_vis_split.obj"
  	CACHE FILEPATH "Location on disk of the Park St. visualization mesh")
endif()

if(${CMAKE_VERSION} VERSION_LESS "3.12.4")
    add_definitions(-DPARKST_COL_MESH_PATH=${PARKST_COL_MESH_PATH})
    add_definitions(-DPARKST_VIS_MESH_PATH=${PARKST_VIS_MESH_PATH})
else()
    add_compile_definitions(PARKST_COL_MESH_PATH=${PARKST_COL_MESH_PATH})
    add_compile_definitions(PARKST_VIS_MESH_PATH=${PARKST_VIS_MESH_PATH})
endif()

#--------------------------------------------------------------

message(STATUS "Demo programs for Park St...")

#add_subdirectory(demo_parkst)
#add_subdirectory(demo_parkst_citybus)
add_subdirectory(demo_vehicles)
add_subdirectory(demo_platoon)
add_subdirectory(demo_chrono_platoon)
add_subdirectory(demo_lanemerge)
add_subdirectory(demo_lanemerge_withtrigger)
add_subdirectory(demo_collision)

if(SYN_SCM)
    add_subdirectory(demo_SCM)
    add_subdirectory(demo_chrono_SCM)
endif()

if(SYN_SENSOR)
    add_subdirectory(demo_V2V)
endif()

option(USE_TENSOR_RT "Enable the use of TensorRT" OFF)

if(USE_TENSOR_RT AND SYN_SENSOR)
    add_subdirectory(demo_SCM_RL)
    add_subdirectory(demo_GVSETS)
endif()

if(ENABLE_MODULE_INTERFACE)
	option(BUILD_DEMOS_INTERFACE "Build demo programs for Interface module" TRUE)
	if(BUILD_DEMOS_INTERFACE)
		add_subdirectory(demo_interface)
	endif()
endif()
