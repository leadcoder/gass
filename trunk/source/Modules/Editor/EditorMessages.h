#ifndef EDITOR_MESSAGES
#define EDITOR_MESSAGES
#include "Core/MessageSystem/BaseMessage.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Scenario/Scene/SceneObjectTemplate.h"

namespace GASS
{
	class ObjectSelectedMessage : public BaseMessage
	{
	public:
		ObjectSelectedMessage(SceneObjectPtr obj, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Object(obj){}
		  SceneObjectPtr GetSceneObject() const {return m_Object;}
	private:
		SceneObjectPtr m_Object;
	};
	typedef boost::shared_ptr<ObjectSelectedMessage> ObjectSelectedMessagePtr;



	class ObjectLockMessage : public BaseMessage
	{
	public:
		ObjectLockMessage(SceneObjectPtr obj, bool lock, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Object(obj), m_Lock(lock){}
		  SceneObjectPtr GetSceneObject() const {return m_Object;}
		  bool GetLock() const {return m_Lock;}
	private:
		SceneObjectPtr m_Object;
		bool m_Lock;
	};
	typedef boost::shared_ptr<ObjectLockMessage> ObjectLockMessagePtr;


	class ObjectVisibleMessage : public BaseMessage
	{
	public:
		ObjectVisibleMessage(SceneObjectPtr obj, bool visible, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Object(obj), m_Visible(visible){}
		  SceneObjectPtr GetSceneObject() const {return m_Object;}
		  bool GetVisible() const {return m_Visible;}
	private:
		SceneObjectPtr m_Object;
		bool m_Visible;
	};
	typedef boost::shared_ptr<ObjectVisibleMessage > ObjectVisibleMessagePtr;

	class ObjectSiteSelectMessage : public BaseMessage
	{
	public:
		ObjectSiteSelectMessage(SceneObjectPtr obj, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Object(obj) {}
		  SceneObjectPtr GetSceneObject() const {return m_Object;}
	private:
		SceneObjectPtr m_Object;
	};
	typedef boost::shared_ptr<ObjectSiteSelectMessage> ObjectSiteSelectMessagePtr;

	

	/*class GotoPositionMessage : public BaseMessage
	{
	public:
		GotoPositionMessage(const Vec3 &position, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Pos(position){}
		  Vec3 GetPosition() const {return m_Pos;}
	private:
		Vec3 m_Pos;
	};
	typedef boost::shared_ptr<GotoPositionMessage> GotoPositionMessagePtr;*/




	class ZoomObjectMessage : public BaseMessage
	{
	public:
		ZoomObjectMessage(SceneObjectPtr obj, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Object(obj){}
		  SceneObjectPtr GetSceneObject() const {return m_Object;}
	private:
		SceneObjectPtr m_Object;
	};
	typedef boost::shared_ptr<ZoomObjectMessage> ZoomObjectMessagePtr;



	class ShowObjectMenuMessage : public BaseMessage
	{
	public:
		ShowObjectMenuMessage(SceneObjectPtr obj, const Vec2 &screen_pos, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Object(obj), m_Pos (screen_pos){}
		  SceneObjectPtr GetSceneObject() const {return m_Object;}
		  Vec2 GetScreenPosition() const {return m_Pos;}
	private:
		SceneObjectPtr m_Object;
		Vec2 m_Pos;
	};
	typedef boost::shared_ptr<ShowObjectMenuMessage> ShowObjectMenuMessagePtr;

	class ObjectDroppedMessage : public BaseMessage
	{
	public:
		ObjectDroppedMessage(const std::string &obj_name, const Vec2 &screen_pos, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_ObjectName(obj_name), m_ScreenPosition(screen_pos) {}
		  std::string GetObjectName() const {return m_ObjectName;}
		  Vec2 GetScreenPosition() const {return m_ScreenPosition;}
	private:
		std::string m_ObjectName;
		Vec2 m_ScreenPosition;
	};
	typedef boost::shared_ptr<ObjectDroppedMessage> ObjectDroppedMessagePtr;


