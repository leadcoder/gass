#pragma once

#include "../EditorCommon.h"
#include "CursorInfo.h"
#include "Core/MessageSystem/IMessage.h"
#include <boost/enable_shared_from_this.hpp>
#include <string>

//State pattern
namespace GASS
{
	class EditorModuleExport IMouseTool : public boost::enable_shared_from_this<IMouseTool>, public IMessageListener
	{
	public:
		virtual ~IMouseTool(void){}
		virtual void MoveTo(const CursorInfo &info) = 0;
		virtual void MouseDown(const CursorInfo &info) = 0;
		virtual void MouseUp(const CursorInfo &info) = 0;
		virtual std::string GetName() = 0;
		virtual void Stop() = 0;
		virtual void Start() = 0;
	};
}
