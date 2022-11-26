#include "CustomTerrainManipulator.h"

using namespace osg;
using namespace osgGA;

CustomTerrainManipulator::CustomTerrainManipulator() : TerrainManipulator()
{
	//_thrown = false;
	//_distance = 1.0f;
	//_trackballSize = 0.8f;
}

CustomTerrainManipulator::~CustomTerrainManipulator()
{

}

bool CustomTerrainManipulator::performMovementRightMouseButton( const double eventTimeDelta, const double dx, const double dy )
{
	// zoom model
	return TerrainManipulator::performMovementLeftMouseButton( eventTimeDelta, dx, dy );
}

bool CustomTerrainManipulator::performMovementLeftMouseButton( const double /*eventTimeDelta*/, const double /*dx*/, const double /*dy*/ )
{
	return true;
}