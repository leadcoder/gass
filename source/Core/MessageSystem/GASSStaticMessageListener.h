#ifndef GASS_STATIC_MESSAGE_LISTENER_H
#define GASS_STATIC_MESSAGE_LISTENER_H

#include "Core/MessageSystem/GASSIMessage.h"

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

	class StaticMessageListener : public GASS::IMessageListener
	{
	public:
		StaticMessageListener()
		{
			m_SharedFromThis = GASS_SHARED_PTR<StaticMessageListener>(this,null_deleter());
		}
		virtual ~StaticMessageListener(){}

		GASS_SHARED_PTR<StaticMessageListener> shared_from_this()
		{
			return m_SharedFromThis;
		}

	protected:
		GASS_SHARED_PTR<StaticMessageListener> m_SharedFromThis;
	};
}


#endif 