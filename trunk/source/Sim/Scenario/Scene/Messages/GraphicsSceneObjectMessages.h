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

#include "Sim/Common.h"
#include "Core/MessageSystem/BaseMessage.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/Math/Vector.h"
#include "Core/Math/Quaternion.h"

/**
This file hold messages that should be posted to
SceneObjects. SceneObjectMessages are used to
enable communication between components hold by
a SceneObject. The user is free to extend this
set of messages but this file hold the "core"
messages that all components share.
If a scene object message is added and it's
of common interest it can be a candidate for
this file.
*/

namespace GASS
{
	class SimSceneManager;
	class ISceneManager;
	class IGeometryComponent;
	struct ManualMeshData;
	typedef boost::shared_ptr<ManualMeshData> ManualMeshDataPtr;
	typedef boost::shared_ptr<ISceneManager> SceneManagerPtr;
	typedef boost::shared_ptr<IGeometryComponent> GeometryComponentPtr;



	class LoadGFXComponentsMessage : public BaseMessage
	{
	public:
		LoadGFXComponentsMessage(SceneManagerPtr gfx_scene_manager, void* user_data = NULL,SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_GFXSceneManager(gfx_scene_manager),m_UserData(user_data){}
		  SceneManagerPtr GetGFXSceneManager() const {return m_GFXSceneManager;}
		  void* GetUserData() const {return m_UserData;}
	private:
		SceneManagerPtr m_GFXSceneManager;
		void *m_UserData;
	};
	typedef boost::shared_ptr<LoadGFXComponentsMessage > LoadGFXComponentsMessagePtr;


	/**
	Position (relative to parent) change is requested,
	Typically the location component respond to this message
	by moving the node in the scene graph. The location component
	is usually implemented in the graphic system and is used by GASS
	to transfer location information to a scene graph node.
	However, it's also possible to have other type of components
	that respond to position messages, for instance a sound source
	probably also want to catch location information.
	*/

	class PositionMessage : public BaseMessage
	{
	public:

		PositionMessage(const Vec3 &pos, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_Position(pos)
		  {

		  }
		  Vec3 GetPosition() const {return m_Position;}
	private:
		Vec3 m_Position;
	};
	typedef boost::shared_ptr<PositionMessage> PositionMessagePtr;


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

	class RotationMessage : public BaseMessage
	{
	public:
		RotationMessage(const Quaternion &rot, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_Rotation(rot)
		  {

		  }
		  Quaternion GetRotation() const {return m_Rotation;}
	private:
		Quaternion m_Rotation;
	};
	typedef boost::shared_ptr<RotationMessage> RotationMessagePtr;



	/**
	Position change is requested,
	Typically the location component respond to this message
	by moving the node in the scene graph. The location component
	is usually implemented in the graphic system and is used by GASS
	to transfer location information to a scene graph node.
	However, it's also possible to have other type of components
	that respond to position messages, for instance a sound source
	probably also want to catch location information.

	Note: In contrast to the PositionMessage this is a request for a
	absolute position change, if the location component that
	respond to this messages has a scene graph node that is
	attached to a parent node this position change will
	probably require more calulations due to the fact
	a new relative position also have to be calulated.
	*/


	class WorldPositionMessage : public BaseMessage
	{
	public:
		WorldPositionMessage(const Vec3 &pos, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage( sender_id , delay), m_Position(pos)
		  {

		  }
		  Vec3 GetPosition() const {return m_Position;}
	private:
		Vec3 m_Position;
	};
	typedef boost::shared_ptr<WorldPositionMessage> WorldPositionMessagePtr;



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

	Note: In contrast to the RotationMessage this is a request for a
	absolute rotation change, if the location component that
	respond to this messages has a scene graph node that is
	attached to a parent node this rotation change will
	probably require more calulations due to the fact
	a new relative rotation also have to be calulated.
	*/

	class WorldRotationMessage : public BaseMessage
	{
	public:
		WorldRotationMessage(const Quaternion &rot, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_Rotation(rot)
		  {

		  }
		  Quaternion GetRotation() const {return m_Rotation;}
	private:
		Quaternion m_Rotation;
	};
	typedef boost::shared_ptr<WorldRotationMessage> WorldRotationMessagePtr;

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
	class ScaleMessage : public BaseMessage
	{
	public:

		ScaleMessage (const Vec3 &scale, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_Scale(scale)
		  {

		  }
		  Vec3 GetScale() const {return m_Scale;}
	private:
		Vec3 m_Scale;
	};
	typedef boost::shared_ptr<ScaleMessage> ScaleMessagePtr;


	/** Message sent by scene node when scene node is moved
	*/

	class TransformationNotifyMessage : public BaseMessage
	{
	public:
		/**
			Constructor
			@param pos Position in world coordiante space
			@param rot	Rotation in world coordiante space
			@param scale Scale in world coordiante space
		*/
		TransformationNotifyMessage(const Vec3  &pos, const Quaternion &rot, const Vec3  &scale,SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_Position(pos), m_Rotation(rot), m_Scale(scale){}
		  Vec3 GetPosition() const {return m_Position;}
		  Quaternion  GetRotation() const {return m_Rotation;}
		  Vec3 GetScale() const {return m_Scale;}
	private:
		Vec3 m_Position;
		Quaternion m_Rotation;
		Vec3 m_Scale;
	};
	typedef boost::shared_ptr<TransformationNotifyMessage> TransformationNotifyMessagePtr;


	/**
	Message used to change visibility of scene nodes,
	Typically the location component respond to this message
	by hiding/unhiding the node in the graphic system scene graph.

	Note: that this messages only intended to hide the visual part
	of a object, sound, physics or any other components
	should not respond to this message. For instance, to disable
	the physics collision component instead use the
	CollisionSettingsMessage.
	*/

	class VisibilityMessage : public BaseMessage
	{
	public:
		VisibilityMessage(bool visible, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_Visible(visible)
		  {

		  }
		  bool GetValue() const {return m_Visible;}
	private:
		bool m_Visible;
	};

	typedef boost::shared_ptr<VisibilityMessage> VisibilityMessagePtr;


	/**
	Message use to alternate whether or not a gfx-scene node should be attached 
	to the first parent gfx-node
	*/
	class AttachToParentMessage : public BaseMessage
	{
	public:
		AttachToParentMessage(bool value,SenderID sender_id = -1, double delay= 0) :
		  BaseMessage( sender_id , delay)
		  {

		  }
		  bool GetAttachToParent() const {return m_AttachToParent;}
	private:
		bool m_AttachToParent;
	};
	typedef boost::shared_ptr<AttachToParentMessage> AttachToParentMessagePtr;


	/**
	Message use indicate that a the scene node structure has changed and that this should 
	be reflected in gfx-scene node should be attached 
	to the first parent gfx-node
	*/

	class ParentChangedMessage : public BaseMessage
	{
	public:
		ParentChangedMessage(SenderID sender_id = -1, double delay= 0) :
		  BaseMessage( sender_id , delay)
		  {

		  }
	private:

	};
	typedef boost::shared_ptr<ParentChangedMessage> ParentChangedMessagePtr;

	/**
		Message used to modify camera settings
	*/
	class CameraParameterMessage : public BaseMessage
	{
	public:
		enum CameraParameterType
		{
			CAMERA_FOV,
			CAMERA_CLIP_DISTANCE,
			CAMERA_ORTHO_WIN_SIZE,
		};
	public:
		CameraParameterMessage(CameraParameterType paramter, float value1, float value2 = 0, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_Value1(value1),m_Value2(value2), m_Parameter(paramter){}
		  float GetValue1()const {return m_Value1;}
		  float GetValue2()const {return m_Value2;}
		  CameraParameterType GetParameter()const {return m_Parameter;}
	private:
		float m_Value1;
		float m_Value2;
		CameraParameterType m_Parameter;
	};
	typedef boost::shared_ptr<CameraParameterMessage> CameraParameterMessagePtr;


	class BoundingInfoMessage : public BaseMessage
	{
	public:
		BoundingInfoMessage(bool bb_visible, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage( sender_id , delay), m_BBVisible(bb_visible)
		  {

		  }
		  bool ShowBoundingBox() const {return m_BBVisible;}
	private:
		bool m_BBVisible;
	};

	typedef boost::shared_ptr<BoundingInfoMessage> BoundingInfoMessagePtr;


