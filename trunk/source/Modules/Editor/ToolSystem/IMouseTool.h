#pragma once

#include "../EditorCommon.h"
#include "CursorInfo.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include <boost/enable_shared_from_this.hpp>
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
			m_SharedFromThis = boost::shared_ptr<IMouseTool>(this,message_null_deleter());
		}
		virtual ~IMouseTool(void){}
		virtual void MouseMoved(const CursorInfo &info) = 0;
		virtual void MouseDown(const CursorInfo &info) = 0;
		virtual void MouseUp(const CursorInfo &info) = 0;
		//virtual void Update(double delta_time) = 0;
		virtual std::string GetName() = 0;
		virtual void Stop() = 0;
		virtual void Start() = 0;

		boost::shared_ptr<IMouseTool> shared_from_this()
		{
			return m_SharedFromThis;
		}

	protected:
		boost::shared_ptr<IMouseTool> m_SharedFromThis;
	};
}
