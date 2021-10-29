/****************************************************************************
* This file is part of GASS.                                                *
* See https://github.com/leadcoder/gass                                     *
*                                                                           *
* Copyright (c) 2008-2016 GASS team. See Contributors.txt for details.      *
*                                                                           *
* GASS is free software: you can redistribute it and/or modify              *
* it under the terms of the GNU Lesser General Public License as published  *
* by the Free Software Foundation, either version 3 of the License, or      *
* (at your option) any later version.                                       *
*                                                                           *
* GASS is distributed in the hope that it will be useful,                   *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU Lesser General Public License for more details.                       *
*                                                                           *
* You should have received a copy of the GNU Lesser General Public License  *
* along with GASS. If not, see <http://www.gnu.org/licenses/>.              *
*****************************************************************************/

#pragma once

#include "Sim/GASSCommon.h"
#include "Sim/GASSResourceHandle.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSGeometryFlags.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSQuaternion.h"
#include "Core/Utils/GASSColorRGBA.h"



/**
This file hold messages that can be posted to
SceneObjects. SceneObjectMessages are used to
enable communication between components hold by
a SceneObject. The user is free to extend this
set of messages but this file hold the "core"
graphic messages that all components share.
If a scene object message is needed and it's
of common interest it can be a candidate for
this file.
*/

namespace GASS
{
	class SimSceneManager;
	class ISceneManager;
	class IGeometryComponent;
	class ILocationComponent;
	class GraphicsMesh;
	typedef GASS_SHARED_PTR<GraphicsMesh> GraphicsMeshPtr;
	typedef GASS_SHARED_PTR<ISceneManager> SceneManagerPtr;
	typedef GASS_SHARED_PTR<IGeometryComponent> GeometryComponentPtr;
	typedef GASS_SHARED_PTR<ILocationComponent> LocationComponentPtr;

	//*********************************************************
	// ALL MESSAGES IN THIS SECTION CAN BE POSTED BY USER
	//*********************************************************

	/*class BaseRotationRequest : public SceneObjectRequestMessage
	{
	public:
		BaseRotationRequest(const Quaternion &rot, SenderID sender_id = -1, double delay = 0) :
			SceneObjectRequestMessage(sender_id, delay), m_Rotation(rot)
		{

		}
		Quaternion GetRotation() const { return m_Rotation; }
	private:
		Quaternion m_Rotation;
	};
	typedef GASS_SHARED_PTR<BaseRotationRequest > BaseRotationRequestPtr;
	*/

	//Keep this to dont break compilation
	enum TerrainLayer
	{
		TL_0,
		TL_1,
		TL_2,
		TL_3,
		TL_4
	};

	//*********************************************************
	// ALL MESSAGES BELOW SHOULD ONLY BE POSTED GASS INTERNALS
	//*********************************************************
	
	class LocationLoadedEvent : public SceneObjectEventMessage
	{
	public:
		LocationLoadedEvent(LocationComponentPtr location,SenderID sender_id = -1, double delay= 0) :
		  SceneObjectEventMessage(sender_id , delay), m_Location(location){}
		  LocationComponentPtr GetLocation() const {return m_Location;}
	private:
		LocationComponentPtr m_Location;
	};
	typedef GASS_SHARED_PTR<LocationLoadedEvent> LocationLoadedEventPtr;


	/** Message sent by scene node when scene node is moved
	*/

	class TransformationChangedEvent : public SceneObjectEventMessage
	{
	public:
		/**
			Constructor
			@param pos Position in world coordiante space
			@param rot	Rotation in world coordiante space
			@param scale Scale in world coordiante space
		*/
		TransformationChangedEvent(const Vec3  &pos, const Quaternion &rot, const Vec3  &scale,SenderID sender_id = -1, double delay= 0) :
		  SceneObjectEventMessage(sender_id , delay), m_Position(pos), m_Rotation(rot), m_Scale(scale){}
		  Vec3 GetPosition() const {return m_Position;}
		  Quaternion  GetRotation() const {return m_Rotation;}
		  Vec3 GetScale() const {return m_Scale;}
	private:
		Vec3 m_Position;
		Quaternion m_Rotation;
		Vec3 m_Scale;
	};
	typedef GASS_SHARED_PTR<TransformationChangedEvent> TransformationChangedEventPtr;

	/**
		Messages sent by geometry components when its geometry is changed. 
	*/
	class GeometryChangedEvent : public SceneObjectEventMessage
	{
	public:
		GeometryChangedEvent(GeometryComponentPtr geom, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectEventMessage(sender_id , delay), m_Geometry(geom){}
		  GeometryComponentPtr  GetGeometry() const {return m_Geometry;}
	private:
		GeometryComponentPtr m_Geometry;
	};
	typedef GASS_SHARED_PTR<GeometryChangedEvent> GeometryChangedEventPtr;


	/**
	Event generated by geometry source,
	Typically used collision system to reflect 
	collision class for this object.
	*/

	class GeometryFlagsChangedEvent : public SceneObjectEventMessage
	{
	public:
		GeometryFlagsChangedEvent(GeometryFlags flags, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectEventMessage(sender_id , delay), m_Flags(flags)
		  {

		  }
		  GeometryFlags GetGeometryFlags() const {return m_Flags;}
	private:
		GeometryFlags m_Flags;
	};
	typedef GASS_SHARED_PTR<GeometryFlagsChangedEvent> GeometryFlagsChangedEventPtr;
}
