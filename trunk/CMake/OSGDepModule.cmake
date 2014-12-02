
if(EXISTS "${GASS_DEPENDENCIES_DIR}/OpenSceneGraph-v3.2.1")
	set(OSG_DIR  "${GASS_DEPENDENCIES_DIR}/OpenSceneGraph-v3.2.1" CACHE PATH "OSG folder")
else()
	set(OSG_DIR $ENV{OSG_HOME} CACHE PATH "OSG folder")
endif()

set(OSG_BIN_DIR ${OSG_DIR}/bin)
set(OSG_INC_DIR ${OSG_DIR}/include)
set(OSG_LIB_DIR ${OSG_DIR}/lib)

set(OSG_LINK_LIBRARIES debug OpenThreadsd
			debug osgd
			debug osgDBd
			debug osgUtild
			debug osgGAd
			debug osgViewerd
			debug osgTextd
			debug osgShadowd
			debug osgSimd
			debug osgTerraind
			optimized OpenThreads
			optimized osg
			optimized osgDB
			optimized osgUtil
			optimized osgGA
			optimized osgViewer
			optimized osgText
			optimized osgShadow
			optimized osgSim
			optimized osgTerrain
			opengl32
			glu32)

			

set(OSG_PLUGINS_DIR_NAME osgPlugins-3.0.1)
set(OSG_PLUGINS_DIR	${OSG_BIN_DIR}/${OSG_PLUGINS_DIR_NAME})

FILE(GLOB OSG_PLUGINS "${OSG_PLUGINS_DIR}/*.dll")

set(OSG_BIN_FILES_DEBUG ${OSG_BIN_DIR}/ot12-OpenThreadsd.dll
${OSG_BIN_DIR}/zlib1d.dll
${OSG_BIN_DIR}/osg80-osgd.dll
${OSG_BIN_DIR}/osg80-osgDBd.dll
${OSG_BIN_DIR}/osg80-osgGAd.dll
${OSG_BIN_DIR}/osg80-osgViewerd.dll
${OSG_BIN_DIR}/osg80-osgTextd.dll
${OSG_BIN_DIR}/osg80-osgUtild.dll
${OSG_BIN_DIR}/osg80-osgSimd.dll
${OSG_BIN_DIR}/osg80-osgFXd.dll
${OSG_BIN_DIR}/osg80-osgTerraind.dll
${OSG_BIN_DIR}/osg80-osgShadowd.dll
${OSG_BIN_DIR}/osg80-osgAnimationd.dll
${OSG_BIN_DIR}/osg80-osgManipulatord.dll
${OSG_BIN_DIR}/osg80-osgParticled.dll
${OSG_BIN_DIR}/osg80-osgVolumed.dll
)

set(OSG_BIN_FILES_RELEASE ${OSG_BIN_DIR}/ot12-OpenThreads.dll
${OSG_BIN_DIR}/zlib1.dll
${OSG_BIN_DIR}/osg80-osg.dll
${OSG_BIN_DIR}/osg80-osgDB.dll
${OSG_BIN_DIR}/osg80-osgGA.dll
${OSG_BIN_DIR}/osg80-osgViewer.dll
${OSG_BIN_DIR}/osg80-osgText.dll
${OSG_BIN_DIR}/osg80-osgUtil.dll
${OSG_BIN_DIR}/osg80-osgSim.dll
${OSG_BIN_DIR}/osg80-osgFX.dll
${OSG_BIN_DIR}/osg80-osgTerrain.dll
${OSG_BIN_DIR}/osg80-osgShadow.dll
${OSG_BIN_DIR}/osg80-osgAnimation.dll
${OSG_BIN_DIR}/osg80-osgManipulator.dll
${OSG_BIN_DIR}/osg80-osgParticle.dll
${OSG_BIN_DIR}/osg80-osgVolume.dll
)

#set(OSG_LIB_DIR ${OSG_BIN_DIR}/lib)
#set(OSG_INC_DIR ${OSG_BIN_DIR}/include)
