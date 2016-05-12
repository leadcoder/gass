#set lib name here!
set(LIB_NAME "SampleSimEditor")

#Set include directories
include_directories(${GASS_SIM_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})

set(CPP_FILES ${LIB_NAME}.cpp)

#Add some folders where to search for dependency libraries
link_directories(${GASS_SIM_LIB_DIRS})

add_executable (${LIB_NAME} ${CPP_FILES} ${H_FILES})

set_target_properties(${LIB_NAME} PROPERTIES DEBUG_POSTFIX _d)

#Add project link libs
target_link_libraries(${LIB_NAME} GASSCore GASSSim  ${GASS_SIM_LIBRARIES})

set(GASS_CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/${LIB_NAME}.xml)

#copy configurations to enable execution from build folder 
FILE(COPY ${GASS_CONFIG} DESTINATION  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
FILE(COPY ${GASS_CONFIG} DESTINATION  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)

#install executable	
install(TARGETS ${LIB_NAME}  RUNTIME DESTINATION ${GASS_INSTALL_BIN_DIR_RELEASE} CONFIGURATIONS Release)
install(TARGETS ${LIB_NAME}  RUNTIME DESTINATION ${GASS_INSTALL_BIN_DIR_DEBUG} CONFIGURATIONS Debug)

#install configuration files
install(FILES ${GASS_CONFIG} DESTINATION ${GASS_INSTALL_BIN_DIR_RELEASE} CONFIGURATIONS Release)
install(FILES ${GASS_CONFIG} DESTINATION ${GASS_INSTALL_BIN_DIR_DEBUG} CONFIGURATIONS Debug)
