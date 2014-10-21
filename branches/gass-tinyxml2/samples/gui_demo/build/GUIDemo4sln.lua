solution "GUIDemo"
	configurations { "Debug", "Release" }

	dofile "GUIDemo4.lua"

	function onclean()
		-- os.rmdir("bin")
	end