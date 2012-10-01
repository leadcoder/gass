#ifdef  WIN32
#	if defined( EDITOR_MODULE_EXPORTS )
#  		define EditorModuleExport __declspec( dllexport )
#	else
#		define EditorModuleExport __declspec( dllimport )
#	endif
#else 
#	define EditorModuleExport
#endif

//predefined tool names
#define TID_MOVE "MoveTool"
#define TID_ROTATE "RotateTool"
#define TID_SELECT "SelectTool"
#define TID_TERRAIN_DEFORM "TerrainDeformTool"
#define TID_VERTICAL_MOVE "VerticalMoveTool"
#define TID_PAINT "PaintTool"
#define TID_GOTO_POS "GoToPositionTool"
#define TID_EDIT_POS "EditPositionTool"
#define TID_MEASUREMENT "MeasurementTool"