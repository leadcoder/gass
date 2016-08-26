find_package(MyGUI)
gass_create_dep_target(MyGUI "${MYGUI_INCLUDE_DIRS}" "${MYGUI_LIBRARIES}")
gass_setup_sim_sample(SampleSimGUI MyGUI)