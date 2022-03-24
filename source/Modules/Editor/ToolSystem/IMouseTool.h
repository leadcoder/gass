#pragma once
#include "Sim/GASSCommon.h"
#include "../EditorCommon.h"
#include "CursorInfo.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/Interface/GASSIInputSystem.h"
#include <string>

//State pattern
namespace GASS
{
	struct message_null_deleter
	{
		void operator()(void const *) const
		{
		}
	};

	class EditorModuleExport IMouseTool : public IMessageListener
	{
	public:
		IMouseTool()
		{
			m_SharedFromThis = GASS_SHARED_PTR<IMouseTool>(this,message_null_deleter());
		}
		virtual ~IMouseTool(void){}
		virtual void MouseMoved(const MouseData &data, const SceneCursorInfo &info) = 0;
		virtual void MouseDown(const MouseData &data, const SceneCursorInfo &info) = 0;
		virtual void MouseUp(const MouseData &data, const SceneCursorInfo &info) = 0;
		virtual void Update(double delta_time) {(void)delta_time;}
		virtual std::string GetName() = 0;
		virtual void Stop() = 0;
		virtual void Start() = 0;

		GASS_SHARED_PTR<IMouseTool> shared_from_this() const
		{
			return m_SharedFromThis;
		}

	protected:
		GASS_SHARED_PTR<IMouseTool> m_SharedFromThis;
	};
}
