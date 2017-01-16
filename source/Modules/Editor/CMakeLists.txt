#gass_get_source_from_current_dir(CPP_FILES H_FILES)
#gass_setup_lib(GASSEditorModule
#					BUILDTYPE ${GASS_BUILDTYPE}
#					SOURCE_FILES ${CPP_FILES} 
#					HEADER_FILES ${H_FILES}
#					PUBLIC_DEPS GASSSim
#					PRIVATE_DEFINITIONS EDITOR_MODULE_EXPORTS ${GASS_COMMON_DEFINITIONS} )
gass_setup_module(GASSEditorModule)

