#ifndef EDITOR_MESSAGES
#define EDITOR_MESSAGES
#include "Core/MessageSystem/GASSBaseMessage.h"
#include "Sim/Messages/GASSCoreSceneMessages.h"
#include "Sim/Messages/GASSCoreSystemMessages.h"
#include "Sim/GASSSceneObjectTemplate.h"
#include "Modules/Editor/EditorSceneManager.h"

namespace GASS
{
	class EditorSelectionChangedEvent : public SceneEventMessage
	{
	public:
		EditorSelectionChangedEvent(std::vector<SceneObjectWeakPtr> selection, SenderID sender_id = -1, double delay= 0) : 
		  SceneEventMessage(sender_id , delay), m_Selection(selection){}
		  bool IsSelected(SceneObjectPtr obj)
		  {
			  for(size_t i = 0 ; i< m_Selection.size() ;i++)
			  {
				  SceneObjectPtr sel = m_Selection[i].lock();
				  if(sel && sel == obj)
					  return true;
			  }
			  return false;
		  }

		  SceneObjectWeakPtr GetFirstSelected()
		  {
			  SceneObjectWeakPtr ret;
			  if(m_Selection.size() > 0)
			  {
				  ret = m_Selection[0];
			  }
			  return ret;
		  }
		  std::vector<SceneObjectWeakPtr> m_Selection;
	};
	typedef GASS_SHARED_PTR<EditorSelectionChangedEvent> EditorSelectionChangedEventPtr;

	class SceneSelectionChangedEvent : public SceneEventMessage
	{
	public:
		SceneSelectionChangedEvent(ScenePtr scene, SenderID sender_id = -1, double delay= 0) : 
		  SceneEventMessage(sender_id , delay), m_Scene(scene){}
		  ScenePtr GetScene() const {return m_Scene;}
	private:
		ScenePtr m_Scene;
	};
	typedef GASS_SHARED_PTR<SceneSelectionChangedEvent> SceneSelectionChangedEventPtr;

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
	typedef GASS_SHARED_PTR<ObjectLockChangedEvent> ObjectLockChangedEventPtr;

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
	typedef GASS_SHARED_PTR<ObjectVisibilityChangedEvent > ObjectVisibilityChangedEventPtr;

	class ObjectSiteChangedEvent : public SceneEventMessage
	{
	public:
		ObjectSiteChangedEvent(SceneObjectPtr obj, SenderID sender_id = -1, double delay= 0) : 
		  SceneEventMessage(sender_id , delay), m_Object(obj) {}
		  SceneObjectPtr GetSceneObject() const {return m_Object;}
	private:
		SceneObjectPtr m_Object;
	};
	typedef GASS_SHARED_PTR<ObjectSiteChangedEvent> ObjectSiteChangedEventPtr;


	class ObjectZoomedEvent : public SceneEventMessage
	{
	public:
		ObjectZoomedEvent(SceneObjectPtr obj, SenderID sender_id = -1, double delay= 0) : 
		  SceneEventMessage(sender_id , delay), m_Object(obj){}
		  SceneObjectPtr GetSceneObject() const {return m_Object;}
	private:
		SceneObjectPtr m_Object;
	};
	typedef GASS_SHARED_PTR<ObjectZoomedEvent> ObjectZoomedEventPtr;

	

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
	typedef GASS_SHARED_PTR<SceneObjectTemplateDroppedEvent> SceneObjectTemplateDroppedEventPtr;

	class SceneObjectDroppedEvent : public SystemEventMessage
	{
	public:
		SceneObjectDroppedEvent(const SceneObjectPtr &obj, const Vec2 &screen_pos, SenderID sender_id = -1, double delay = 0) :
			SystemEventMessage(sender_id, delay), m_Object(obj), m_ScreenPosition(screen_pos) {}
		SceneObjectPtr GetObject() const { return m_Object; }
		Vec2 GetScreenPosition() const { return m_ScreenPosition; }
	private:
		SceneObjectPtr m_Object;
		Vec2 m_ScreenPosition;
	};
	typedef GASS_SHARED_PTR<SceneObjectDroppedEvent> SceneObjectDroppedEventPtr;
	
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
	typedef GASS_SHARED_PTR<CursorMovedOverSceneEvent> CursorMovedOverSceneEventPtr;



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
	typedef GASS_SHARED_PTR<PaintRequest> PaintRequestPtr;

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
	typedef GASS_SHARED_PTR<ToolChangedEvent> ToolChangedEventPtr;


	class SceneChangedEvent: public SystemEventMessage
	{
	public:
		SceneChangedEvent(SenderID sender_id = -1, double delay= 0) : 
		  SystemEventMessage(sender_id , delay){}
	};
	typedef GASS_SHARED_PTR<SceneChangedEvent> SceneChangedEventPtr;
		
