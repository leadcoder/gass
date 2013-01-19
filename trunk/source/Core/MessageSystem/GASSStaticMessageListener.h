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

	class GASSCoreExport StaticMessageListener : public GASS::IMessageListener
	{
	public:
		StaticMessageListener()
		{
			m_SharedFromThis = SPTR<StaticMessageListener>(this,null_deleter());
		}
		virtual ~StaticMessageListener(){}

		SPTR<StaticMessageListener> shared_from_this()
		{
			return m_SharedFromThis;
		}

	protected:
		SPTR<StaticMessageListener> m_SharedFromThis;
	};
}


#endif 