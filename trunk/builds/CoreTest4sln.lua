solution "CoreTest"
	configurations { "Debug", "Release" }

	dofile "CoreTest4.lua"
	dofile "GASSCore4.lua"
--	dofile "GASS4.lua"
--	dofile "PluginOgre4.lua"
	
	function onclean()
		-- os.rmdir("bin")
	end