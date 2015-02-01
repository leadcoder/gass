rem ECHO Copy Ogre dll files
IF "%1" == "Debug" (
	::copying 
 	copy "%2\debug\OgreMain_d.dll" "%3"
	copy "%2\debug\OgreTerrain_d.dll" "%3"
	copy "%2\debug\OgrePaging_d.dll" "%3"
	copy "%2\debug\OgreOverlay_d.dll" "%3"
	copy "%2\debug\Plugin_CgProgramManager_d.dll" "%3"
	copy "%2\debug\Plugin_ParticleFX_d.dll" "%3"
	copy "%2\debug\Plugin_OctreeSceneManager_d.dll" "%3"
	copy "%2\debug\RenderSystem_Direct3D9_d.dll" "%3"
	copy "%2\debug\RenderSystem_GL_d.dll" "%3"
	copy "%2\debug\cg.dll" "%3"
)
IF "%1" == "Release" (
	::copying 
 	copy "%2\Release\OgreMain.dll" "%3"
	copy "%2\Release\OgreTerrain.dll" "%3"
	copy "%2\Release\OgrePaging.dll" "%3"
	copy "%2\Release\OgreOverlay.dll" "%3"
	copy "%2\Release\Plugin_CgProgramManager.dll" "%3"
	copy "%2\Release\Plugin_ParticleFX.dll" "%3"
	copy "%2\Release\Plugin_OctreeSceneManager.dll" "%3"
	copy "%2\Release\RenderSystem_Direct3D9.dll" "%3"
	copy "%2\Release\RenderSystem_GL.dll" "%3"
	copy "%2\Release\cg.dll" "%3"
)

pause