	class SceneObjectChangedEvent : public SceneEventMessage
	{
	public:
		SceneObjectChangedEvent(SceneObjectPtr obj, SenderID sender_id = -1, double delay= 0) : 
		  SceneEventMessage(sender_id , delay), m_Object(obj){}
		  SceneObjectPtr GetSceneObject() const {return m_Object;}
	private:
		SceneObjectPtr m_Object;
	};
	typedef GASS_SHARED_PTR<SceneObjectChangedEvent> SceneObjectChangedEventPtr;

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
	typedef GASS_SHARED_PTR<ObjectAttributeChangedEvent> ObjectAttributeChangedEventPtr;

	class LayoutLoadedMessage : public BaseMessage
	{
	public:
		LayoutLoadedMessage(SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay){}
	};
	typedef GASS_SHARED_PTR<LayoutLoadedMessage> LayoutLoadedMessagePtr;

	class CreateResourceRequest : public SystemRequestMessage
	{
	public:
		CreateResourceRequest(SceneObjectTemplatePtr template_object, const std::string &menu_name,const std::string &icon_name, SenderID sender_id = -1, double delay= 0) : 
		  SystemRequestMessage(sender_id , delay), m_Menu(menu_name),m_Template(template_object),m_Icon(icon_name){}
		  std::string GetMenu() const {return m_Menu;}
		  SceneObjectTemplatePtr GetTemplate() const {return m_Template;}
		  std::string GetIcon() const {return m_Icon;}
	private:
		SceneObjectTemplatePtr m_Template;
		std::string m_Menu;
		std::string m_Icon;
	};
	typedef GASS_SHARED_PTR<CreateResourceRequest> CreateResourceRequestPtr;

	class RemoveResourceRequest : public SystemRequestMessage
	{
	public:
		RemoveResourceRequest(const std::string &template_name, const std::string &menu_name,SenderID sender_id = -1, double delay= 0) : 
		  SystemRequestMessage(sender_id , delay), m_Menu(menu_name), m_Template(template_name){}
		  std::string GetTemplate() const {return m_Template;}
		  std::string GetMenu() const {return m_Menu;}
	private:
		std::string m_Template;
		std::string m_Menu;
	};
	typedef GASS_SHARED_PTR<RemoveResourceRequest> RemoveResourceRequestPtr;

	class AboutMessage : public BaseMessage
	{
	public:
		AboutMessage(SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay) {}
	};
	typedef GASS_SHARED_PTR<AboutMessage> AboutMessagePtr;

	class EditModeChangedEvent : public SystemEventMessage
	{
	public:
		EditModeChangedEvent(GizmoEditMode mode, SenderID sender_id = -1, double delay= 0) : 
		  SystemEventMessage(sender_id , delay), m_Mode(mode){}
		  GizmoEditMode GetEditMode() const {return m_Mode;}
	private:
		GizmoEditMode  m_Mode;
	};
	typedef GASS_SHARED_PTR<EditModeChangedEvent> EditModeChangedEventPtr;

	class TemplateSelectedMessage : public BaseMessage
	{
	public:
		TemplateSelectedMessage(const std::string &name, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Name(name){}
		  std::string GetName() const {return m_Name;}

	private:
		std::string  m_Name;
	};
	typedef GASS_SHARED_PTR<TemplateSelectedMessage> TemplateSelectedMessagePtr;


	class EditPositionEvent : public SceneObjectEventMessage
	{
	public:
		EditPositionEvent(const Vec3 &pos, SenderID sender_id = -1, double delay= 0) :
			SceneObjectEventMessage(sender_id , delay),m_Position(pos)
		  {

		  }
		Vec3 m_Position;
	};
	typedef GASS_SHARED_PTR<EditPositionEvent> EditPositionEventPtr;


	//MFC stuff, use mfc/windows messages instead?
	class ShowSceneObjectMenuRequest : public SceneRequestMessage
	{
	public:
		ShowSceneObjectMenuRequest(const Vec2 &screen_pos, SenderID sender_id = -1, double delay= 0) : 
		  SceneRequestMessage(sender_id , delay), m_Pos (screen_pos){}
		  //SceneObjectPtr GetSceneObject() const {return m_Object;}
		  Vec2 GetScreenPosition() const {return m_Pos;}
	private:
		//SceneObjectPtr m_Object;
		Vec2 m_Pos;
	};
	typedef GASS_SHARED_PTR<ShowSceneObjectMenuRequest> ShowSceneObjectMenuRequestPtr;


	class TextFeedbackRequest : public SystemRequestMessage
	{
	public:
		TextFeedbackRequest(std::string text, SenderID sender_id = -1, double delay= 0) : 
		  SystemRequestMessage(sender_id , delay), m_Text(text){}
		  std::string GetText() const {return m_Text;}
	private:
		std::string m_Text;
	};
	typedef GASS_SHARED_PTR<TextFeedbackRequest> TextFeedbackRequestPtr;


	class ClearFeedbackWindowRequest: public SystemRequestMessage
	{
	public:
		ClearFeedbackWindowRequest(SenderID sender_id = -1, double delay= 0) : 
		  SystemRequestMessage(sender_id , delay) {}
	};
	typedef GASS_SHARED_PTR<ClearFeedbackWindowRequest> ClearFeedbackWindowRequestPtr;


}
#endif 