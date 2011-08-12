/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                 *
*                                                                           *
* Copyright (c) 2008-2009 GASS team. See Contributors.txt for details.      *
*                                                                           *
* GASS is free software: you can redistribute it and/or modify              *
* it under the terms of the GNU Lesser General Public License as published  *
* by the Free Software Foundation, either version 3 of the License, or      *
* (at your option) any later version.                                       *
*                                                                           *
* GASS is distributed in the hope that it will be useful,                   *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU Lesser General Public License for more details.                       *
*                                                                           *
* You should have received a copy of the GNU Lesser General Public License  *
* along with GASS. If not, see <http://www.gnu.org/licenses/>.              *
*****************************************************************************/

#ifndef SYSTEMFACTORY_HH
#define SYSTEMFACTORY_HH

#include "Core/Common.h"
#include "Core/System/ISystem.h"
#include "Core/Utils/Factory.h"

namespace GASS
{

	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup System
	*  @{
	*/

	/**	
		The factory where all systems should be registred in
	*/

	class GASSCoreExport SystemFactory : public Factory<ISystem,std::string>
	{
	public:
		SystemFactory();
		virtual ~SystemFactory();
		static SystemFactory* GetPtr();
		static SystemFactory& Get();
	protected:
		static SystemFactory* m_Instance;
	protected:
	};
}
#endif // #ifndef SYSTEMFACTORY_HH
