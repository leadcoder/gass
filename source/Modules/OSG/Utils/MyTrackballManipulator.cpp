#include "MyTrackballManipulator.h"
#include <osg/Quat>
#include <osg/BoundsChecking>

using namespace osg;
using namespace osgGA;

MyTrackballManipulator::MyTrackballManipulator()
{
	_modelScale = 0.01f;
	_minimumZoomScale = 0.05f;
	_thrown = false;
	_distance = 1.0f;
	_trackballSize = 0.8f;
}


MyTrackballManipulator::~MyTrackballManipulator()
{
}


void MyTrackballManipulator::setNode(osg::Node* node)
{
	_node = node;
	if (_node.get())
	{
		const osg::BoundingSphere& boundingSphere=_node->getBound();
		_modelScale = boundingSphere._radius;
	}
	if (getAutoComputeHomePosition()) computeHomePosition();
}


const osg::Node* MyTrackballManipulator::getNode() const
{
	return _node.get();
}

osg::Node* MyTrackballManipulator::getNode()
{
	return _node.get();
}

void MyTrackballManipulator::home(double /*currentTime*/)
{
	if (getAutoComputeHomePosition()) computeHomePosition();
	computePosition(_homeEye, _homeCenter, _homeUp);
	_thrown = false;
}

void MyTrackballManipulator::home(const GUIEventAdapter& ea ,GUIActionAdapter& us)
{
	home(ea.getTime());
	us.requestRedraw();
	us.requestContinuousUpdate(false);
}


void MyTrackballManipulator::init(const GUIEventAdapter& ,GUIActionAdapter& )
{
	flushMouseEventStack();
}

void MyTrackballManipulator::getUsage(osg::ApplicationUsage& usage) const
{
	usage.addKeyboardMouseBinding("Trackball: Space","Reset the viewing position to home");
	usage.addKeyboardMouseBinding("Trackball: +","When in stereo, increase the fusion distance");
	usage.addKeyboardMouseBinding("Trackball: -","When in stereo, reduce the fusion distance");
}

bool MyTrackballManipulator::handle(const GUIEventAdapter& ea,GUIActionAdapter& us)
{
	switch(ea.getEventType())
	{
	case(GUIEventAdapter::FRAME):
		if (_thrown)
		{
			if (calcMovement()) us.requestRedraw();
		}
		return false;
	default:
		break;
	}

	if (ea.getHandled()) return false;

	switch(ea.getEventType())
	{
	case(GUIEventAdapter::PUSH):
		{
			flushMouseEventStack();
			addMouseEvent(ea);
			if (calcMovement()) us.requestRedraw();
			us.requestContinuousUpdate(false);
			_thrown = false;
			return true;
		}

	case(GUIEventAdapter::RELEASE):
		{
			if (ea.getButtonMask()==0)
			{

				double timeSinceLastRecordEvent = _ga_t0.valid() ? (ea.getTime() - _ga_t0->getTime()) : DBL_MAX;
				if (timeSinceLastRecordEvent>0.02) flushMouseEventStack();

				if (isMouseMoving())
				{
					if (calcMovement())
					{
						us.requestRedraw();
						us.requestContinuousUpdate(true);
						_thrown = true;
					}
				}
				else
				{
					flushMouseEventStack();
					addMouseEvent(ea);
					if (calcMovement()) us.requestRedraw();
					us.requestContinuousUpdate(false);
					_thrown = false;
				}

			}
			else
			{
				flushMouseEventStack();
				addMouseEvent(ea);
				if (calcMovement()) us.requestRedraw();
				us.requestContinuousUpdate(false);
				_thrown = false;
			}
			return true;
		}

	case(GUIEventAdapter::DRAG):
		{
			addMouseEvent(ea);
			if (calcMovement()) us.requestRedraw();
			us.requestContinuousUpdate(false);
			_thrown = false;
			return true;
		}

	case(GUIEventAdapter::MOVE):
		{
			return false;
		}

	case(GUIEventAdapter::SCROLL):
		{

			float speed = 0.11f;
			osgGA::GUIEventAdapter::ScrollingMotion sm = ea.getScrollingMotion();
			switch( sm )
			{
				// mouse scroll up event
			case GUIEventAdapter::SCROLL_UP:
				{
					break;
				}

				// mouse scroll down event
			case GUIEventAdapter::SCROLL_DOWN:
				{
					speed = -speed;
					break;
				}
			case GUIEventAdapter::SCROLL_NONE:
			case GUIEventAdapter::SCROLL_RIGHT:
			case GUIEventAdapter::SCROLL_LEFT:
			case GUIEventAdapter::SCROLL_2D:
				break;
			}


			// zoom model.

			float fd = static_cast<float>(_distance);
			float scale = 1.0f+speed;
			if (fd*scale>_modelScale*_minimumZoomScale)
			{

				_distance *= scale;

			}
			else
			{

				// notify(DEBUG_INFO) << "Pushing forward"<<std::endl;
				// push the camera forward.
				scale = -fd;

				osg::Matrix rotation_matrix(_rotation);

				osg::Vec3 dv = (osg::Vec3(0.0f, 0.0f, -1.0f) * rotation_matrix)*(speed*scale);

				_center += dv;

			}
		}
		break;

	case(GUIEventAdapter::KEYDOWN):
		if (ea.getKey()== GUIEventAdapter::KEY_Space)
		{
			flushMouseEventStack();
			_thrown = false;
			home(ea,us);
			return true;
		}
		return false;
		break;
	case(GUIEventAdapter::FRAME):
		if (_thrown)
		{
			if (calcMovement()) us.requestRedraw();
		}
		return false;
	default:
		return false;
	}
	return false;
}


