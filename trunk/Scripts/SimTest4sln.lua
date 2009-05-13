solution "SimTest"
	configurations { "Debug", "Release" }

	dofile "SimTest4.lua"

	dofile "GASSSim4.lua"
	dofile "GASSCore4.lua"

	dofile "GASSPluginODE4.lua"
	dofile "GASSPluginOgre4.lua"
	dofile "GASSPluginOIS4.lua"

	
	function onclean()
		-- os.rmdir("bin")
	end