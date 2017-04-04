
if(GASS_EMBED_GIT_SHA)
	configure_file(
		"${CMAKE_CURRENT_SOURCE_DIR}/GASSVersion.h.in"
		"${CMAKE_CURRENT_BINARY_DIR}/GASSVersion.h" 
		@ONLY)
 	set(GASS_VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/GASSVersion.h")	
endif()

configure_file(
		"${CMAKE_CURRENT_SOURCE_DIR}/GASSConfig.h.in"
		"${CMAKE_CURRENT_BINARY_DIR}/GASSConfig.h" 
		@ONLY)
set(GASS_CONFIG_HEADER "${CMAKE_CURRENT_BINARY_DIR}/GASSConfig.h")	
	 
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
	Reflection/GASSProperty.cpp
	Serialize/GASSSerialize.cpp
	Serialize/tinyxml2.cpp
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
	Math/GASSMat4.h
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
	Math/GASSVec2.h
	Math/GASSVec3.h
	Math/GASSVec4.h
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
	Serialize/GASSISerialize.h
	Serialize/GASSIXMLSerialize.h
	Serialize/GASSSerialize.h
	Serialize/GASSSTDSerialize.h
	Serialize/tinyxml2.h
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
	${GASS_CONFIG_HEADER}
	${GASS_VERSION_HEADER}
)
set(PLATFORM_DEPS "")
if(WIN32)
   #Add dependency for timer
   set(PLATFORM_DEPS Winmm)
else()
   set(PLATFORM_DEPS ${CMAKE_DL_LIBS})
endif()

if(GASS_USE_BOOST)
	set(BOOST_DEPS Boost::filesystem Boost::thread)
endif()

gass_setup_lib(GASSCore
					BUILDTYPE ${GASS_BUILDTYPE}
					SOURCE_FILES ${CPP_FILES} 
					HEADER_FILES ${H_FILES}
					PUBLIC_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../" "${CMAKE_CURRENT_BINARY_DIR}"
					PUBLIC_DEPS ${BOOST_DEPS}
					PRIVATE_DEPS ${PLATFORM_DEPS}
					PRIVATE_DEFINITIONS GASS_CORE_EXPORTS ${GASS_COMMON_DEFINITIONS} )


