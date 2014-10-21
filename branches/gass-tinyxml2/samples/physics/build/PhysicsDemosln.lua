solution "PhysicsDemo"
	configurations { "Debug", "Release" }

	dofile "PhysicsDemo.lua"

	function onclean()
		-- os.rmdir("bin")
	end