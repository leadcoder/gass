#ifndef StaticMessageListener_HH
#define StaticMessageListener_HH

#include "Core/MessageSystem/IMessage.h"

namespace GASS
{

	struct null_deleter
	{
		void operator()(void const *) const
		{
		}
	};


	class GASSCoreExport StaticMessageListener : public GASS::IMessageListener
	{
	public:
		StaticMessageListener()
		{
			m_SharedFromThis = boost::shared_ptr<StaticMessageListener>(this,null_deleter());
		}
		virtual ~StaticMessageListener(){}

		boost::shared_ptr<StaticMessageListener> shared_from_this()
		{
			return m_SharedFromThis;
		}

	protected:
		boost::shared_ptr<StaticMessageListener> m_SharedFromThis;
	};
}


#endif 