	class CursorMoved2DMessage : public BaseMessage
	{
	public:
		CursorMoved2DMessage(const Vec2 &pos, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_ScreenPosition(pos){}
		  Vec2 GetScreenPosition() const {return m_ScreenPosition;}
	private:
		Vec2 m_ScreenPosition;
	};
	typedef boost::shared_ptr<CursorMoved2DMessage> CursorMoved2DMessagePtr;


	class CursorMoved3DMessage : public BaseMessage
	{
	public:
		CursorMoved3DMessage(const Vec2 &screen_pos,const Vec3 &pos3d, SceneObjectPtr obj, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_ScreenPosition(screen_pos), m_3DPosition(pos3d), m_Object(obj){}
		  SceneObjectPtr GetSceneObjectUnderCursor() const {return m_Object;}
		  Vec2 GetScreenPosition() const {return m_ScreenPosition;}
		  Vec3 Get3DPosition() const {return m_3DPosition;}
	private:
		Vec2 m_ScreenPosition;
		Vec3 m_3DPosition;
		SceneObjectPtr m_Object;
	};
	typedef boost::shared_ptr<CursorMoved3DMessage> CursorMoved3DMessagePtr;



	class PaintMessage : public BaseMessage
	{
	public:
		PaintMessage(const Vec3 &pos, SceneObjectPtr obj, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Position(pos), m_Object(obj){}
		  SceneObjectPtr GetObjectUnderCursor() const {return m_Object;}
		  Vec3 GetPosition() const {return m_Position;}
	private:
		Vec3 m_Position;
		SceneObjectPtr m_Object;
	};
	typedef boost::shared_ptr<PaintMessage> PaintMessagePtr;

	class ToolChangedMessage : public BaseMessage
	{
	public:
		ToolChangedMessage(const std::string &tool,  SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), 
			  m_Tool(tool)
		  {

		  }

		  void SetCreateObjectName(const std::string &object_name) 
		  {
			  m_ObjectName =object_name;
		  }
		  std::string GetCreateObjectName() const 
		  {
			  return m_ObjectName;
		  }
		  void SetCreateParentObject(GASS::SceneObjectPtr obj) 
		  {
			  m_ParentObject = obj;
		  }
		  GASS::SceneObjectPtr GetCreateParentObject() const 
		  {
			  return m_ParentObject;
		  }

		  std::string GetTool() const {return m_Tool;}
	private:
		std::string  m_Tool;
		std::string m_ObjectName;
		GASS::SceneObjectPtr m_ParentObject; 
	};
	typedef boost::shared_ptr<ToolChangedMessage> ToolChangedMessagePtr;



	class WindowFocusChangedMessage : public BaseMessage
	{
	public:
		WindowFocusChangedMessage(const std::string &window, bool focus,SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Window(window), m_Focus(focus){}
		  std::string GetWindow() const {return m_Window;}
		  bool GetFocus() const {return m_Focus;}
	private:
		std::string  m_Window;
		bool m_Focus;
	};
	typedef boost::shared_ptr<WindowFocusChangedMessage> WindowFocusChangedMessagePtr;

	class FileSaveMessage : public BaseMessage
	{
	public:
		FileSaveMessage(SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay){}
	private:
	};
	typedef boost::shared_ptr<FileSaveMessage> FileSaveMessagePtr;


	/**
	Notify that file is saved, sent by application/plugin that listen to FileSaveMessage messages
	*/

	class FileSavedMessage : public BaseMessage
	{
	public:
		FileSavedMessage(const std::string &filename, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Filename(filename){}
		  std::string GetFilename() const {return m_Filename;}
	private:
		std::string m_Filename;
	};
	typedef boost::shared_ptr<FileSavedMessage> FileSavedMessagePtr;



	class FileOpenMessage : public BaseMessage
	{
	public:
		FileOpenMessage(SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay){}
	private:
	};
	typedef boost::shared_ptr<FileOpenMessage> FileOpenMessagePtr;

