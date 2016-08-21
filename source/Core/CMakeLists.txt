#set lib name here!
set(LIB_NAME "GASSCore")

find_package(TinyXML2 REQUIRED)
find_package(Boost 1.46.1 REQUIRED filesystem system)

#TBB
find_package(TBB REQUIRED)
#post-process results 
set(TBB_LIBRARIES optimized ${TBB_LIBRARY}
	 optimized ${TBB_MALLOC_LIBRARY}
	 debug ${TBB_LIBRARY_DEBUG}
	 debug ${TBB_MALLOC_LIBRARY_DEBUG})

	 
#add all source files from current directory (recursively)
set(CPP_FILES
	ComponentSystem/GASSComponent.cpp
	ComponentSystem/GASSComponentContainer.cpp
	ComponentSystem/GASSComponentContainerFactory.cpp
	ComponentSystem/GASSComponentContainerTemplate.cpp
	ComponentSystem/GASSComponentContainerTemplateFactory.cpp
	ComponentSystem/GASSComponentContainerTemplateManager.cpp
	ComponentSystem/GASSComponentFactory.cpp
	ComponentSystem/GASSSchemaManager.cpp
	Math/GASSAABox.cpp
	Math/GASSEllipsoid.cpp
	Math/GASSKeyframeAnimation.cpp
	Math/GASSMath.cpp
	Math/GASSMatrix.cpp
	Math/GASSPlane.cpp
	Math/GASSPolygon.cpp
	Math/GASSQuaternion.cpp
	Math/GASSRotationSpline.cpp
	Math/GASSSphere.cpp
	Math/GASSSpline.cpp
	Math/GASSSplineAnimation.cpp
	Math/GASSVector.cpp
	MessageSystem/GASSBaseMessage.cpp
	MessageSystem/GASSMessageManager.cpp
	PluginSystem/GASSDynamicModule.cpp
	PluginSystem/GASSPluginManager.cpp
	Reflection/GASSBaseReflectionObject.cpp
	Reflection/GASSIProperty.cpp
	Reflection/GASSProperty.cpp
	RTC/GASSTaskNode.cpp
	RTC/GASSTBBManager.cpp
	Serialize/GASSISerialize.cpp
	Serialize/GASSIXMLSerialize.cpp
	Serialize/GASSSerialize.cpp
	Utils/GASSEnumLookup.cpp
	Utils/GASSException.cpp
	Utils/GASSFilePath.cpp
	Utils/GASSFileUtils.cpp
	Utils/GASSHeightField.cpp
	Utils/GASSLog.cpp
	Utils/GASSLogManager.cpp
	Utils/GASSPIDControl.cpp
	Utils/GASSRawFile.cpp
	Utils/GASSStringUtils.cpp
	Utils/GASSTimer.cpp
	Utils/GASSXMLUtils.cpp
)

