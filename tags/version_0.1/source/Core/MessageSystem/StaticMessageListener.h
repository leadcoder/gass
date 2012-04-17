#ifndef StaticMessageListener_HH
#define StaticMessageListener_HH

#include "Core/MessageSystem/IMessage.h"

namespace GASS
{

	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Message
	*  @{
	*/


	struct null_deleter
	{
		void operator()(void const *) const
		{
		}
	};

	/**
	Convenient class that static allocated suscribers can derive from
	The message registration macro assume that the boost shared_from_this method 
	is present, this class fake that with the null_deleter
	*/

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