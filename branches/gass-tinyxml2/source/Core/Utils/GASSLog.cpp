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

#include "GASSLog.h"
#include "GASSLogManager.h"
#include <iostream>
#include <iomanip>

namespace GASS
{

    //-----------------------------------------------------------------------
    Log::Log( const std::string& name, bool debuggerOuput, bool suppressFile ) : 
        mLogLevel(LL_NORMAL), mDebugOut(debuggerOuput),
        mSuppressFile(suppressFile), mTimeStamp(true), mLogName(name)
    {
		if (!mSuppressFile)
		{
			mfpLog.open(name.c_str());
		}
    }
    //-----------------------------------------------------------------------
    Log::~Log()
    {
		if (!mSuppressFile)
		{
	        mfpLog.close();
		}
    }
    //-----------------------------------------------------------------------
    void Log::logMessage( const std::string& message, LogMessageLevel lml, bool maskDebug )
    {
        if ((mLogLevel + lml) >= GASS_LOG_THRESHOLD)
        {
            for( mtLogListener::iterator i = mListeners.begin(); i != mListeners.end(); ++i )
                (*i)->messageLogged( message, lml, maskDebug, mLogName );

			if (mDebugOut && !maskDebug)
                std::cerr << message << std::endl;

            // Write time into log
			if (!mSuppressFile)
			{
				if (mTimeStamp)
			    {
                    struct tm *pTime;
                    time_t ctTime; time(&ctTime);
                    pTime = localtime( &ctTime );
                    mfpLog << std::setw(2) << std::setfill('0') << pTime->tm_hour
                        << ":" << std::setw(2) << std::setfill('0') << pTime->tm_min
                        << ":" << std::setw(2) << std::setfill('0') << pTime->tm_sec
                        << ": ";
                }
                mfpLog << message << std::endl;

				// Flush stcmdream to ensure it is written (incase of a crash, we need log to be up to date)
				mfpLog.flush();
			}
        }
    }
    
    //-----------------------------------------------------------------------
    void Log::setTimeStampEnabled(bool timeStamp)
    {
        mTimeStamp = timeStamp;
    }

    //-----------------------------------------------------------------------
    void Log::setDebugOutputEnabled(bool debugOutput)
    {
        mDebugOut = debugOutput;
    }

	//-----------------------------------------------------------------------
    void Log::setLogDetail(LoggingLevel ll)
    {
        mLogLevel = ll;
    }

    //-----------------------------------------------------------------------
    void Log::addListener(LogListener* listener)
    {
        mListeners.push_back(listener);
    }

    //-----------------------------------------------------------------------
    void Log::removeListener(LogListener* listener)
    {
        mListeners.erase(std::find(mListeners.begin(), mListeners.end(), listener));
    }
	//---------------------------------------------------------------------
	Log::Stream Log::stream(LogMessageLevel lml, bool maskDebug) 
	{
		return Stream(this, lml, maskDebug);

	}
}
