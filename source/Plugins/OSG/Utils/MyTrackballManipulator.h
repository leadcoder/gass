/* -*-c++-*- OpenSceneGraph - Copyright (C) 1998-2006 Robert Osfield
 *
 * This library is open source and may be redistributed and/or modified under
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * OpenSceneGraph Public License for more details.
*/

#pragma once

#include "Plugins/OSG/OSGCommon.h"

namespace osgGA {

	class MyTrackballManipulator : public OrbitManipulator
	{
	public:
		MyTrackballManipulator();

		const char* className() const override { return "Trackball"; }

		/** set the position of the matrix manipulator using a 4x4 Matrix.*/
		void setByMatrix(const osg::Matrixd& matrix) override;

		/** set the position of the matrix manipulator using a 4x4 Matrix.*/
		void setByInverseMatrix(const osg::Matrixd& matrix) override { setByMatrix(osg::Matrixd::inverse(matrix)); }

		/** get the position of the manipulator as 4x4 Matrix.*/
		osg::Matrixd getMatrix() const override;

		/** get the position of the manipulator as a inverse matrix of the manipulator, typically used as a model view matrix.*/
		osg::Matrixd getInverseMatrix() const override;

		/** Get the FusionDistanceMode. Used by SceneView for setting up stereo convergence.*/
		osgUtil::SceneView::FusionDistanceMode getFusionDistanceMode() const override { return osgUtil::SceneView::USE_FUSION_DISTANCE_VALUE; }

		/** Get the FusionDistanceValue. Used by SceneView for setting up stereo convergence.*/
		float getFusionDistanceValue() const override { return static_cast<float>(_distance); }

		/** Attach a node to the manipulator.
			Automatically detaches previously attached node.
			setNode(NULL) detaches previously nodes.
			Is ignored by manipulators which do not require a reference model.*/
		void setNode(osg::Node*) override;

		/** Return node if attached.*/
		const osg::Node* getNode() const override;

		/** Return node if attached.*/
		osg::Node* getNode() override;

		/** Move the camera to the default position.
			May be ignored by manipulators if home functionality is not appropriate.*/
		void home(const GUIEventAdapter& ea, GUIActionAdapter& us) override;
		void home(double) override;

		/** Start/restart the manipulator.*/
		void init(const GUIEventAdapter& ea, GUIActionAdapter& us) override;

		/** handle events, return true if handled, false otherwise.*/
		bool handle(const GUIEventAdapter& ea, GUIActionAdapter& us) override;

		/** Get the keyboard and mouse usage of this manipulator.*/
		void getUsage(osg::ApplicationUsage& usage) const override;


		/** set the minimum distance (as ratio) the eye point can be zoomed in towards the
			center before the center is pushed forward.*/
		void setMinimumZoomScale(double minimumZoomScale) { _minimumZoomScale = minimumZoomScale; }

		/** get the minimum distance (as ratio) the eye point can be zoomed in */
		double getMinimumZoomScale() const { return _minimumZoomScale; }

		/** Set the center of the trackball. */
		void setCenter(const osg::Vec3d& center) override { _center = center; }

		/** Get the center of the trackball. */
		const osg::Vec3d& getCenter() const { return _center; }

		/** Set the rotation of the trackball. */
		void setRotation(const osg::Quat& rotation) override { _rotation = rotation; }

		/** Get the rotation of the trackball. */
		const osg::Quat& getRotation() const { return _rotation; }

		/** Set the distance of the trackball. */
		void setDistance(double distance) override { _distance = distance; }

		/** Get the distance of the trackball. */
		double getDistance() const { return _distance; }

		/** Set the size of the trackball. */
		void setTrackballSize(float size);

		/** Get the size of the trackball. */
		float getTrackballSize() const { return _trackballSize; }
	protected:

		~MyTrackballManipulator() override;

		/** Reset the internal GUIEvent stack.*/
		void flushMouseEventStack();
		/** Add the current mouse GUIEvent to internal stack.*/
		void addMouseEvent(const GUIEventAdapter& ea);

		void computePosition(const osg::Vec3& eye, const osg::Vec3& lv, const osg::Vec3& up);

		/** For the give mouse movement calculate the movement of the camera.
			Return true is camera has moved and a redraw is required.*/
		bool calcMovement();

		void trackball(osg::Vec3& axis, float& angle, float p1x, float p1y, float p2x, float p2y);
		float tb_project_to_sphere(float r, float x, float y);


		/** Check the speed at which the mouse is moving.
			If speed is below a threshold then return false, otherwise return true.*/
		bool isMouseMoving();

		// Internal event stack comprising last two mouse events.
		osg::ref_ptr<const GUIEventAdapter> _ga_t1;
		osg::ref_ptr<const GUIEventAdapter> _ga_t0;

		osg::ref_ptr<osg::Node>       _node;

		double _modelScale;
		double _minimumZoomScale;

		bool _thrown;

		osg::Vec3d   _center;
		osg::Quat    _rotation;
		double       _distance;
		float        _trackballSize;

	};

}

