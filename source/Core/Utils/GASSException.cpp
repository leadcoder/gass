/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2009 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#include "GASSException.h"
#include "GASSLogger.h"
#include <sstream>

namespace GASS {

    Exception::Exception(int num, const std::string& desc, const std::string& src) :
        m_Line( 0 ),
        m_Number( num ),
        m_Description( desc ),
        m_Source( src )
    {
        
    }

    Exception::Exception(int num, const std::string& desc, const std::string& src, 
		const char* typ, const char* fil, long lin) :
        m_Line( lin ),
        m_Number( num ),
		m_TypeName(typ),
        m_Description( desc ),
        m_Source( src ),
        m_File( fil )
    {
        // Log this error, mask it from debug though since it may be caught and ignored
        /*if(LogManager::getSingletonPtr())
		{
            LogManager::getSingleton().logMessage(
				this->getFullDescription(), 
                LML_CRITICAL, true);
		}*/
		GASS_LOG(LERROR) << this->GetFullDescription();

    }

    Exception::Exception(const Exception& rhs)
        : m_Line( rhs.m_Line ), 
		m_Number( rhs.m_Number ), 
		m_TypeName( rhs.m_TypeName ), 
		m_Description( rhs.m_Description ), 
		m_Source( rhs.m_Source ), 
		m_File( rhs.m_File )
    {
    }

    void Exception::operator = ( const Exception& rhs )
    {
        m_Description = rhs.m_Description;
        m_Number = rhs.m_Number;
        m_Source = rhs.m_Source;
        m_File = rhs.m_File;
        m_Line = rhs.m_Line;
		m_TypeName = rhs.m_TypeName;
    }

    const std::string& Exception::GetFullDescription() const
    {
		if (m_FullDesc.empty())
		{
			std::stringstream desc;
			
			desc <<  "GASS EXCEPTION(" << m_Number << ":" << m_TypeName << "): "
				<< m_Description 
				<< " in " << m_Source;

			if( m_Line > 0 )
			{
				desc << " at " << m_File << " (line " << m_Line << ")";
			}

			m_FullDesc = desc.str();
		}

		return m_FullDesc;
    }

    int Exception::GetNumber() const throw()
    {
        return m_Number;
    }

}

