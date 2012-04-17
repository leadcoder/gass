solution "SimDemo"
	configurations { "Debug", "Release" }

	dofile "SimDemo4.lua"

	--dofile "GASSSim4.lua"
	--dofile "GASSCore4.lua"

	--dofile "GASSPluginODE4.lua"
	--dofile "GASSPluginOgre4.lua"
	--dofile "GASSPluginOIS4.lua"

	
	function onclean()
		-- os.rmdir("bin")
	end