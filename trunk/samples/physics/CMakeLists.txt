#set lib name here!
set(SAMPLE_NAME "SamplePhysics")

#Set include directories
include_directories(${GASS_SIM_INCLUDE_DIRS} "${PROJECT_SOURCE_DIR}/Samples/physics/source")

set(CPP_FILES "source/SamplePhysics.cpp")
set(H_FILES "")

add_executable (${SAMPLE_NAME} ${CPP_FILES} ${H_FILES})
set_target_properties(${SAMPLE_NAME} PROPERTIES DEBUG_POSTFIX _d)

#Add project link libs
target_link_libraries(${SAMPLE_NAME} GASSCore GASSSim  ${GASS_SIM_LIBRARIES})

set(GASS_CONFIG "${CMAKE_CURRENT_SOURCE_DIR}/source/SamplePhysics.xml")

#copy configurations to enable execution from build folder 
FILE(COPY ${GASS_CONFIG} DESTINATION  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
FILE(COPY ${GASS_CONFIG} DESTINATION  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)

#install executable	
install(TARGETS ${SAMPLE_NAME}  RUNTIME DESTINATION ${GASS_INSTALL_BIN_DIR_RELEASE} CONFIGURATIONS Release)
install(TARGETS ${SAMPLE_NAME}  RUNTIME DESTINATION ${GASS_INSTALL_BIN_DIR_DEBUG} CONFIGURATIONS Debug)

#install configuration files
install(FILES ${GASS_CONFIG} DESTINATION ${GASS_INSTALL_BIN_DIR_RELEASE} CONFIGURATIONS Release)
install(FILES ${GASS_CONFIG} DESTINATION ${GASS_INSTALL_BIN_DIR_DEBUG} CONFIGURATIONS Debug)


if(WIN32)
		set(PHYSX3_INSTALL_DIR $ENV{PHYSX_HOME} CACHE PATH "PhysX folder")
		find_package(PhysX3)
		FILE(COPY ${PX_BINARIES_REL} DESTINATION  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
		FILE(COPY ${PX_BINARIES_REL} DESTINATION  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
endif()