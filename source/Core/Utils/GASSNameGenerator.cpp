#include "GASSNameGenerator.h"

namespace GASS
{
	NameGenerator::NameGenerator(bool skip_id_for_first, std::string prefix, std::string suffix) : m_SkipIdForFirst(skip_id_for_first),
		m_Prefix(prefix),
		m_Suffix(suffix)
	{

	}

	std::string NameGenerator::RemoveIDFromName(const std::string& name) const
	{
		auto pos = name.find(m_Prefix);
		if (pos != std::string::npos)
			return name.substr(0, pos);

		pos = name.find("[");
		if (pos != std::string::npos)
			return name.substr(0, pos);

		return name;
	}

	int NameGenerator::GetNameCount(const std::string& name) const
	{
		auto iter = m_Names.find(name);
		if (iter != m_Names.end())
		{
			return iter->second;
		}
		return 0;
	}

	int FindIDInName(const std::string& name)
	{
		int num = 0;
		std::size_t const n = name.find_last_of("0123456789");
		if (n != std::string::npos)
		{
			std::size_t const m = name.find_last_not_of("0123456789", n);
			const auto num_str = m != std::string::npos ? name.substr(m + 1, n - m) : name.substr(0, n + 1);
			num = std::stoi(num_str);
		}
		return num;
	}

	void NameGenerator::Clear(const std::string& name)
	{
		auto iter = m_Names.find(name);
		if (iter != m_Names.end())
		{
			m_Names.erase(iter);
		}
	}

	/*void NameGenerator::Free(const std::string& name)
	{
		const std::string stripped_name = RemoveIDFromName(name);
		auto iter = m_Names.find(stripped_name);
		if (iter != m_Names.end())
		{
			auto num = FindIDInName(name);
			if(iter->second >= num)
				iter->second--;
		}
	}*/

	std::string NameGenerator::CreateUniqueName(const std::string& name) const
	{
		const std::string stripped_name = RemoveIDFromName(name);
		auto iter = m_Names.find(stripped_name);
		std::string final_name;
		if (iter != m_Names.end())
		{
			const auto count = iter->second + 1;
			m_Names[stripped_name] = count;
			final_name = stripped_name + m_Prefix + std::to_string(count) + m_Suffix;
		}
		else
		{
			if (m_SkipIdForFirst)
			{
				final_name = stripped_name + m_Prefix + "1" + m_Suffix;
			}
			else
				final_name = stripped_name;
			m_Names[stripped_name] = 1;
		}
		return final_name;
	}
}
