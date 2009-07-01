solution "GASSSim"
	configurations { "Debug", "Release" }

dofile "GASSCore4.lua"
dofile "GASSSim4.lua"
dofile "GASSPluginOgre4.lua"
dofile "GASSPluginODE4.lua"
dofile "GASSPluginOIS4.lua"
dofile "GASSPluginPagedGeometry4.lua"
dofile "GASSPluginOpenAL4.lua"
--[[newoption {
   trigger     = "gfxapi",
   value       = "API",
   description = "Choose a particular 3D API for rendering",
   allowed = {
      { "opengl",    "OpenGL" },
      { "direct3d",  "Direct3D (Windows only)" },
      { "software",  "Software Renderer" }
   }
}
  ]]--
