# ------------------------------------------------------------------------------
# List of demos according their depencies
# ------------------------------------------------------------------------------

# List all FSI demos that use chrono
SET(FSI_DEMOS # add fluid demos here
demo_FSI_DamBreak
demo_FSI_Compressibility
demo_FSI_Poiseuille_flow
demo_FSI_CylinderDrop
demo_FSI_Granular_ConeDrop
demo_FSI_CylinderRotate_granular
demo_FSI_DamBreak_granular
demo_FSI_ConeDrop_granular
demo_FSI_SphereDrop_granular
demo_FSI_DamBreak_Obstacle_granular
demo_FSI_Landslide_granular
demo_FSI_AngleRepose_granular
demo_FSI_SweepTool_granular
demo_FSI_SweepTool_granular_NSC
demo_FSI_Rover_granular_NSC
)

# List all FSI demos that use chrono-parallel
SET(FSI_PARALLEL_DEMOS
)


#add fluid demos + chrono_fea + chrono_mkl here
SET(FSI_MKL_DEMOS 
demo_FSI_Flexible_Elements
)

#add fluid demos + chrono_fea + chrono_mkl that need to be built with nvcc
SET(FSI_FEA_MKL_NVCC_DEMOS
)


#--------------------------------------------------------------
SET(LIBRARIES
    ChronoEngine
    ChronoEngine_fsi
)
message( STATUS "FSI_DEMO_LIBRARY: ${ChronoEngine_fsi}")


INCLUDE_DIRECTORIES(${CH_FSI_INCLUDES})
INCLUDE_DIRECTORIES(${CH_FEA_INCLUDES})

SET(COMPILER_FLAGS "${CH_CXX_FLAGS}")
SET(LINKER_FLAGS "${CH_LINKERFLAG_EXE}")
LIST(APPEND LIBS "")

IF(ENABLE_MODULE_MKL)
  INCLUDE_DIRECTORIES(${CH_MKL_INCLUDES})
  SET(COMPILER_FLAGS "${COMPILER_FLAGS} ${CH_MKL_CXX_FLAGS}")
  SET(LINKER_FLAGS "${LINKER_FLAGS} ${CH_MKL_LINK_FLAGS}")
  LIST(APPEND LIBS "ChronoEngine_mkl")
ENDIF()

MESSAGE(STATUS "Demo programs for FSI module...")

# Add executables for demos that have no other dependencies

	
	
IF(ENABLE_MODULE_MKL)

   INCLUDE_DIRECTORIES(${CH_MKL_INCLUDES})
   
    FOREACH(PROGRAM ${FSI_MKL_DEMOS})
        MESSAGE(STATUS "...add ${PROGRAM}")

        CUDA_ADD_EXECUTABLE(${PROGRAM}  "${PROGRAM}.cpp")
        SOURCE_GROUP(""  FILES  "${PROGRAM}.cpp")    
        SET_TARGET_PROPERTIES(${PROGRAM} PROPERTIES 
            FOLDER demos
            COMPILE_FLAGS "${CH_CXX_FLAGS} ${CH_MKL_CXX_FLAGS} ${CH_PARALLEL_CXX_FLAGS}"
            LINK_FLAGS "${CH_LINKERFLAG_EXE} ${CH_MKL_LINK_FLAGS} ")
        TARGET_LINK_LIBRARIES(${PROGRAM}
        	${LIBRARIES}
            ChronoEngine
            
            ChronoEngine_mkl
            ChronoEngine_fsi)
        TARGET_INCLUDE_DIRECTORIES(${PROGRAM} PUBLIC
		${CH_MKL_INCLUDES})
        ADD_DEPENDENCIES(${PROGRAM}
         	${LIBRARIES}
            ChronoEngine
            ChronoEngine_mkl
            ChronoEngine_fsi)
        INSTALL(TARGETS ${PROGRAM} DESTINATION bin)
    ENDFOREACH()
    
    
    FOREACH(PROGRAM ${FSI_MKL_NVCC_DEMOS})
        MESSAGE(STATUS "...add ${PROGRAM}")

        CUDA_ADD_EXECUTABLE(${PROGRAM}  "${PROGRAM}.cu")
        SOURCE_GROUP(""  FILES  "${PROGRAM}.cu")    
        SET_TARGET_PROPERTIES(${PROGRAM} PROPERTIES 
            FOLDER demos
            COMPILE_FLAGS "${CH_CXX_FLAGS} ${CH_MKL_CXX_FLAGS}"
            LINK_FLAGS "${CH_LINKERFLAG_EXE} ${CH_MKL_LINK_FLAGS}  -lstdc++")
        TARGET_LINK_LIBRARIES(${PROGRAM}
        	${LIBRARIES}
            ChronoEngine
            
            ChronoEngine_mkl
            ChronoEngine_fsi)
        TARGET_INCLUDE_DIRECTORIES(${PROGRAM} PUBLIC
		${CH_MKL_INCLUDES})
        ADD_DEPENDENCIES(${PROGRAM}
         	${LIBRARIES}
            ChronoEngine
            ChronoEngine_mkl
            ChronoEngine_fsi)
        INSTALL(TARGETS ${PROGRAM} DESTINATION bin)
    ENDFOREACH()
        
ENDIF()

	    
IF(ENABLE_MODULE_PARALLEL)
    
    INCLUDE_DIRECTORIES(${CH_PARALLEL_INCLUDES})
		
	FOREACH(PROGRAM ${FSI_PARALLEL_DEMOS})
		MESSAGE(STATUS "...add ${PROGRAM}")

		CUDA_ADD_EXECUTABLE(${PROGRAM}  "${PROGRAM}.cpp")
		SOURCE_GROUP(""  FILES "${PROGRAM}.cpp")

		SET_TARGET_PROPERTIES(${PROGRAM} PROPERTIES
			FOLDER demos
			COMPILE_FLAGS "${CH_CXX_FLAGS} ${CH_PARALLEL_CXX_FLAGS}"
			LINK_FLAGS "${CH_LINKERFLAG_EXE}"
		)

		TARGET_LINK_LIBRARIES(${PROGRAM} ${LIBRARIES})
		ADD_DEPENDENCIES(${PROGRAM} ${LIBRARIES})

		INSTALL(TARGETS ${PROGRAM} DESTINATION ${CH_INSTALL_DEMO})
	ENDFOREACH()

ENDIF()
	



FOREACH(PROGRAM ${FSI_DEMOS})
  MESSAGE(STATUS "...add ${PROGRAM}")

  CUDA_ADD_EXECUTABLE(${PROGRAM}  "${PROGRAM}.cpp")
  SOURCE_GROUP(""  FILES "${PROGRAM}.cpp")

  SET_TARGET_PROPERTIES(${PROGRAM} PROPERTIES
    FOLDER demos
    COMPILE_FLAGS "${COMPILER_FLAGS}"
    LINK_FLAGS "${LINKER_FLAGS}")
  TARGET_LINK_LIBRARIES(${PROGRAM} ChronoEngine ChronoEngine_fsi ${LIBS})

  INSTALL(TARGETS ${PROGRAM} DESTINATION ${CH_INSTALL_DEMO})
ENDFOREACH()





