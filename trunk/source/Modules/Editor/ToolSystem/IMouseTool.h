#pragma once

#include "../EditorCommon.h"
#include "CursorInfo.h"
#include <string>

//State pattern
namespace GASS
{
	class EditorModuleExport IMouseTool
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
