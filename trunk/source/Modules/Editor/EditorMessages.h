#ifndef EDITOR_MESSAGES
#define EDITOR_MESSAGES
#include "Core/MessageSystem/GASSBaseMessage.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Messages/GASSCoreSceneMessages.h"
#include "Sim/Messages/GASSCoreSystemMessages.h"
#include "Sim/GASSSceneObjectTemplate.h"

namespace GASS
{
	class ObjectSelectionChangedEvent : public SceneEventMessage
	{
	public:
		ObjectSelectionChangedEvent(SceneObjectPtr obj, SenderID sender_id = -1, double delay= 0) : 
		  SceneEventMessage(sender_id , delay), m_Object(obj){}
		  SceneObjectPtr GetSceneObject() const {return m_Object;}
	private:
		SceneObjectPtr m_Object;
	};
	typedef SPTR<ObjectSelectionChangedEvent> ObjectSelectionChangedEventPtr;

	class SceneSelectionChangedEvent : public SceneEventMessage
	{
	public:
		SceneSelectionChangedEvent(ScenePtr scene, SenderID sender_id = -1, double delay= 0) : 
		  SceneEventMessage(sender_id , delay), m_Scene(scene){}
		  ScenePtr GetScene() const {return m_Scene;}
	private:
		ScenePtr m_Scene;
	};
	typedef SPTR<SceneSelectionChangedEvent> SceneSelectionChangedEventPtr;

	class ObjectLockChangedEvent : public SceneEventMessage
	{
	public:
		ObjectLockChangedEvent(SceneObjectPtr obj, bool lock, SenderID sender_id = -1, double delay= 0) : 
		  SceneEventMessage(sender_id , delay), m_Object(obj), m_Lock(lock){}
		  SceneObjectPtr GetSceneObject() const {return m_Object;}
		  bool GetLock() const {return m_Lock;}
	private:
		SceneObjectPtr m_Object;
		bool m_Lock;
	};
	typedef SPTR<ObjectLockChangedEvent> ObjectLockChangedEventPtr;

	class ObjectVisibilityChangedEvent : public SceneEventMessage
	{
	public:
		ObjectVisibilityChangedEvent(SceneObjectPtr obj, bool visible, SenderID sender_id = -1, double delay= 0) : 
		  SceneEventMessage(sender_id , delay), m_Object(obj), m_Visible(visible){}
		  SceneObjectPtr GetSceneObject() const {return m_Object;}
		  bool GetVisible() const {return m_Visible;}
	private:
		SceneObjectPtr m_Object;
		bool m_Visible;
	};
	typedef SPTR<ObjectVisibilityChangedEvent > ObjectVisibilityChangedEventPtr;

	class ObjectSiteChangedEvent : public SceneEventMessage
	{
	public:
		ObjectSiteChangedEvent(SceneObjectPtr obj, SenderID sender_id = -1, double delay= 0) : 
		  SceneEventMessage(sender_id , delay), m_Object(obj) {}
		  SceneObjectPtr GetSceneObject() const {return m_Object;}
	private:
		SceneObjectPtr m_Object;
	};
	typedef SPTR<ObjectSiteChangedEvent> ObjectSiteChangedEventPtr;


	class ObjectZoomedEvent : public SceneEventMessage
	{
	public:
		ObjectZoomedEvent(SceneObjectPtr obj, SenderID sender_id = -1, double delay= 0) : 
		  SceneEventMessage(sender_id , delay), m_Object(obj){}
		  SceneObjectPtr GetSceneObject() const {return m_Object;}
	private:
		SceneObjectPtr m_Object;
	};
	typedef SPTR<ObjectZoomedEvent> ObjectZoomedEventPtr;

	

	class SceneObjectTemplateDroppedEvent : public SystemEventMessage
	{
	public:
		SceneObjectTemplateDroppedEvent(const std::string &obj_name, const Vec2 &screen_pos, SenderID sender_id = -1, double delay= 0) : 
		  SystemEventMessage(sender_id , delay), m_ObjectName(obj_name), m_ScreenPosition(screen_pos) {}
		  std::string GetObjectName() const {return m_ObjectName;}
		  Vec2 GetScreenPosition() const {return m_ScreenPosition;}
	private:
		std::string m_ObjectName;
		Vec2 m_ScreenPosition;
	};
	typedef SPTR<SceneObjectTemplateDroppedEvent> SceneObjectTemplateDroppedEventPtr;


	
	class CursorMovedOverSceneEvent : public SceneEventMessage
	{
	public:
		CursorMovedOverSceneEvent(const Vec2 &screen_pos,const Vec3 &pos3d, SceneObjectPtr obj, SenderID sender_id = -1, double delay= 0) : 
		  SceneEventMessage(sender_id , delay), m_ScreenPosition(screen_pos), m_3DPosition(pos3d), m_Object(obj){}
		  SceneObjectPtr GetSceneObjectUnderCursor() const {return m_Object;}
		  Vec2 GetScreenPosition() const {return m_ScreenPosition;}
		  Vec3 Get3DPosition() const {return m_3DPosition;}
	private:
		Vec2 m_ScreenPosition;
		Vec3 m_3DPosition;
		SceneObjectPtr m_Object;
	};
	typedef SPTR<CursorMovedOverSceneEvent> CursorMovedOverSceneEventPtr;