bool MyTrackballManipulator::isMouseMoving()
{
	if (_ga_t0.get()==nullptr || _ga_t1.get()==nullptr) return false;

	static const float velocity = 0.1f;

	float dx = _ga_t0->getXnormalized()-_ga_t1->getXnormalized();
	float dy = _ga_t0->getYnormalized()-_ga_t1->getYnormalized();
	float len = sqrtf(dx*dx+dy*dy);
	float dt = static_cast<float>(_ga_t0->getTime()-_ga_t1->getTime());

	return (len>dt*velocity);
}


void MyTrackballManipulator::flushMouseEventStack()
{
	_ga_t1 = nullptr;
	_ga_t0 = nullptr;
}


void MyTrackballManipulator::addMouseEvent(const GUIEventAdapter& ea)
{
	_ga_t1 = _ga_t0;
	_ga_t0 = &ea;
}

void MyTrackballManipulator::setByMatrix(const osg::Matrixd& matrix)
{
	_center = osg::Vec3d(0.0, 0.0, -_distance) *matrix;
	_rotation = matrix.getRotate();
}

osg::Matrixd MyTrackballManipulator::getMatrix() const
{
	return osg::Matrixd::translate(0.0,0.0,_distance)*osg::Matrixd::rotate(_rotation)*osg::Matrixd::translate(_center);
}

osg::Matrixd MyTrackballManipulator::getInverseMatrix() const
{
	return osg::Matrixd::translate(-_center)*osg::Matrixd::rotate(_rotation.inverse())*osg::Matrixd::translate(0.0,0.0,-_distance);
}

void MyTrackballManipulator::computePosition(const osg::Vec3& eye,const osg::Vec3& center,const osg::Vec3& up)
{

	osg::Vec3 lv(center-eye);

	osg::Vec3 f(lv);
	f.normalize();
	osg::Vec3 s(f^up);
	s.normalize();
	osg::Vec3 u(s^f);
	u.normalize();

	osg::Matrix rotation_matrix(s[0],     u[0],     -f[0],     0.0f,
		s[1],     u[1],     -f[1],     0.0f,
		s[2],     u[2],     -f[2],     0.0f,
		0.0f,     0.0f,     0.0f,      1.0f);

	_center = center;
	_distance = lv.length();
	_rotation = rotation_matrix.getRotate().inverse();
}


