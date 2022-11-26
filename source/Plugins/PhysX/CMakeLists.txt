find_package(PhysX4 REQUIRED)

if(MSVC)
    gass_setup_plugin(GASSPluginPhysX PRIVATE_DEPS ${PHYSX4_ALL_MODULE_TARGETS})
else() 
    #Dont use target here to enable linker flags due to fact that we only get static physx to compile with vcpkg
    #could not find how to inject linker flags while creating import target in FindPhysX4.cmake 
    gass_setup_plugin(GASSPluginPhysX)
    target_compile_definitions(GASSPluginPhysX PUBLIC $<IF:$<CONFIG:Debug>,_DEBUG,NDEBUG>)
    target_include_directories(GASSPluginPhysX PRIVATE $<BUILD_INTERFACE:${PHYSX4_INCLUDE_DIR}>)
    foreach(PX_LIB ${PHYSX4_RELEASE_LIBS})
      target_link_libraries(GASSPluginPhysX PRIVATE "-Wl,--whole-archive ${PX_LIB} -Wl,--no-whole-archive")
    endforeach()
endif()

