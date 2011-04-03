#ifdef  WIN32
#	if defined( EDITOR_MODULE_EXPORTS )
#  		define EditorModuleExport __declspec( dllexport )
#	else
#		define EditorModuleExport __declspec( dllimport )
#	endif
#else 
#	define EditorModuleExport
#endif