bool MyTrackballManipulator::calcMovement()
{
	// return if less then two events have been added.
	if (_ga_t0.get()==nullptr || _ga_t1.get()==nullptr) return false;

	float dx = _ga_t0->getXnormalized()-_ga_t1->getXnormalized();
	float dy = _ga_t0->getYnormalized()-_ga_t1->getYnormalized();

	float distance = sqrtf(dx*dx + dy*dy);
	// return if movement is too fast, indicating an error in event values or change in screen.
	if (distance>0.5)
	{
		return false;
	}

	// return if there is no movement.
	if (distance==0.0f)
	{
		return false;
	}

	unsigned int buttonMask = _ga_t1->getButtonMask();
	if (buttonMask==GUIEventAdapter::RIGHT_MOUSE_BUTTON)
	{

		// rotate camera.

		osg::Vec3 axis;
		float angle;

		float px0 = _ga_t0->getXnormalized();
		float py0 = _ga_t0->getYnormalized();

		float px1 = _ga_t1->getXnormalized();
		float py1 = _ga_t1->getYnormalized();


		trackball(axis,angle,px1,py1,px0,py0);

		osg::Quat new_rotate;
		new_rotate.makeRotate(angle,axis);

		_rotation = _rotation*new_rotate;

		return true;

	}
	else if (buttonMask==GUIEventAdapter::MIDDLE_MOUSE_BUTTON ||
		buttonMask==(GUIEventAdapter::LEFT_MOUSE_BUTTON|GUIEventAdapter::RIGHT_MOUSE_BUTTON))
	{

		// pan model.

		float scale = -0.3f*static_cast<float>(_distance);

		osg::Matrix rotation_matrix;
		rotation_matrix.makeRotate(_rotation);

		osg::Vec3 dv(dx*scale,dy*scale,0.0f);

		_center += dv*rotation_matrix;

		return true;

	}
	//else if (buttonMask==GUIEventAdapter::RIGHT_MOUSE_BUTTON)
	else if (buttonMask==GUIEventAdapter::MIDDLE_MOUSE_BUTTON)
	{


		return true;

	}

	return false;
}


/*
* This size should really be based on the distance from the center of
* rotation to the point on the object underneath the mouse.  That
* point would then track the mouse as closely as possible.  This is a
* simple example, though, so that is left as an Exercise for the
* Programmer.
*/
void MyTrackballManipulator::setTrackballSize(float size)
{
	_trackballSize = size;
	osg::clampBetweenRange(_trackballSize,0.1f,1.0f,"MyTrackballManipulator::setTrackballSize(float)");
}

/*
* Ok, simulate a track-ball.  Project the points onto the virtual
* trackball, then figure out the axis of rotation, which is the cross
* product of P1 P2 and O P1 (O is the center of the ball, 0,0,0)
* Note:  This is a deformed trackball-- is a trackball in the center,
* but is deformed into a hyperbolic sheet of rotation away from the
* center.  This particular function was chosen after trying out
* several variations.
*
* It is assumed that the arguments to this routine are in the range
* (-1.0 ... 1.0)
*/
void MyTrackballManipulator::trackball(osg::Vec3& axis,float& angle, float p1x, float p1y, float p2x, float p2y)
{
	/*
	* First, figure out z-coordinates for projection of P1 and P2 to
	* deformed sphere
	*/

	osg::Matrix rotation_matrix(_rotation);


	osg::Vec3 uv = osg::Vec3(0.0f,1.0f,0.0f)*rotation_matrix;
	osg::Vec3 sv = osg::Vec3(1.0f,0.0f,0.0f)*rotation_matrix;
	osg::Vec3 lv = osg::Vec3(0.0f,0.0f,-1.0f)*rotation_matrix;

	osg::Vec3 p1 = sv * p1x + uv * p1y - lv * tb_project_to_sphere(_trackballSize, p1x, p1y);
	osg::Vec3 p2 = sv * p2x + uv * p2y - lv * tb_project_to_sphere(_trackballSize, p2x, p2y);

	/*
	*  Now, we want the cross product of P1 and P2
	*/

	// Robert,
	//
	// This was the quick 'n' dirty  fix to get the trackball doing the right 
	// thing after fixing the Quat rotations to be right-handed.  You may want
	// to do something more elegant.
	//   axis = p1^p2;
	axis = p2^p1;
	axis.normalize();

	/*
	*  Figure out how much to rotate around that axis.
	*/
	float t = static_cast<float>((p2 - p1).length() / (2.0 * _trackballSize));

	/*
	* Avoid problems with out-of-control values...
	*/
	if (t > 1.0) t = 1.0;
	if (t < -1.0) t = -1.0;
	angle = inRadians(asin(t));

}


/*
* Project an x,y pair onto a sphere of radius r OR a hyperbolic sheet
* if we are away from the center of the sphere.
*/
float MyTrackballManipulator::tb_project_to_sphere(float r, float x, float y)
{
	float d, t, z;

	d = sqrt(x*x + y*y);
	/* Inside sphere */
	if (d < r * 0.70710678118654752440)
	{
		z = sqrt(r*r - d*d);
	}                            /* On hyperbola */
	else
	{
		t = static_cast<float>(r / 1.41421356237309504880);
		z = t*t / d;
	}
	return z;
}