	class FileOpenedMessage : public BaseMessage
	{
	public:
		FileOpenedMessage(const std::string &filename, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Filename(filename){}
		  std::string GetFilename() const {return m_Filename;}
	private:
		std::string m_Filename;
	};
	typedef boost::shared_ptr<FileOpenedMessage> FileOpenedMessagePtr;


	class FileNewMessage : public BaseMessage
	{
	public:
		FileNewMessage(SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay){}
	private:
	};
	typedef boost::shared_ptr<FileNewMessage> FileNewMessagePtr;



	class ScenarioChangedMessage : public BaseMessage
	{
	public:
		ScenarioChangedMessage(SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay){}
	private:
	};
	typedef boost::shared_ptr<ScenarioChangedMessage> ScenarioChangedMessagePtr;



	class NewWindowMessage : public BaseMessage
	{
	public:
		NewWindowMessage(const std::string &window_name, int id,SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_WindowName(window_name), m_ID(id){}
		  std::string GetWindowName() const {return m_WindowName;}
		  int GetId() const {return m_ID;}
	private:
		std::string  m_WindowName;
		int m_ID;
	};
	typedef boost::shared_ptr<NewWindowMessage> NewWindowMessagePtr;

	class RemoveWindowMessage : public BaseMessage
	{
	public:
		RemoveWindowMessage(const std::string &window_name, int id,SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_WindowName(window_name), m_ID(id){}
		  std::string GetWindowName() const {return m_WindowName;}
		  int GetId() const {return m_ID;}
	private:
		std::string  m_WindowName;
		int m_ID;
	};
	typedef boost::shared_ptr<RemoveWindowMessage> RemoveWindowMessagePtr;


	class LoadLayoutMessage : public BaseMessage
	{
	public:
		LoadLayoutMessage(const std::string &filename, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Filename(filename){}
		  std::string GetFilename() const {return m_Filename;}

	private:
		std::string  m_Filename;
	};
	typedef boost::shared_ptr<LoadLayoutMessage> LoadLayoutMessagePtr;



	class MenuCommandMessage : public BaseMessage
	{
	public:
		MenuCommandMessage(int id,SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_ID(id){}
		  int GetId() const {return m_ID;}
	private:
		int m_ID;
	};
	typedef boost::shared_ptr<MenuCommandMessage> MenuCommandMessagePtr;


	class MouseButtonMessage : public BaseMessage
	{
	public:
		enum MouseButtonId
		{
			MBID_LEFT,
			MBID_RIGHT,
			MBID_MIDDLE,
		};
		MouseButtonMessage(MouseButtonId button, bool down, SenderID sender_id = -1, double delay= 0) : 
			BaseMessage(sender_id , delay), m_ID(button), m_Down(down){}
			MouseButtonId GetButton() const {return m_ID;}
			int IsDown() const {return m_Down;}
	private:
		MouseButtonId m_ID;
		bool m_Down;
	};
	typedef boost::shared_ptr<MouseButtonMessage> MouseButtonMessagePtr;

	class ObjectChangedMessage : public BaseMessage
	{
	public:
		ObjectChangedMessage(BaseReflectionObjectPtr obj, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Object(obj){}
		  BaseReflectionObjectPtr GetObject() const {return m_Object;}
	private:
		BaseReflectionObjectPtr m_Object;
	};
	typedef boost::shared_ptr<ObjectChangedMessage> ObjectChangedMessagePtr;

	class ObjectAttributeChangedMessage : public BaseMessage
	{
	public:
		ObjectAttributeChangedMessage(BaseReflectionObjectPtr obj, const std::vector<std::string> &attribs, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Object(obj), m_Attributes(attribs){}
		  BaseReflectionObjectPtr GetObject() const {return m_Object;}
		  std::vector<std::string> GetAttributes() const {return m_Attributes;}
	private:
		BaseReflectionObjectPtr m_Object;
		std::vector<std::string> m_Attributes;
	};
	typedef boost::shared_ptr<ObjectAttributeChangedMessage> ObjectAttributeChangedMessagePtr;


