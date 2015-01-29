if(EXISTS "${DEPENDENCIES_DIR}/OpenSceneGraph-v3.2.1")
	set(OSG_DIR  "${DEPENDENCIES_DIR}/OpenSceneGraph-v3.2.1" CACHE PATH "OSG folder")
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

			

set(OSG_PLUGINS_DIR_NAME osgPlugins-3.2.1)
FILE(GLOB OSG_PLUGINS "${OSG_PLUGINS_DIR}/*.dll")
set(OSG_PLUGINS_DIR	${OSG_BIN_DIR}/${OSG_PLUGINS_DIR_NAME})
set(OSG_PLUGINS_FILES_DEBUG 
	${OSG_PLUGINS_DIR}/osgdb_3dcd.dll
	${OSG_PLUGINS_DIR}/osgdb_3dsd.dll
	${OSG_PLUGINS_DIR}/osgdb_acd.dll
	${OSG_PLUGINS_DIR}/osgdb_bmpd.dll
	${OSG_PLUGINS_DIR}/osgdb_bspd.dll
	${OSG_PLUGINS_DIR}/osgdb_bvhd.dll
	${OSG_PLUGINS_DIR}/osgdb_cfgd.dll
	
	${OSG_PLUGINS_DIR}/osgdb_ddsd.dll
	${OSG_PLUGINS_DIR}/osgdb_deprecated_osgd.dll
	${OSG_PLUGINS_DIR}/osgdb_deprecated_osganimationd.dll
	${OSG_PLUGINS_DIR}/osgdb_deprecated_osgfxd.dll
	${OSG_PLUGINS_DIR}/osgdb_deprecated_osgparticled.dll
	${OSG_PLUGINS_DIR}/osgdb_deprecated_osgshadowd.dll
	${OSG_PLUGINS_DIR}/osgdb_deprecated_osgsimd.dll
	${OSG_PLUGINS_DIR}/osgdb_deprecated_osgterraind.dll
	${OSG_PLUGINS_DIR}/osgdb_deprecated_osgtextd.dll
	${OSG_PLUGINS_DIR}/osgdb_deprecated_osgviewerd.dll
	${OSG_PLUGINS_DIR}/osgdb_deprecated_osgvolumed.dll
	${OSG_PLUGINS_DIR}/osgdb_deprecated_osgwidgetd.dll
	${OSG_PLUGINS_DIR}/osgdb_dotd.dll
	${OSG_PLUGINS_DIR}/osgdb_dwd.dll
	${OSG_PLUGINS_DIR}/osgdb_dxfd.dll
	${OSG_PLUGINS_DIR}/osgdb_glsld.dll
	${OSG_PLUGINS_DIR}/osgdb_hdrd.dll
	${OSG_PLUGINS_DIR}/osgdb_ived.dll
	${OSG_PLUGINS_DIR}/osgdb_ktxd.dll
	${OSG_PLUGINS_DIR}/osgdb_logod.dll
	${OSG_PLUGINS_DIR}/osgdb_lwod.dll
	${OSG_PLUGINS_DIR}/osgdb_lwsd.dll
	${OSG_PLUGINS_DIR}/osgdb_md2d.dll
	${OSG_PLUGINS_DIR}/osgdb_mdld.dll
	${OSG_PLUGINS_DIR}/osgdb_normalsd.dll
	${OSG_PLUGINS_DIR}/osgdb_objd.dll
	${OSG_PLUGINS_DIR}/osgdb_openflightd.dll
	${OSG_PLUGINS_DIR}/osgdb_oscd.dll
	${OSG_PLUGINS_DIR}/osgdb_osgd.dll
	${OSG_PLUGINS_DIR}/osgdb_osgad.dll
	${OSG_PLUGINS_DIR}/osgdb_osgshadowd.dll
	${OSG_PLUGINS_DIR}/osgdb_osgterraind.dll
	${OSG_PLUGINS_DIR}/osgdb_osgtgzd.dll
	${OSG_PLUGINS_DIR}/osgdb_osgviewerd.dll
	${OSG_PLUGINS_DIR}/osgdb_p3dd.dll
	${OSG_PLUGINS_DIR}/osgdb_picd.dll
	${OSG_PLUGINS_DIR}/osgdb_plyd.dll
	${OSG_PLUGINS_DIR}/osgdb_pnmd.dll
	${OSG_PLUGINS_DIR}/osgdb_povd.dll
	${OSG_PLUGINS_DIR}/osgdb_pvrd.dll
	${OSG_PLUGINS_DIR}/osgdb_revisionsd.dll
	${OSG_PLUGINS_DIR}/osgdb_rgbd.dll
	${OSG_PLUGINS_DIR}/osgdb_rotd.dll
	${OSG_PLUGINS_DIR}/osgdb_scaled.dll
	${OSG_PLUGINS_DIR}/osgdb_serializers_osganimation.dll
	${OSG_PLUGINS_DIR}/osgdb_serializers_osgd.dll
	${OSG_PLUGINS_DIR}/osgdb_serializers_osgfxd.dll
	${OSG_PLUGINS_DIR}/osgdb_serializers_osggad.dll
	${OSG_PLUGINS_DIR}/osgdb_serializers_osgmanipulatord.dll
	${OSG_PLUGINS_DIR}/osgdb_serializers_osgparticled.dll
	${OSG_PLUGINS_DIR}/osgdb_serializers_osgshadowd.dll
	${OSG_PLUGINS_DIR}/osgdb_serializers_osgsimd.dll
	${OSG_PLUGINS_DIR}/osgdb_serializers_osgterraind.dll
	${OSG_PLUGINS_DIR}/osgdb_serializers_osgtextd.dll
	${OSG_PLUGINS_DIR}/osgdb_serializers_osgviewerd.dll
	${OSG_PLUGINS_DIR}/osgdb_serializers_osgvolumed.dll
	${OSG_PLUGINS_DIR}/osgdb_shpd.dll
	${OSG_PLUGINS_DIR}/osgdb_stld.dll
	${OSG_PLUGINS_DIR}/osgdb_tgad.dll
	${OSG_PLUGINS_DIR}/osgdb_tgzd.dll
	${OSG_PLUGINS_DIR}/osgdb_transd.dll
	${OSG_PLUGINS_DIR}/osgdb_trkd.dll
	${OSG_PLUGINS_DIR}/osgdb_txfd.dll
	${OSG_PLUGINS_DIR}/osgdb_txpd.dll
	${OSG_PLUGINS_DIR}/osgdb_vtfd.dll
	${OSG_PLUGINS_DIR}/osgdb_xd.dll
)

