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


namespace GASS
{
	class SimSceneManager;
	class ISceneManager;
	struct ManualMeshData;
	typedef boost::shared_ptr<ManualMeshData> ManualMeshDataPtr;
	typedef boost::shared_ptr<ISceneManager> SceneManagerPtr;


	//Position (relative to parent) change for SceneObject is requested
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


	/** \brief: 
	Rotation (relative to parent) change for SceneObject is requested 
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

	class CollisionSettingsMessage : public BaseMessage
	{
	public:
		CollisionSettingsMessage(bool enable, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Enable(enable)
		  {

		  }
		  bool EnableCollision() const {return m_Enable;}
	private:
		bool m_Enable;
	};

	typedef boost::shared_ptr<CollisionSettingsMessage> CollisionSettingsMessagePtr;

	class PhysicsJointMessage : public BaseMessage
	{
	public:
		enum PhysicsJointParameterType
		{
			AXIS1_VELOCITY,
			AXIS2_VELOCITY,
			AXIS1_FORCE,
			AXIS2_FORCE,
			AXIS1_DESIRED_ANGLE
		};
	public:
		PhysicsJointMessage(PhysicsJointParameterType parameter, float value, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Value(value), m_Parameter(parameter)
		  {

		  }
		  float GetValue()const {return m_Value;}
		  PhysicsJointParameterType GetParameter()const {return m_Parameter;}
	private:
		PhysicsJointParameterType m_Parameter;
		float m_Value;
	};
	typedef boost::shared_ptr<PhysicsJointMessage> PhysicsJointMessagePtr;


	class PhysicsBodyMessage : public BaseMessage
	{
	public:
		enum PhysicsBodyParameterType
		{
			TORQUE,
			FORCE,
			VELOCITY,
			ENABLE,
			DISABLE
		};
	public:
		PhysicsBodyMessage(PhysicsBodyParameterType parameter, Vec3 value, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Value(value), m_Parameter(parameter)
		  {

		  }
		  Vec3 GetValue()const {return m_Value;}
		  PhysicsBodyParameterType GetParameter()const {return m_Parameter;}
	private:
		PhysicsBodyParameterType m_Parameter;
		Vec3 m_Value;
	};
	typedef boost::shared_ptr<PhysicsBodyMessage> PhysicsBodyMessagePtr;

	class PhysicsMassMessage : public BaseMessage
	{
	public:
		PhysicsMassMessage(Float mass, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Value(mass)
		  {

		  }
		  Float GetMass()const {return m_Value;}
	private:
		Float m_Value;
	};

	typedef boost::shared_ptr<PhysicsMassMessage> PhysicsMassMessagePtr;

	class SoundParameterMessage : public BaseMessage
	{
	public:
		enum SoundParameterType
		{
			PLAY,
			STOP,
			PAUSE,
			PITCH,
			LOOP,
			VOLUME,
		};
	public:
		SoundParameterMessage(SoundParameterType parameter, float value, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Value(value), m_Parameter(parameter)
		  {

		  }
		  float GetValue()const {return m_Value;}
		  SoundParameterType GetParameter()const {return m_Parameter;}
	private:
		SoundParameterType m_Parameter;
		float m_Value;
	};
	typedef boost::shared_ptr<SoundParameterMessage> SoundParameterMessagePtr;


	/**
	Change name of scene object
	*/
	class SceneObjectNameMessage : public BaseMessage
	{
	public:
		SceneObjectNameMessage(const std::string &name, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Name(name)
		  {

		  }
		  std::string GetName()const {return m_Name;}
	private:
		std::string m_Name;


	};
	typedef boost::shared_ptr<SceneObjectNameMessage> SceneObjectNameMessagePtr;


	class MeshFileMessage : public BaseMessage
	{
	public:
		MeshFileMessage(const std::string mesh_file, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_FileName(mesh_file)
		  {

		  }
		  std::string GetFileName()const {return m_FileName;}
	private:
		std::string m_FileName;
	};
	typedef boost::shared_ptr<MeshFileMessage> MeshFileMessagePtr;



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

	class ColorMessage : public BaseMessage
	{
	public:
		ColorMessage(const Vec4 &color, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Color(color){}
		  Vec4 GetColor()const {return m_Color;}
	private:
		Vec4 m_Color;
	};
	typedef boost::shared_ptr<ColorMessage> ColorMessagePtr;

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
		GeometryChangedMessage(SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay){}
	private:
	};
	typedef boost::shared_ptr<GeometryChangedMessage> GeometryChangedMessagePtr;


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


