#set lib name here!
set(LIB_NAME "SampleRTC")

#Set include directories
include_directories(${GASS_SIM_INCLUDE_DIRS})

set(CPP_FILES "source/SampleRTC.cpp")
set(H_FILES)

#Add some folders where to search for dependency libraries
link_directories(${GASS_CORE_LIB_DIRS})

add_executable (${LIB_NAME} ${CPP_FILES} ${H_FILES})

set_target_properties(${LIB_NAME} PROPERTIES DEBUG_POSTFIX _d)

#Add project link libs
target_link_libraries(${LIB_NAME} GASSCore ${GASS_CORE_LIBRARIES})

#install executable			
install(TARGETS ${LIB_NAME}  RUNTIME DESTINATION ${GASS_INSTALL_BIN_DIR_RELEASE} CONFIGURATIONS Release)
install(TARGETS ${LIB_NAME}  RUNTIME DESTINATION ${GASS_INSTALL_BIN_DIR_DEBUG} CONFIGURATIONS Debug)

