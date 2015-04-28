#set lib name here!
set(LIB_NAME "SampleGUI")


find_package(MyGUI)
#Set include directories
include_directories(${GASS_SIM_INCLUDE_DIRS} "${PROJECT_SOURCE_DIR}/Samples/gui/source" ${MYGUI_INCLUDE_DIRS})

set(CPP_FILES "source/SampleGUI.cpp" 
			  "source/SampleDialog.cpp")
set(H_FILES  "source/SampleDialog.h")

			  #set(XML_FILES "source/GUISample.xml")
#link_directories(${GASS_SIM_LIB_DIRS})
add_executable (${LIB_NAME} ${CPP_FILES} ${H_FILES})
set_target_properties(${LIB_NAME} PROPERTIES DEBUG_POSTFIX _d)

#Add project link libs
target_link_libraries(${LIB_NAME} GASSCore GASSSim  ${GASS_SIM_LIBRARIES} ${MYGUI_LIBRARIES})

set(GASS_CONFIG "${CMAKE_CURRENT_SOURCE_DIR}/source/SampleGUI.xml")

#copy configurations to enable execution from build folder 
FILE(COPY ${GASS_CONFIG} DESTINATION  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
FILE(COPY ${GASS_CONFIG} DESTINATION  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)

if(WIN32)
	#move this to parrent cmakefile and check if osg plugin is present
	find_package(OSGExt 3.2.1 REQUIRED osgUtil osgDB osgGA osgVolume osgFX osgText osgShadow osgViewer osgSim osgTerrain)
	FILE(COPY ${OSG_BINARIES_REL} DESTINATION  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	FILE(COPY ${OSG_BINARIES_DBG} DESTINATION  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
	
	FILE(COPY ${OSGPLUGIN_BINARIES_REL} DESTINATION  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	FILE(COPY ${OSGPLUGIN_BINARIES_DBG} DESTINATION  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)

	find_package(MyGUI)
	FILE(COPY ${MYGUI_BINARIES_REL} DESTINATION  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
	FILE(COPY ${MYGUI_BINARIES_DBG} DESTINATION  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
endif()

#install executable			
install(TARGETS ${LIB_NAME}  RUNTIME DESTINATION ${GASS_INSTALL_BIN_DIR_RELEASE} CONFIGURATIONS Release)
install(TARGETS ${LIB_NAME}  RUNTIME DESTINATION ${GASS_INSTALL_BIN_DIR_DEBUG} CONFIGURATIONS Debug)

#install configuration files
install(FILES ${GASS_CONFIG} DESTINATION ${GASS_INSTALL_BIN_DIR_RELEASE} CONFIGURATIONS Release)
install(FILES ${GASS_CONFIG} DESTINATION ${GASS_INSTALL_BIN_DIR_DEBUG} CONFIGURATIONS Debug)