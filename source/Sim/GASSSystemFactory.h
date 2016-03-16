/****************************************************************************
* This file is part of GASS.                                                *
* See https://github.com/leadcoder/gass                                     *
*                                                                           *
* Copyright (c) 2008-2016 GASS team. See Contributors.txt for details.      *
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

#include "Sim/GASSCommon.h"
#include "Sim/GASSSimSystem.h"
#include "Core/Utils/GASSFactory.h"

namespace GASS
{

	/** \addtogroup GASSSim
	*  @{
	*/
	/** \addtogroup System
	*  @{
	*/

	/**	
		The factory where all systems should be registred in
	*/

	class GASSExport SystemFactory : public Factory<SimSystem,std::string,void>
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
