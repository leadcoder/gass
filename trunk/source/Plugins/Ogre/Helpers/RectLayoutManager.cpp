

#include <Ogre.h>
#include <OgrePrerequisites.h>
#include <OgreFontManager.h>
#include <OgreTextAreaOverlayElement.h>
#include "ObjectTextDisplay.h"

namespace GASS
{
	void ObjectTextDisplay::update()  
	{
		if (!m_enabled)
			return;

		// get the projection of the object's AABB into screen space
		const Ogre::AxisAlignedBox& bbox = m_p->getWorldBoundingBox(true);
		Ogre::Matrix4 mat = m_c->getViewMatrix();

		const Ogre::Vector3* corners = bbox.getAllCorners();

		float min_x = 1.0f, max_x = 0.0f, min_y = 1.0f, max_y = 0.0f;

		// expand the screen-space bounding-box so that it completely encloses 
		// the object's AABB
		for (int i=0; i<8; i++) {
			Ogre::Vector3 corner = corners[i];

			// multiply the AABB corner vertex by the view matrix to 
			// get a camera-space vertex
			corner = mat * corner;

			// make 2D relative/normalized coords from the view-space vertex
			// by dividing out the Z (depth) factor -- this is an approximation
			float x = corner.x / corner.z + 0.5;
			float y = corner.y / corner.z + 0.5;

			if (x < min_x) 
				min_x = x;

			if (x > max_x) 
				max_x = x;

			if (y < min_y) 
				min_y = y;

			if (y > max_y) 
				max_y = y;
		}
		// we now have relative screen-space coords for the object's bounding box; here
		// we need to center the text above the BB on the top edge. The line that defines
		// this top edge is (min_x, min_y) to (max_x, min_y)

		//m_pContainer->setPosition(min_x, min_y);
		m_pContainer->setPosition(1-max_x, min_y);  // Edited by alberts: This code works for me
		m_pContainer->setDimensions(max_x - min_x, 0.1); // 0.1, just "because"
	}
}