	class PaintRequest : public SceneRequestMessage
	{
	public:
		PaintRequest(const Vec3 &pos, SceneObjectPtr obj, SenderID sender_id = -1, double delay= 0) : 
		  SceneRequestMessage(sender_id , delay), m_Position(pos), m_Object(obj){}
		  SceneObjectPtr GetObjectUnderCursor() const {return m_Object;}
		  Vec3 GetPosition() const {return m_Position;}
	private:
		Vec3 m_Position;
		SceneObjectPtr m_Object;
	};
	typedef SPTR<PaintRequest> PaintRequestPtr;

	class ToolChangedEvent : public SystemEventMessage
	{
	public:
		ToolChangedEvent(const std::string &tool,  SenderID sender_id = -1, double delay= 0) : 
		  SystemEventMessage(sender_id , delay), 
			  m_Tool(tool)
		  {

		  }
		  std::string GetTool() const {return m_Tool;}
	private:
		std::string  m_Tool;
	};
	typedef SPTR<ToolChangedEvent> ToolChangedEventPtr;


	class SceneChangedEvent: public SystemEventMessage
	{
	public:
		SceneChangedEvent(SenderID sender_id = -1, double delay= 0) : 
		  SystemEventMessage(sender_id , delay){}
	private:
	};
	typedef SPTR<SceneChangedEvent> SceneChangedEventPtr;
		
	class SceneObjectChangedEvent : public SceneEventMessage
	{
	public:
		SceneObjectChangedEvent(SceneObjectPtr obj, SenderID sender_id = -1, double delay= 0) : 
		  SceneEventMessage(sender_id , delay), m_Object(obj){}
		  SceneObjectPtr GetSceneObject() const {return m_Object;}
	private:
		SceneObjectPtr m_Object;
	};
	typedef SPTR<SceneObjectChangedEvent> SceneObjectChangedEventPtr;

	class ObjectAttributeChangedEvent : public SceneEventMessage
	{
	public:
		ObjectAttributeChangedEvent(BaseReflectionObjectPtr obj, const std::vector<std::string> &attribs, SenderID sender_id = -1, double delay= 0) : 
		  SceneEventMessage(sender_id , delay), m_Object(obj), m_Attributes(attribs){}
		  BaseReflectionObjectPtr GetObject() const {return m_Object;}
		  std::vector<std::string> GetAttributes() const {return m_Attributes;}
	private:
		BaseReflectionObjectPtr m_Object;
		std::vector<std::string> m_Attributes;
	};
	typedef SPTR<ObjectAttributeChangedEvent> ObjectAttributeChangedEventPtr;


	

	class LayoutLoadedMessage : public BaseMessage
	{
	public:
		LayoutLoadedMessage(SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay){}
	private:
	};
	typedef SPTR<LayoutLoadedMessage> LayoutLoadedMessagePtr;



	class CreateResourceMessage : public BaseMessage
	{
	public:
		CreateResourceMessage(SceneObjectTemplatePtr template_object, const std::string &menu_name,const std::string &icon_name, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Menu(menu_name),m_Template(template_object),m_Icon(icon_name){}
		  std::string GetMenu() const {return m_Menu;}
		  SceneObjectTemplatePtr GetTemplate() const {return m_Template;}
		  std::string GetIcon() const {return m_Icon;}
	private:
		SceneObjectTemplatePtr m_Template;
		std::string m_Menu;
		std::string m_Icon;
	};
	typedef SPTR<CreateResourceMessage> CreateResourceMessagePtr;


	class RemoveResourceMessage : public BaseMessage
	{
	public:
		RemoveResourceMessage(const std::string &template_name, const std::string &menu_name,SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Menu(menu_name), m_Template(template_name){}
		  std::string GetTemplate() const {return m_Template;}
		  std::string GetMenu() const {return m_Menu;}
	private:
		std::string m_Template;
		std::string m_Menu;
	};
	typedef SPTR<RemoveResourceMessage> RemoveResourceMessagePtr;

	class AboutMessage : public BaseMessage
	{
	public:
		AboutMessage(SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay) {}
	};
	typedef SPTR<AboutMessage> AboutMessagePtr;



	class NewGASSSceneMessage : public BaseMessage
	{
	public:
		NewGASSSceneMessage(const std::string &filename, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Filename(filename){}
		  std::string GetSceneName() const {return m_Filename;}
	private:
		std::string  m_Filename;
	};
	typedef SPTR<NewGASSSceneMessage> NewGASSSceneMessagePtr;