set(OSG_PLUGINS_FILES_RELEASE 
	${OSG_PLUGINS_DIR}/osgdb_3dc.dll
	${OSG_PLUGINS_DIR}/osgdb_3ds.dll
	${OSG_PLUGINS_DIR}/osgdb_ac.dll
	${OSG_PLUGINS_DIR}/osgdb_bmp.dll
	${OSG_PLUGINS_DIR}/osgdb_bsp.dll
	${OSG_PLUGINS_DIR}/osgdb_bvh.dll
	${OSG_PLUGINS_DIR}/osgdb_cfg.dll
	${OSG_PLUGINS_DIR}/osgdb_dds.dll
	${OSG_PLUGINS_DIR}/osgdb_deprecated_osg.dll
	${OSG_PLUGINS_DIR}/osgdb_deprecated_osganimation.dll
	${OSG_PLUGINS_DIR}/osgdb_deprecated_osgfx.dll
	${OSG_PLUGINS_DIR}/osgdb_deprecated_osgparticle.dll
	${OSG_PLUGINS_DIR}/osgdb_deprecated_osgshadow.dll
	${OSG_PLUGINS_DIR}/osgdb_deprecated_osgsim.dll
	${OSG_PLUGINS_DIR}/osgdb_deprecated_osgterrain.dll
	${OSG_PLUGINS_DIR}/osgdb_deprecated_osgtext.dll
	${OSG_PLUGINS_DIR}/osgdb_deprecated_osgviewer.dll
	${OSG_PLUGINS_DIR}/osgdb_deprecated_osgvolume.dll
	${OSG_PLUGINS_DIR}/osgdb_deprecated_osgwidget.dll
	${OSG_PLUGINS_DIR}/osgdb_dot.dll
	${OSG_PLUGINS_DIR}/osgdb_dw.dll
	${OSG_PLUGINS_DIR}/osgdb_dxf.dll
	${OSG_PLUGINS_DIR}/osgdb_glsl.dll
	${OSG_PLUGINS_DIR}/osgdb_hdr.dll
	${OSG_PLUGINS_DIR}/osgdb_ive.dll
	${OSG_PLUGINS_DIR}/osgdb_ktx.dll
	${OSG_PLUGINS_DIR}/osgdb_logo.dll
	${OSG_PLUGINS_DIR}/osgdb_lwo.dll
	${OSG_PLUGINS_DIR}/osgdb_lws.dll
	${OSG_PLUGINS_DIR}/osgdb_md2.dll
	${OSG_PLUGINS_DIR}/osgdb_mdl.dll
	${OSG_PLUGINS_DIR}/osgdb_normals.dll
	${OSG_PLUGINS_DIR}/osgdb_obj.dll
	${OSG_PLUGINS_DIR}/osgdb_openflight.dll
	${OSG_PLUGINS_DIR}/osgdb_osc.dll
	${OSG_PLUGINS_DIR}/osgdb_osg.dll
	${OSG_PLUGINS_DIR}/osgdb_osga.dll
	${OSG_PLUGINS_DIR}/osgdb_osgshadow.dll
	${OSG_PLUGINS_DIR}/osgdb_osgterrain.dll
	${OSG_PLUGINS_DIR}/osgdb_osgtgz.dll
	${OSG_PLUGINS_DIR}/osgdb_osgviewer.dll
	${OSG_PLUGINS_DIR}/osgdb_p3d.dll
	${OSG_PLUGINS_DIR}/osgdb_pic.dll
	${OSG_PLUGINS_DIR}/osgdb_ply.dll
	${OSG_PLUGINS_DIR}/osgdb_pnm.dll
	${OSG_PLUGINS_DIR}/osgdb_pov.dll
	${OSG_PLUGINS_DIR}/osgdb_pvr.dll
	${OSG_PLUGINS_DIR}/osgdb_revisions.dll
	${OSG_PLUGINS_DIR}/osgdb_rgb.dll
	${OSG_PLUGINS_DIR}/osgdb_rot.dll
	${OSG_PLUGINS_DIR}/osgdb_scale.dll
	${OSG_PLUGINS_DIR}/osgdb_serializers_osg.dll
	${OSG_PLUGINS_DIR}/osgdb_serializers_osganimationd.dll
	${OSG_PLUGINS_DIR}/osgdb_serializers_osgfx.dll
	${OSG_PLUGINS_DIR}/osgdb_serializers_osgga.dll
	${OSG_PLUGINS_DIR}/osgdb_serializers_osgmanipulator.dll
	${OSG_PLUGINS_DIR}/osgdb_serializers_osgparticle.dll
	${OSG_PLUGINS_DIR}/osgdb_serializers_osgshadow.dll
	${OSG_PLUGINS_DIR}/osgdb_serializers_osgsim.dll
	${OSG_PLUGINS_DIR}/osgdb_serializers_osgterrain.dll
	${OSG_PLUGINS_DIR}/osgdb_serializers_osgtext.dll
	${OSG_PLUGINS_DIR}/osgdb_serializers_osgviewer.dll
	${OSG_PLUGINS_DIR}/osgdb_serializers_osgvolume.dll
	${OSG_PLUGINS_DIR}/osgdb_shp.dll
	${OSG_PLUGINS_DIR}/osgdb_stl.dll
	${OSG_PLUGINS_DIR}/osgdb_tga.dll
	${OSG_PLUGINS_DIR}/osgdb_tgz.dll
	${OSG_PLUGINS_DIR}/osgdb_trans.dll
	${OSG_PLUGINS_DIR}/osgdb_trk.dll
	${OSG_PLUGINS_DIR}/osgdb_txf.dll
	${OSG_PLUGINS_DIR}/osgdb_txp.dll
	${OSG_PLUGINS_DIR}/osgdb_vtf.dll
	${OSG_PLUGINS_DIR}/osgdb_x.dll
)


