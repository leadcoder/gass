#set lib name here!
set(LIB_NAME "SimDemo")

#Set include directiores
include_directories(${GASS_COMMON_INCLUDE_DIRS} "${PROJECT_SOURCE_DIR}/Samples/sim_demo/source")

set(CPP_FILES "source/SimDemo.cpp")
set(H_FILES "source/Client.h" "source/Server.h" "source/SimApplication.h")

#Add some foldets where to search for dependency libraries
link_directories(${GASS_COMMON_LIB_DIRS})
add_executable (${LIB_NAME} ${CPP_FILES} ${H_FILES})

#Add project dependency libs
target_link_libraries(${LIB_NAME} GASSSim GASSCore ${GASS_COMMON_LINK_LIBRARIES})
install (TARGETS ${LIB_NAME} DESTINATION ${GASS_INSTALL_SAMPLES_DIR}/${LIB_NAME}/bin)
#install config files
FILE(GLOB GASS_CONFIG "${CMAKE_CURRENT_SOURCE_DIR}/bin/Configuration/*.xml")
install(FILES ${GASS_CONFIG} DESTINATION ${GASS_INSTALL_SAMPLES_DIR}/${LIB_NAME}/Configuration)
install(DIRECTORY ../common DESTINATION ${GASS_INSTALL_SAMPLES_DIR} FILES_MATCHING PATTERN "*.*" PATTERN ".svn" EXCLUDE)
#ADD_CUSTOM_COMMAND(TARGET SimDemo
 #         POST_BUILD
         # COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}
	 
 #         COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_INSTALL_PREFIX}/${GASS_INSTALL_LIB_DIR}/*.dll ${GASS_INSTALL_SAMPLES_DIR}/${LIB_NAME}/bin/.)
#install shared files if windows
if(${WIN32})
	set(DEP_DLLS 
	${GASS_DEPENDENCIES_DIR}/lib/tbb.dll 
	${GASS_DEPENDENCIES_DIR}/lib/tbb_debug.dll 
	${GASS_DEPENDENCIES_DIR}/lib/OpenAL32.dll
	${GASS_DEPENDENCIES_DIR}/lib/OIS.dll
	${GASS_DEPENDENCIES_DIR}/lib/OIS_d.dll)
	install (FILES  ${DEP_DLLS} DESTINATION ${GASS_INSTALL_SAMPLES_DIR}/${LIB_NAME}/bin)
	#install GASS files, maybe applications can share this?
	#FILE(GLOB GASS_DLLS "${CMAKE_INSTALL_PREFIX}/${GASS_INSTALL_LIB_DIR}/*.dll")
	#MESSAGE(${GASS_DLLS})
	#install(FILES ${GASS_DLLS} DESTINATION ${GASS_INSTALL_SAMPLES_DIR}/${LIB_NAME}/bin)
endif()