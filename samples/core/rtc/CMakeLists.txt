#set lib name here!
set(LIB_NAME "SampleCoreRTC")

set(CPP_FILES ${LIB_NAME}.cpp)
set(H_FILES)

add_executable (${LIB_NAME} ${CPP_FILES} ${H_FILES})
target_link_libraries(${LIB_NAME} GASSCore ${GASS_CORE_LIBRARIES})

#install executable			
install(TARGETS ${LIB_NAME}  RUNTIME DESTINATION ${GASS_INSTALL_BIN_DIR_RELEASE} CONFIGURATIONS Release)
install(TARGETS ${LIB_NAME}  RUNTIME DESTINATION ${GASS_INSTALL_BIN_DIR_DEBUG} CONFIGURATIONS Debug)