	class LoadGASSSceneMessage : public BaseMessage
	{
	public:
		LoadGASSSceneMessage(const std::string &filename, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Filename(filename){}
		  std::string GetSceneName() const {return m_Filename;}
	private:
		std::string  m_Filename;
	};
	typedef SPTR<LoadGASSSceneMessage> LoadGASSSceneMessagePtr;


	class SaveGASSSceneMessage : public BaseMessage
	{
	public:
		SaveGASSSceneMessage(const std::string &filename, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Filename(filename){}
		  std::string GetSceneName() const {return m_Filename;}
	private:
		std::string  m_Filename;
	};
	typedef SPTR<SaveGASSSceneMessage> SaveGASSSceneMessagePtr;

	class EditModeMessage : public BaseMessage
	{
	public:
		EditModeMessage(const std::string &mode, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Mode(mode){}
		  std::string GetEditMode() const {return m_Mode;}
	private:
		std::string  m_Mode;
	};
	typedef SPTR<EditModeMessage> EditModeMessagePtr;


	/*class SnapSettingsMessage : public BaseMessage
	{
	public:
		SnapSettingsMessage(Float movment_snap, Float angle_snap, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_MSnap(movment_snap), m_RSnap(angle_snap){}
		  Float GetMovementSnap() const {return m_MSnap;}
		  Float GetRotationSnap() const {return m_RSnap;}
	private:
		Float m_MSnap;
		Float m_RSnap;
	};
	typedef SPTR<SnapSettingsMessage> SnapSettingsMessagePtr;

	class SnapModeMessage : public BaseMessage
	{
	public:
		SnapModeMessage (bool movment_snap, bool angle_snap, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_MSnap(movment_snap), m_RSnap(angle_snap){}
		  bool MovementSnapEnabled() const {return m_MSnap;}
		  bool RotationSnapEnabled() const {return m_RSnap;}
	private:
		bool m_MSnap;
		bool m_RSnap;
	};
	typedef SPTR<SnapModeMessage> SnapModeMessagePtr;
	*/


	class ChangeGridRequest : public SystemRequestMessage
	{
	public:
		ChangeGridRequest(Float size, Float spacing, SenderID sender_id = -1, double delay= 0) : 
		  SystemRequestMessage(sender_id , delay), m_Size(size), m_Spacing(spacing){}
		  Float GetSpacing() const {return m_Spacing;}
		  Float GetSize() const {return m_Size;}
	private:
		Float m_Spacing;
		Float m_Size;
	};
	typedef SPTR<ChangeGridRequest> ChangeGridRequestPtr;
	

	class TemplateSelectedMessage : public BaseMessage
	{
	public:
		TemplateSelectedMessage(const std::string &name, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Name(name){}
		  std::string GetName() const {return m_Name;}

	private:
		std::string  m_Name;
	};
	typedef SPTR<TemplateSelectedMessage> TemplateSelectedMessagePtr;


	class EditPositionMessage : public BaseMessage
	{
	public:
		EditPositionMessage(const Vec3 &pos, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay),m_Position(pos)
		  {

		  }
		  Vec3 GetPosition() const {return m_Position;}
	private:
		Vec3 m_Position;
	};
	typedef SPTR<EditPositionMessage> EditPositionMessagePtr;


	//MFC stuff, use mfc/windows messages instead?
	class ShowSceneObjectMenuRequest : public SceneRequestMessage
	{
	public:
		ShowSceneObjectMenuRequest(SceneObjectPtr obj, const Vec2 &screen_pos, SenderID sender_id = -1, double delay= 0) : 
		  SceneRequestMessage(sender_id , delay), m_Object(obj), m_Pos (screen_pos){}
		  SceneObjectPtr GetSceneObject() const {return m_Object;}
		  Vec2 GetScreenPosition() const {return m_Pos;}
	private:
		SceneObjectPtr m_Object;
		Vec2 m_Pos;
	};
	typedef SPTR<ShowSceneObjectMenuRequest> ShowSceneObjectMenuRequestPtr;


	class TextFeedbackRequest : public SystemRequestMessage
	{
	public:
		TextFeedbackRequest(std::string text, SenderID sender_id = -1, double delay= 0) : 
		  SystemRequestMessage(sender_id , delay), m_Text(text){}
		  std::string GetText() const {return m_Text;}
	private:
		std::string m_Text;
	};
	typedef SPTR<TextFeedbackRequest> TextFeedbackRequestPtr;


	class ClearFeedbackWindowRequest: public SystemRequestMessage
	{
	public:
		ClearFeedbackWindowRequest(SenderID sender_id = -1, double delay= 0) : 
		  SystemRequestMessage(sender_id , delay) {}
	};
	typedef SPTR<ClearFeedbackWindowRequest> ClearFeedbackWindowRequestPtr;


}
#endif 