set(OSG_BIN_FILES_DEBUG ${OSG_BIN_DIR}/ot20-OpenThreadsd.dll
${OSG_BIN_DIR}/zlibd.dll
${OSG_BIN_DIR}/osg100-osgd.dll
${OSG_BIN_DIR}/osg100-osgDBd.dll
${OSG_BIN_DIR}/osg100-osgGAd.dll
${OSG_BIN_DIR}/osg100-osgViewerd.dll
${OSG_BIN_DIR}/osg100-osgTextd.dll
${OSG_BIN_DIR}/osg100-osgUtild.dll
${OSG_BIN_DIR}/osg100-osgSimd.dll
${OSG_BIN_DIR}/osg100-osgFXd.dll
${OSG_BIN_DIR}/osg100-osgTerraind.dll
${OSG_BIN_DIR}/osg100-osgShadowd.dll
${OSG_BIN_DIR}/osg100-osgAnimationd.dll
${OSG_BIN_DIR}/osg100-osgManipulatord.dll
${OSG_BIN_DIR}/osg100-osgParticled.dll
${OSG_BIN_DIR}/osg100-osgVolumed.dll
)

set(OSG_BIN_FILES_RELEASE ${OSG_BIN_DIR}/ot20-OpenThreads.dll
${OSG_BIN_DIR}/zlib.dll
${OSG_BIN_DIR}/osg100-osg.dll
${OSG_BIN_DIR}/osg100-osgDB.dll
${OSG_BIN_DIR}/osg100-osgGA.dll
${OSG_BIN_DIR}/osg100-osgViewer.dll
${OSG_BIN_DIR}/osg100-osgText.dll
${OSG_BIN_DIR}/osg100-osgUtil.dll
${OSG_BIN_DIR}/osg100-osgSim.dll
${OSG_BIN_DIR}/osg100-osgFX.dll
${OSG_BIN_DIR}/osg100-osgTerrain.dll
${OSG_BIN_DIR}/osg100-osgShadow.dll
${OSG_BIN_DIR}/osg100-osgAnimation.dll
${OSG_BIN_DIR}/osg100-osgManipulator.dll
${OSG_BIN_DIR}/osg100-osgParticle.dll
${OSG_BIN_DIR}/osg100-osgVolume.dll
)

#set(OSG_LIB_DIR ${OSG_BIN_DIR}/lib)
#set(OSG_INC_DIR ${OSG_BIN_DIR}/include)
