/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                 *
*                                                                           *
* Copyright (c) 2008-2009 GASS team. See Contributors.txt for details.      *
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
#include "Core/MessageSystem/GASSBaseMessage.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSQuaternion.h"
#include "Core/Utils/GASSColorRGB.h"



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
	typedef SPTR<GraphicsMesh> GraphicsMeshPtr;
	typedef SPTR<ISceneManager> SceneManagerPtr;
	typedef SPTR<IGeometryComponent> GeometryComponentPtr;
	typedef SPTR<ILocationComponent> LocationComponentPtr;

	//*********************************************************
	// ALL MESSAGES IN THIS SECTION CAN BE POSTED BY USER
	//*********************************************************

	/**
	Position (relative to parent) change is requested,
	Typically the location component respond to this message
	by moving the node in the scene graph. The location component
	is usually implemented in the graphic system and is used by GASS
	to transfer location information to a scene graph node.
	However, it's also possible to have other type of components
	that respond to position messages, for instance a 3d sound source
	probably also want to catch location information.
	*/

	class PositionRequest : public SceneObjectRequestMessage
	{
	public:

		PositionRequest(const Vec3 &pos, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage(sender_id , delay), m_Position(pos)
		  {

		  }
		  Vec3 GetPosition() const {return m_Position;}
	private:
		Vec3 m_Position;
	};
	typedef SPTR<PositionRequest> PositionRequestPtr;


	/**
	Rotation (relative to parent) change is requested,
	Typically the location component respond to this message
	by apply the rotation to the node in the scene graph.
	The location component is usually implemented in the
	graphic system and is used by GASS to transfer location
	information to a scene graph node.
	However, it's also possible to have other type of components
	that respond to rotation messages, for instance a light source
	probably also want to catch rotation information if its
	not attached to a scene node.
	*/

	class RotationRequest : public SceneObjectRequestMessage
	{
	public:
		RotationRequest(const Quaternion &rot, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage(sender_id , delay), m_Rotation(rot)
		  {

		  }
		  Quaternion GetRotation() const {return m_Rotation;}
	private:
		Quaternion m_Rotation;
	};
	typedef SPTR<RotationRequest> RotationRequestPtr;



	/**
	Position change is requested,
	Typically the location component respond to this message
	by moving the node in the scene graph. The location component
	is usually implemented in the graphic system and is used by GASS
	to transfer location information to a scene graph node.
	However, it's also possible to have other type of components
	that respond to position messages, for instance a sound source
	probably also want to catch location information.

	Note: In contrast to the PositionRequest this is a request for a
	absolute position change, if the location component that
	respond to this messages has a scene graph node that is
	attached to a parent node this position change will
	probably require more calculations due to the fact
	a new relative position also have to be calculated.
	*/


	class WorldPositionRequest : public SceneObjectRequestMessage
	{
	public:
		WorldPositionRequest(const Vec3 &pos, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage( sender_id , delay), m_Position(pos)
		  {

		  }
		  Vec3 GetPosition() const {return m_Position;}
	private:
		Vec3 m_Position;
	};
	typedef SPTR<WorldPositionRequest> WorldPositionRequestPtr;



	/**
	Rotation change is requested,
	Typically the location component respond to this message
	by apply the rotation to the node in the scene graph.
	The location component is usually implemented in the
	graphic system and is used by GASS to transfer location
	information to a scene graph node.
	However, it's also possible to have other type of components
	that respond to rotation messages, for instance a light source
	probably also want to catch rotation information if its
	not attached to a scene node.

	Note: In contrast to the RotationRequest this is a request for a
	absolute rotation change, if the location component that
	respond to this messages has a scene graph node that is
	attached to a parent node this rotation change will
	probably require more calculations due to the fact
	a new relative rotation also have to be calculated.
	*/

	class WorldRotationRequest : public SceneObjectRequestMessage
	{
	public:
		WorldRotationRequest(const Quaternion &rot, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage(sender_id , delay), m_Rotation(rot)
		  {

		  }
		  Quaternion GetRotation() const {return m_Rotation;}
	private:
		Quaternion m_Rotation;
	};
	typedef SPTR<WorldRotationRequest> WorldRotationRequestPtr;

	/**
	Scale (relative to parent) change requested.
	Typically the location component respond to this message
	by apply the scale to the node in the scene graph.
	The location component is usually implemented in the
	graphic system and is used by GASS to transfer location
	information to a scene graph node.
	Note that scaling scene nodes should be used by care,
	due to the fact that mesh normals can get out of
	hand and as a result the lightning will not look correct.
	Instead try to prescale your meshes before loading.
	*/
	class ScaleRequest : public SceneObjectRequestMessage
	{
	public:

		ScaleRequest (const Vec3 &scale, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage(sender_id , delay), m_Scale(scale)
		  {

		  }
		  Vec3 GetScale() const {return m_Scale;}
	private:
		Vec3 m_Scale;
	};
	typedef SPTR<ScaleRequest> ScaleRequestPtr;


	


	/**
	Message used to change visibility of scene nodes,
	Typically the location component respond to this message
	by hiding/unhiding the node in the graphic system scene graph.

	Note: that this messages only intended to hide the visual part
	of a object, sound, physics or any other components
	should not respond to this message. For instance, to disable
	the physics collision component instead use the
	CollisionSettingsRequest.
	*/

	class VisibilityRequest : public SceneObjectRequestMessage
	{
	public:
		VisibilityRequest(bool visible, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage(sender_id , delay), m_Visible(visible)
		  {

		  }
		  bool GetValue() const {return m_Visible;}
	private:
		bool m_Visible;
	};

	typedef SPTR<VisibilityRequest> VisibilityRequestPtr;


	class GeometryVisibilityRequest : public SceneObjectRequestMessage
	{
	public:
		GeometryVisibilityRequest(bool visible, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage(sender_id , delay), m_Visible(visible)
		  {

		  }
		  bool GetValue() const {return m_Visible;}
	private:
		bool m_Visible;
	};

	typedef SPTR<GeometryVisibilityRequest> GeometryVisibilityRequestPtr;



	/**
	Message use to alternate whether or not a gfx-scene node should be attached 
	to the first parent gfx-node
	*/
	class AttachToParentRequest : public SceneObjectRequestMessage
	{
	public:
		AttachToParentRequest(bool value,SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage( sender_id , delay), m_AttachToParent(value)
		  {

		  }
		  bool GetAttachToParent() const {return m_AttachToParent;}
	private:
		bool m_AttachToParent;
	};
	typedef SPTR<AttachToParentRequest> AttachToParentRequestPtr;


	
	/**
		Message used to modify camera settings
	*/
	class CameraParameterRequest : public SceneObjectRequestMessage
	{
	public:
		enum CameraParameterType
		{
			CAMERA_FOV,
			CAMERA_CLIP_DISTANCE,
			CAMERA_ORTHO_WIN_SIZE,
		};
	public:
		CameraParameterRequest(CameraParameterType paramter, float value1, float value2 = 0, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage(sender_id , delay), m_Value1(value1),m_Value2(value2), m_Parameter(paramter){}
		  float GetValue1()const {return m_Value1;}
		  float GetValue2()const {return m_Value2;}
		  CameraParameterType GetParameter()const {return m_Parameter;}
	private:
		float m_Value1;
		float m_Value2;
		CameraParameterType m_Parameter;
	};
	typedef SPTR<CameraParameterRequest> CameraParameterRequestPtr;


	/**
		Request camera to track scene object
	*/
	class CameraTrackObjectRequest : public SceneObjectRequestMessage
	{
	public:
		CameraTrackObjectRequest(SceneObjectPtr track_object, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage(sender_id , delay), m_Track(track_object){}
		  SceneObjectPtr GetTrackObject()const {return m_Track;}
	private:
		SceneObjectPtr m_Track;
	};
	typedef SPTR<CameraTrackObjectRequest> CameraTrackObjectRequestPtr;


	class BoundingInfoRequest : public SceneObjectRequestMessage
	{
	public:
		BoundingInfoRequest(bool bb_visible, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage( sender_id , delay), m_BBVisible(bb_visible)
		  {

		  }
		  bool ShowBoundingBox() const {return m_BBVisible;}
	private:
		bool m_BBVisible;
	};

	typedef SPTR<BoundingInfoRequest> BoundingInfoRequestPtr;


	/**
	Request update of Euler angles from quaternion, by performance reasons this 
	is usually not done by default in the location component
	*/
	class UpdateEulerAnglesRequest : public SceneObjectRequestMessage
	{
	public:
		UpdateEulerAnglesRequest(SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage( sender_id , delay)
		  {
		  }
	};

	typedef SPTR<UpdateEulerAnglesRequest> UpdateEulerAnglesRequestPtr;


	
	/**
	Message used to load mesh files, mesh components will listen to this message
	*/

	class MeshFileRequest : public SceneObjectRequestMessage
	{
	public:
		MeshFileRequest(const std::string &mesh_file, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage(sender_id , delay), m_FileName(mesh_file)
		  {

		  }
		  std::string GetFileName()const {return m_FileName;}
	private:
		std::string m_FileName;
	};
	typedef SPTR<MeshFileRequest> MeshFileRequestPtr;

	/**
	Modify mesh texture coordinates,  mesh components will listen to this message
	*/

	class TextureCoordinateRequest : public SceneObjectRequestMessage
	{
	public:
		TextureCoordinateRequest(const Vec2 &st, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage(sender_id , delay), m_TexCoords(st){}
		  Vec2 GetTextureCoordinates()const {return m_TexCoords;}
	private:
		Vec2 m_TexCoords;
	};
	typedef SPTR<TextureCoordinateRequest> TextureCoordinateRequestPtr;

	/**
	Replace material for sub mesh.
	@param material_name New material name
	@param sub_mesh_id Select which sub mesh id to replace/mod. If sub_mesh_id is -1 all sub_meshes are effected
	*/
	class ReplaceMaterialRequest : public SceneObjectRequestMessage
	{
	public:
		ReplaceMaterialRequest(const std::string &material_name, int sub_mesh_id = -1, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage(sender_id , delay), m_MaterialName(material_name),m_SubMeshID(sub_mesh_id)
		  {

		  }
		  std::string GetMaterialName() const {return m_MaterialName;}
		  int GetSubMeshID()const {return m_SubMeshID;}
	private:
		std::string m_MaterialName;
		int m_SubMeshID;
	};
	typedef SPTR<ReplaceMaterialRequest> ReplaceMaterialRequestPtr;


	/**
		Reset material to original state. After sending MaterialMessage request this message can
		be used to reset material back to it's start state
	*/
	class ResetMaterialRequest : public SceneObjectRequestMessage
	{
	public:
		ResetMaterialRequest(SenderID sender_id = -1, double delay= 0) : SceneObjectRequestMessage(sender_id , delay){}
	private:
	};
	typedef SPTR<ResetMaterialRequest> ResetMaterialRequestPtr;


	/**
		Message used to change billboard color
	*/
	
	class BillboardColorRequest  : public SceneObjectRequestMessage
	{
	public:
		BillboardColorRequest(const ColorRGBA &color, SenderID sender_id = -1, double delay= 0) : SceneObjectRequestMessage(sender_id , delay) , m_Color(color){}
		ColorRGBA GetColor() const {return m_Color;}
	private:
		ColorRGBA m_Color;
	};
	typedef SPTR<BillboardColorRequest> BillboardColorRequestPtr;


	/**
	Message used to change texture of manual mesh object
	param 
	*/
	class TextureRequest : public SceneObjectRequestMessage
	{
	public:
		TextureRequest(const std::string &texture ,SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage(sender_id , delay), m_Texture(texture){}
		  std::string GetTexture()const {return m_Texture;}
		  void SetTexture(const std::string &texture) {m_Texture=texture;}
	private:
		std::string m_Texture;
	};
	typedef SPTR<TextureRequest> TextureRequestPtr;


	/**
		Message used to modify bone transformation	
	*/
	class BoneTransformationRequest : public SceneObjectRequestMessage
	{
	public:
		BoneTransformationRequest(const std::string &name, const Vec3  &pos, const Quaternion &rot,SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage(sender_id , delay), m_Name(name),m_Position(pos), m_Rotation(rot){}
		  std::string GetName() const {return m_Name;}
		  Vec3 GetPosition() const {return m_Position;}
		  Quaternion  GetRotation() const {return m_Rotation;}
	private:
		std::string m_Name;
		Vec3 m_Position;
		Quaternion m_Rotation;
	};
	typedef SPTR<BoneTransformationRequest> BoneTransformationRequestPtr;


	/**
		Message used to send new mesh data to manual mesh components
	*/
	class ManualMeshDataRequest : public SceneObjectRequestMessage
	{
	public:
		ManualMeshDataRequest(GraphicsMeshPtr data, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage(sender_id , delay), m_Data(data){}
		  GraphicsMeshPtr GetData()const {return m_Data;}
	private:
		GraphicsMeshPtr m_Data;

	};
	typedef SPTR<ManualMeshDataRequest> ManualMeshDataRequestPtr;

	
	/**
		Message used to clear manual mesh components
	*/
	class ClearManualMeshRequest : public SceneObjectRequestMessage
	{
	public:
		ClearManualMeshRequest (SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage(sender_id , delay){}
	private:
	};
	typedef SPTR<ClearManualMeshRequest> ClearManualMeshRequestPtr;

	
	/**
		Scale actual geometry and not scene node, 
		Note that only a few geometry components support this.
	*/

	class GeometryScaleRequest : public SceneObjectRequestMessage
	{
	public:

		GeometryScaleRequest(const Vec3 &scale, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage(sender_id , delay), m_Scale(scale)
		  {

		  }
		  Vec3 GetScale() const {return m_Scale;}
	private:
		Vec3 m_Scale;
	};
	typedef SPTR<GeometryScaleRequest> GeometryScaleRequestPtr;

	/**
	  Change particle system parameter
	*/

	class ParticleSystemParameterRequest : public SceneObjectRequestMessage
	{
	public:
		enum ParticleSystemParameterType
		{
			/** \brief Message data:
			Vec2 = "Speed" - Texture scroll speed in x,y(s,t) direction*/
			EMISSION_RATE,
			PARTICLE_LIFE_TIME,
		};
	public:
		ParticleSystemParameterRequest(ParticleSystemParameterType paramter, int emitter, float value, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage(sender_id , delay), m_Emitter(emitter), m_Value(value), m_Parameter(paramter){}
		  float GetValue()const {return m_Value;}
		  int GetEmitter()const {return m_Emitter;}
		  ParticleSystemParameterType GetParameter() const {return m_Parameter;}
	private:
		float m_Value;
		int m_Emitter;
		ParticleSystemParameterType m_Parameter;
	};
	typedef SPTR<ParticleSystemParameterRequest> ParticleSystemParameterRequestPtr;

	class TextCaptionRequest : public SceneObjectRequestMessage
	{
	public:
		TextCaptionRequest(const std::string  &caption, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage(sender_id , delay), m_Caption(caption){}
		  std::string GetCaption()const {return m_Caption;}
	private:
		std::string m_Caption;
	};
	typedef SPTR<TextCaptionRequest> TextCaptionRequestPtr;

	class TerrainHeightModifyRequest : public SceneObjectRequestMessage
	{
	public:
		enum ModifyType
		{
			MT_DEFORM,
			MT_FLATTEN,
			MT_SMOOTH
		};
		TerrainHeightModifyRequest(ModifyType type, const Vec3 &pos, float brush_size, float brush_inner_size, float intensity = 1, float noise = 0, SenderID sender_id = -1, double delay= 0) :  
			SceneObjectRequestMessage(sender_id , delay), 
			m_Type(type),
			m_Position(pos), 
			m_BrushSize(brush_size),
			m_BrushInnerSize(brush_inner_size),
			m_Intensity(intensity) ,
			m_Noise(noise)
		{

		}
		ModifyType GetModifyType() const { return m_Type;}
		float GetBrushSize() const {return m_BrushSize;}
		float GetBrushInnerSize() const {return m_BrushInnerSize;}
		float GetIntensity() const {return m_Intensity;}
		float GetNoise() const {return m_Noise;}
		Vec3 GetPosition() const {return m_Position;}
	private:
		ModifyType m_Type;
		Vec3 m_Position;
		float m_BrushSize;
		float m_BrushInnerSize;
		float m_Intensity;
		float m_Noise;
	};
	typedef SPTR<TerrainHeightModifyRequest> TerrainHeightModifyRequestPtr;

	
	enum TerrainLayer
	{
		TL_0,
		TL_1,
		TL_2,
		TL_3,
		TL_4

	};

	

	class TerrainPaintRequest : public SceneObjectRequestMessage
	{
	public:
		TerrainPaintRequest(const Vec3 &pos, float brush_size, float brush_inner_size, TerrainLayer layer, float intensity = 1, float noise = 0, SenderID sender_id = -1, double delay= 0) :  
		  SceneObjectRequestMessage(sender_id , delay), 
			m_Position(pos), 
			m_BrushSize(brush_size),
			m_BrushInnerSize(brush_inner_size),
			m_Intensity(intensity),
			m_Layer(layer),
			m_Noise(noise)
		{

		}
		float GetBrushSize() const {return m_BrushSize;}
		float GetBrushInnerSize() const {return m_BrushInnerSize;}
		float GetIntensity() const {return m_Intensity;}
		TerrainLayer GetLayer() const {return m_Layer;}
		Vec3 GetPosition() const {return m_Position;}
		float GetNoise() const {return m_Noise;}
	private:
		TerrainLayer m_Layer;
		Vec3 m_Position;
		float m_BrushSize;
		float m_BrushInnerSize;
		float m_Intensity;
		float m_Noise;
	};
	typedef SPTR<TerrainPaintRequest> TerrainPaintRequestPtr;

	class TerrainLayerRequest : public SceneObjectRequestMessage
	{
	public:

		TerrainLayerRequest(TerrainLayer layer, const std::string &texture, float tiling, SenderID sender_id = -1, double delay= 0) :  
		  SceneObjectRequestMessage(sender_id , delay), 
			m_Layer(layer),
			m_Texture(texture), 
			m_Tiling(tiling)
		{

		}
		TerrainLayer  GetLayer() const { return m_Layer;}
		float GetTiling() const {return m_Tiling;}
		ResourceHandle GetTexture() const {return m_Texture;}
	private:
		TerrainLayer m_Layer;
		ResourceHandle m_Texture;
		float m_Tiling;
	};
	typedef SPTR<TerrainLayerRequest> TerrainLayerRequestPtr;

	class RoadRequest : public SceneObjectRequestMessage
	{
	public:
		RoadRequest(const std::vector<Vec3> &road, float flatten_width, float paint_width, float paint_intensity, TerrainLayer layer, SenderID sender_id = -1, double delay= 0) :  
		  SceneObjectRequestMessage(sender_id , delay), 
			m_RoadWaypoints(road),
			m_Layer(layer),
			m_FlattenWidth(flatten_width),
			m_PaintWidth(paint_width),
			m_PaintIntensity(paint_intensity)
		{

		}
		float GetFlattenWidth() const {return m_FlattenWidth;}
		float GetPaintWidth() const {return m_PaintWidth;}
		float GetPaintIntensity() const {return m_PaintIntensity;}
		std::vector<Vec3>  GetRoadWaypoints() const {return m_RoadWaypoints;}
		TerrainLayer GetLayer() const {return m_Layer;}
	private:
		TerrainLayer m_Layer;
		std::vector<Vec3>  m_RoadWaypoints;
		float m_PaintWidth;
		float m_FlattenWidth;
		float m_PaintIntensity;
	};
	typedef SPTR<RoadRequest> RoadRequestPtr;

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
	typedef SPTR<LocationLoadedEvent> LocationLoadedEventPtr;


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
	typedef SPTR<TransformationChangedEvent> TransformationChangedEventPtr;

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
	typedef SPTR<GeometryChangedEvent> GeometryChangedEventPtr;


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
	typedef SPTR<GeometryFlagsChangedEvent> GeometryFlagsChangedEventPtr;



	
}