	/**
	Request update of euler angles from quaternion, by performance reasons this 
	is usually not done by default in the location component
	*/
	class UpdateEulerAnglesMessage : public BaseMessage
	{
	public:
		UpdateEulerAnglesMessage(SenderID sender_id = -1, double delay= 0) :
		  BaseMessage( sender_id , delay)
		  {
		  }
	};

	typedef boost::shared_ptr<UpdateEulerAnglesMessage> UpdateEulerAnglesMessagePtr;


	
	/**
	Message used to load mesh files, mesh components will listen to this message
	*/

	class MeshFileMessage : public BaseMessage
	{
	public:
		MeshFileMessage(const std::string &mesh_file, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_FileName(mesh_file)
		  {

		  }
		  std::string GetFileName()const {return m_FileName;}
	private:
		std::string m_FileName;
	};
	typedef boost::shared_ptr<MeshFileMessage> MeshFileMessagePtr;

	/**
	Modify mesh texture coordinates,  mesh components will listen to this message
	*/
	class TextureCoordinateMessage : public BaseMessage
	{
	public:
		TextureCoordinateMessage(const Vec2 &st, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_TexCoords(st){}
		  Vec2 GetTextureCoordinates()const {return m_TexCoords;}
	private:
		Vec2 m_TexCoords;
	};
	typedef boost::shared_ptr<TextureCoordinateMessage> TextureCoordinateMessagePtr;

	/**
	Change material of mesh components
	*/
	class MaterialMessage : public BaseMessage
	{
	public:
		MaterialMessage(const Vec4 &diffuse,const Vec3 &ambient,const Vec3 &specular = Vec3(-1,-1,-1), const Vec3 &selfIllumination = Vec3(-1,-1,-1), float shininess = -1,bool depth_test_on = true,SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_Diffuse(diffuse),m_Ambient(ambient),m_Specular(specular),m_SelfIllumination(selfIllumination ),m_Shininess(shininess), m_DepthTest(depth_test_on){}
		  Vec4 GetDiffuse()const {return m_Diffuse;}
		  Vec3 GetAmbient()const {return m_Ambient;}
		  Vec3 GetSpecular()const {return m_Specular;}
		  Vec3 GetSelfIllumination()const {return m_SelfIllumination;}
		  float GetShininess()const {return m_Shininess;}
		  bool GetDepthTest()const {return m_DepthTest;}
	private:
		Vec4 m_Diffuse;
		Vec3 m_Ambient;
		Vec3 m_Specular;
		Vec3 m_SelfIllumination;
		float m_Shininess;
		bool m_DepthTest;
	};
	typedef boost::shared_ptr<MaterialMessage> MaterialMessagePtr;


	/**
	Message used to change texture of manual mesh object
	param 
	*/
	class TextureMessage : public BaseMessage
	{
	public:
		TextureMessage(const std::string &texture ,SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_Texture(texture){}
		  std::string GetTexture()const {return m_Texture;}
		  void SetTexture(const std::string &texture) {m_Texture=texture;}
	private:
		std::string m_Texture;
	};
	typedef boost::shared_ptr<TextureMessage> TextureMessagePtr;


	/**
		Message used to modify bone transformation	
	*/
	class BoneTransformationMessage : public BaseMessage
	{
	public:
		BoneTransformationMessage(const std::string &name, const Vec3  &pos, const Quaternion &rot,SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_Name(name),m_Position(pos), m_Rotation(rot){}
		  std::string GetName() const {return m_Name;}
		  Vec3 GetPosition() const {return m_Position;}
		  Quaternion  GetRotation() const {return m_Rotation;}
	private:
		std::string m_Name;
		Vec3 m_Position;
		Quaternion m_Rotation;
	};
	typedef boost::shared_ptr<BoneTransformationMessage> BoneTransformationMessagePtr;


	class ManualMeshDataMessage : public BaseMessage
	{
	public:
		ManualMeshDataMessage(ManualMeshDataPtr data, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_Data(data){}
		  ManualMeshDataPtr GetData()const {return m_Data;}
	private:
		ManualMeshDataPtr m_Data;

	};
	typedef boost::shared_ptr<ManualMeshDataMessage> ManualMeshDataMessagePtr;