set(H_FILES
	Common.h
	Prerequisits.h
	ComponentSystem/GASSComponent.h
	ComponentSystem/GASSComponentContainer.h
	ComponentSystem/GASSComponentContainerFactory.h
	ComponentSystem/GASSComponentContainerTemplate.h
	ComponentSystem/GASSComponentContainerTemplateFactory.h
	ComponentSystem/GASSComponentContainerTemplateManager.h
	ComponentSystem/GASSComponentFactory.h
	ComponentSystem/GASSSchemaManager.h
	Math/GASSAABox.h
	Math/GASSEllipsoid.h
	Math/GASSKeyframeAnimation.h
	Math/GASSLineSegment.h
	Math/GASSMath.h
	Math/GASSMatrix.h
	Math/GASSPlane.h
	Math/GASSPolygon.h
	Math/GASSQuaternion.h
	Math/GASSRay.h
	Math/GASSRotationSpline.h
	Math/GASSSphere.h
	Math/GASSSpline.h
	Math/GASSSplineAnimation.h
	Math/GASSTriangle.h
	Math/GASSVector.h
	MessageSystem/GASSBaseMessage.h
	MessageSystem/GASSIMessage.h
	MessageSystem/GASSMessageManager.h
	MessageSystem/GASSMessageType.h
	MessageSystem/GASSStaticMessageListener.h
	PluginSystem/GASSDynamicModule.h
	PluginSystem/GASSPluginManager.h
	Reflection/GASSBaseReflectionObject.h
	Reflection/GASSClassMetaData.h
	Reflection/GASSIProperty.h
	Reflection/GASSNullClass.h
	Reflection/GASSProperty.h
	Reflection/GASSPropertyMetaData.h
	Reflection/GASSReflection.h
	Reflection/GASSRTTI.h
	Reflection/GASSTypedProperty.h
	RTC/GASSTaskNode.h
	RTC/GASSTBBManager.h
	Serialize/GASSISerialize.h
	Serialize/GASSIXMLSerialize.h
	Serialize/GASSSerialize.h
	Serialize/GASSSTDSerialize.h
	Utils/GASSAny.h
	Utils/GASSColorRGB.h
	Utils/GASSColorRGBA.h
	Utils/GASSEnumBinder.h
	Utils/GASSEnumLookup.h
	Utils/GASSException.h
	Utils/GASSFactory.h
	Utils/GASSFilePath.h
	Utils/GASSFilesystem.h
	Utils/GASSFileUtils.h
	Utils/GASSFloatArray16.h
	Utils/GASSHeightField.h
	Utils/GASSIterators.h
	Utils/GASSLog.h
	Utils/GASSLogManager.h
	Utils/GASSLokiTypeInfo.h
	Utils/GASSPIDControl.h
	Utils/GASSRawFile.h
	Utils/GASSSingleton.h
	Utils/GASSStringUtils.h
	Utils/GASSTimer.h
	Utils/GASSXMLUtils.h
)

add_library (${LIB_NAME} ${GASS_BUILDTYPE} ${CPP_FILES} ${H_FILES})

target_include_directories(${LIB_NAME}
	PUBLIC  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
	PUBLIC  $<BUILD_INTERFACE:${Boost_INCLUDE_DIR}>
	PUBLIC  $<BUILD_INTERFACE:${TINYXML2_INCLUDE_DIRS}>
	PUBLIC  $<BUILD_INTERFACE:${TBB_INCLUDE_DIRS}>)

target_link_libraries(${LIB_NAME}
		PRIVATE ${TINYXML2_LIBRARIES}
		PUBLIC ${TBB_LIBRARIES}
		PUBLIC ${Boost_LIBRARIES})
		
if(WIN32)
   #Add dependency for timer
   target_link_libraries(${LIB_NAME} PRIVATE Winmm)
endif()

target_compile_definitions(${LIB_NAME} PRIVATE GASS_CORE_EXPORTS ${GASS_COMMON_DEFINITIONS} )

#Install section
gass_install_target()

if(GASS_INSTALL_DEP_BINARIES)
		install(FILES ${TBB_BINARIES_REL} DESTINATION ${GASS_INSTALL_BIN_DIR_RELEASE} CONFIGURATIONS Release)
		install(FILES ${TBB_BINARIES_DBG} DESTINATION ${GASS_INSTALL_BIN_DIR_DEBUG} CONFIGURATIONS Debug)

		FILE(COPY ${TBB_BINARIES_REL} DESTINATION  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/release)
		FILE(COPY ${TBB_BINARIES_DBG} DESTINATION  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/debug)
endif()

# install(TARGETS ${LIB_NAME} EXPORT LibraryTargets
  # LIBRARY DESTINATION lib
  # ARCHIVE DESTINATION lib
  # RUNTIME DESTINATION bin
  # INCLUDES DESTINATION include
# )

#install(
#  FILES
#	${lib_HEADERS}
#  DESTINATION
#    ${lib_INSTALL_HEADER_DIRECTORY}
#  COMPONENT
#    Devel
#)

foreach ( h_file ${H_FILES} )
    get_filename_component( dir ${h_file} DIRECTORY )
    install( FILES ${h_file} DESTINATION include/Core/${dir} )
endforeach()

#install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION ${GASS_INSTALL_INCLUDE_DIR} FILES_MATCHING PATTERN "*.h" PATTERN ".svn" EXCLUDE)