	class VelocityNotifyMessage : public BaseMessage
	{
	public:
		VelocityNotifyMessage(const Vec3  &linear_velocity, const Vec3  &angular_velocity, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_LinearVel(linear_velocity), m_AngularVel(angular_velocity){}
		  Vec3 GetLinearVelocity() const {return m_LinearVel;}
		  Vec3 GetAngularVelocity() const {return m_AngularVel;}
	private:
		Vec3 m_LinearVel;
		Vec3 m_AngularVel;
	};
	typedef boost::shared_ptr<VelocityNotifyMessage> VelocityNotifyMessagePtr;

	class HingeJointNotifyMessage : public BaseMessage
	{
	public:
		HingeJointNotifyMessage(float angle,float angle_rate, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Angle(angle), m_AngleRate(angle_rate){}
		  float GetAngle() const {return m_Angle;}
		  float GetAngleRate() const {return m_AngleRate;}
	private:
		float m_Angle;
		float m_AngleRate;
	};
	typedef boost::shared_ptr<HingeJointNotifyMessage> HingeJointNotifyMessagePtr;



	/** \brief message data: 
	Vec3 = "Position"		- Position (relative to parent) is changed for SceneObject
	Vec3 = "Scale"			- Scale is changed for SceneObject
	Quaternion = "Rotation"	- Position (relative to parent) is changed for SceneObject
	*/

	class TransformationNotifyMessage : public BaseMessage
	{
	public:
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

	class UnloadComponentsMessage : public BaseMessage
	{
	public:
		UnloadComponentsMessage(SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay)
		  {

		  }
	private:
	};
	typedef boost::shared_ptr<UnloadComponentsMessage> UnloadComponentsMessagePtr;


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


	class LoadPhysicsComponentsMessage : public BaseMessage
	{
	public:
		LoadPhysicsComponentsMessage(SceneManagerPtr physics_scene_manager, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage( sender_id , delay), m_PhysicsSceneManager(physics_scene_manager){}
		  SceneManagerPtr GetPhysicsSceneManager() const {return m_PhysicsSceneManager;}
	private:
		SceneManagerPtr m_PhysicsSceneManager;
	};
	typedef boost::shared_ptr<LoadPhysicsComponentsMessage> LoadPhysicsComponentsMessagePtr;


	class LoadNetworkComponentsMessage : public BaseMessage
	{
	public:
		LoadNetworkComponentsMessage(SceneManagerPtr network_scene_manager, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage( sender_id , delay), m_NetworkSceneManager(network_scene_manager){}
		  SceneManagerPtr GetNetworkSceneManager() const {return m_NetworkSceneManager;}
	private:
		SceneManagerPtr m_NetworkSceneManager;
	};
	typedef boost::shared_ptr<LoadNetworkComponentsMessage> LoadNetworkComponentsMessagePtr;



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


	//typedef boost::shared_ptr<char> NetworkDataPtr;
	class NetworkPackage
	{
	public:
		NetworkPackage() 
		{}
		NetworkPackage(int id) : Id(id)
		{}
		virtual ~NetworkPackage(){}
		virtual int GetSize() = 0;
		virtual void Assign(char* data) = 0;
		int Id;
		//NetworkDataPtr Data;
	};
	typedef boost::shared_ptr<NetworkPackage> NetworkPackagePtr;

	class NetworkSerializeMessage : public BaseMessage
	{

	public:
		NetworkSerializeMessage(unsigned int time_stamp, NetworkPackagePtr package, SenderID sender_id = -1, double delay= 0) : 
		BaseMessage( sender_id , delay), 
		m_Package(package),
		m_TimeStamp(time_stamp){}
		NetworkPackagePtr GetPackage() const {return m_Package;}
		unsigned int GetTimeStamp() const {return m_TimeStamp;}
	private:
		NetworkPackagePtr m_Package;
		unsigned int m_TimeStamp;
	};
	typedef boost::shared_ptr<NetworkSerializeMessage> NetworkSerializeMessagePtr;


	class NetworkDeserializeMessage : public BaseMessage
	{
	public:
		NetworkDeserializeMessage(unsigned int time_stamp, NetworkPackagePtr package, SenderID sender_id = -1, double delay= 0) : 
		BaseMessage( sender_id , delay), 
		m_Package(package),
		m_TimeStamp(time_stamp){}
		NetworkPackagePtr GetPackage() const {return m_Package;}
		unsigned int GetTimeStamp() const {return m_TimeStamp;}
	private:
		NetworkPackagePtr m_Package;
		unsigned int m_TimeStamp;
	};
	typedef boost::shared_ptr<NetworkDeserializeMessage> NetworkDeserializeMessagePtr;


}