	class ClearManualMeshMessage : public BaseMessage
	{
	public:
		ClearManualMeshMessage (SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay){}
	private:
	};
	typedef boost::shared_ptr<ClearManualMeshMessage> ClearManualMeshMessagePtr;

	class GeometryChangedMessage : public BaseMessage
	{
	public:
		GeometryChangedMessage(GeometryComponentPtr geom, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_Geometry(geom){}
	private:
		GeometryComponentPtr m_Geometry;
	};
	typedef boost::shared_ptr<GeometryChangedMessage> GeometryChangedMessagePtr;


	class GeometryScaleMessage : public BaseMessage
	{
	public:

		GeometryScaleMessage(const Vec3 &scale, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_Scale(scale)
		  {

		  }
		  Vec3 GetScale() const {return m_Scale;}
	private:
		Vec3 m_Scale;
	};
	typedef boost::shared_ptr<GeometryScaleMessage> GeometryScaleMessagePtr;


	class ParticleSystemParameterMessage : public BaseMessage
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
		ParticleSystemParameterMessage(ParticleSystemParameterType paramter, int emitter, float value, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_Emitter(emitter), m_Value(value), m_Parameter(paramter){}
		  float GetValue()const {return m_Value;}
		  int GetEmitter()const {return m_Emitter;}
		  ParticleSystemParameterType GetParameter() const {return m_Parameter;}

	private:
		float m_Value;
		int m_Emitter;
		ParticleSystemParameterType m_Parameter;
	};
	typedef boost::shared_ptr<ParticleSystemParameterMessage> ParticleSystemParameterMessagePtr;

	class TextCaptionMessage : public BaseMessage
	{
	public:
		TextCaptionMessage(const std::string  &caption, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_Caption(caption){}
		  std::string GetCaption()const {return m_Caption;}
	private:
		std::string m_Caption;
	};
	typedef boost::shared_ptr<TextCaptionMessage> TextCaptionMessagePtr;

	class TerrainHeightModifyMessage : public BaseMessage
	{
	public:
		enum ModifyType
		{
			MT_DEFORM,
			MT_FLATTEN,
			MT_SMOOTH
		};
		TerrainHeightModifyMessage(ModifyType type, const Vec3 &pos, float brush_size, float brush_inner_size, float intensity = 1, float noise = 0, SenderID sender_id = -1, double delay= 0) :  BaseMessage(sender_id , delay), 
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
	typedef boost::shared_ptr<TerrainHeightModifyMessage> TerrainHeightModifyMessagePtr;

	
	enum TerrainLayer
	{
		TL_0,
		TL_1,
		TL_2,
		TL_3,
		TL_4

	};

	

	class TerrainPaintMessage : public BaseMessage
	{
	public:
		TerrainPaintMessage(const Vec3 &pos, float brush_size, float brush_inner_size, TerrainLayer layer, float intensity = 1, float noise = 0, SenderID sender_id = -1, double delay= 0) :  BaseMessage(sender_id , delay), 
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
	typedef boost::shared_ptr<TerrainPaintMessage> TerrainPaintMessagePtr;

	class TerrainLayerMessage : public BaseMessage
	{
	public:

		TerrainLayerMessage(TerrainLayer layer, const std::string &texture, float tiling, SenderID sender_id = -1, double delay= 0) :  BaseMessage(sender_id , delay), 
			m_Layer(layer),
			m_Texture(texture), 
			m_Tiling(tiling)
		{

		}
		TerrainLayer  GetLayer() const { return m_Layer;}
		float GetTiling() const {return m_Tiling;}
		std::string GetTexture() const {return m_Texture;}
	private:
		TerrainLayer m_Layer;
		std::string m_Texture;
		float m_Tiling;
	};
	typedef boost::shared_ptr<TerrainLayerMessage> TerrainLayerMessagePtr;

	class RoadMessage : public BaseMessage
	{
	public:
		RoadMessage(const std::vector<Vec3> &road, float flatten_width, float paint_width, float paint_intensity, TerrainLayer layer, SenderID sender_id = -1, double delay= 0) :  BaseMessage(sender_id , delay), 
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
	typedef boost::shared_ptr<RoadMessage> RoadMessagePtr;
	
}
