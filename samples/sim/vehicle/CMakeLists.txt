#set lib name here!
set(LIB_NAME "SampleSimVehicle")

#Set include directories
#include_directories(${GASS_SIM_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})

set(CPP_FILES ${LIB_NAME}.cpp)

add_executable (${LIB_NAME} ${CPP_FILES} ${H_FILES})

set_target_properties(${LIB_NAME} PROPERTIES DEBUG_POSTFIX _d)

target_link_libraries(${LIB_NAME} GASSSim)
#target_include_directories(${LIB_NAME} PRIVATE  $<BUILD_INTERFACE:${INC_DIR}>)

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
