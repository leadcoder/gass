#pragma once
#include "Core/Common.h"
#include <string>

namespace GASS
{
	class GASSCoreExport NameGenerator
	{
	public:
		NameGenerator(bool add_number_to_first= false, std::string prefix = " (", std::string suffix = ")");
		std::string CreateUniqueName(const std::string& name) const;
		void Clear() {m_Names.clear();}
		void Clear(const std::string& name);
		std::string RemoveIDFromName(const std::string& name) const;
		int GetNameCount(const std::string& name) const;
	private:
		bool m_SkipIdForFirst = false;
		mutable std::map<std::string, int> m_Names;
		std::string m_Prefix = " (";
		std::string m_Suffix = ")";
	};
}