	class TextFeedbackMessage : public BaseMessage
	{
	public:
		TextFeedbackMessage(std::string text, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Text(text){}
		  std::string GetText() const {return m_Text;}
	private:
		std::string m_Text;
	};
	typedef boost::shared_ptr<TextFeedbackMessage> TextFeedbackMessagePtr;

	class ClearFeedbackWindowMessage : public BaseMessage
	{
	public:
		ClearFeedbackWindowMessage(SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay) {}
	};
	typedef boost::shared_ptr<ClearFeedbackWindowMessage> ClearFeedbackWindowMessagePtr;


	class LayoutLoadedMessage : public BaseMessage
	{
	public:
		LayoutLoadedMessage(SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay){}
	private:
	};
	typedef boost::shared_ptr<LayoutLoadedMessage> LayoutLoadedMessagePtr;



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
	typedef boost::shared_ptr<CreateResourceMessage> CreateResourceMessagePtr;


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
	typedef boost::shared_ptr<RemoveResourceMessage> RemoveResourceMessagePtr;

	class AboutMessage : public BaseMessage
	{
	public:
		AboutMessage(SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay) {}
	};
	typedef boost::shared_ptr<AboutMessage> AboutMessagePtr;



	class NewGASSScenarioMessage : public BaseMessage
	{
	public:
		NewGASSScenarioMessage(const std::string &filename, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Filename(filename){}
		  std::string GetScenarioName() const {return m_Filename;}
	private:
		std::string  m_Filename;
	};
	typedef boost::shared_ptr<NewGASSScenarioMessage> NewGASSScenarioMessagePtr;

	class LoadGASSScenarioMessage : public BaseMessage
	{
	public:
		LoadGASSScenarioMessage(const std::string &filename, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Filename(filename){}
		  std::string GetScenarioName() const {return m_Filename;}
	private:
		std::string  m_Filename;
	};
	typedef boost::shared_ptr<LoadGASSScenarioMessage> LoadGASSScenarioMessagePtr;


	class SaveGASSScenarioMessage : public BaseMessage
	{
	public:
		SaveGASSScenarioMessage(const std::string &filename, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Filename(filename){}
		  std::string GetScenarioName() const {return m_Filename;}
	private:
		std::string  m_Filename;
	};
	typedef boost::shared_ptr<SaveGASSScenarioMessage> SaveGASSScenarioMessagePtr;

	class EditModeMessage : public BaseMessage
	{
	public:
		EditModeMessage(const std::string &mode, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Mode(mode){}
		  std::string GetEditMode() const {return m_Mode;}
	private:
		std::string  m_Mode;
	};
	typedef boost::shared_ptr<EditModeMessage> EditModeMessagePtr;


	class SnapSettingsMessage : public BaseMessage
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
	typedef boost::shared_ptr<SnapSettingsMessage> SnapSettingsMessagePtr;

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
	typedef boost::shared_ptr<SnapModeMessage> SnapModeMessagePtr;



	class GridMessage : public BaseMessage
	{
	public:
		GridMessage(Float size, Float spacing, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Size(spacing), m_Spacing(spacing){}
		  Float GetSpacing() const {return m_Spacing;}
		  Float GetSize() const {return m_Size;}
	private:
		Float m_Spacing;
		Float m_Size;
	};
	typedef boost::shared_ptr<GridMessage> GridMessagePtr;


	class TemplateSelectedMessage : public BaseMessage
	{
	public:
		TemplateSelectedMessage(const std::string &name, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Name(name){}
		  std::string GetName() const {return m_Name;}

	private:
		std::string  m_Name;
	};
	typedef boost::shared_ptr<TemplateSelectedMessage> TemplateSelectedMessagePtr;

}
#endif 