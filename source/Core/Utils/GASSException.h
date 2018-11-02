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

//This file is from the Ogre3D project, only namespace and
//some syntax changes are different from the version
//provided with Ogre3D

#ifndef GASS_EXCEPTION_H
#define GASS_EXCEPTION_H


#include "Core/Common.h"
#include <string>
#include <exception>
// Precompiler options

// Backwards compatibility with old assert mode definitions
#if GASS_RELEASE_ASSERT == 1
#   define GASS_ASSERT_MODE 1
#endif

// Check for GASS assert mode

// RELEASE_EXCEPTIONS mode
#if GASS_ASSERT_MODE == 1
#   ifdef _DEBUG
#       define GASSAssert( a, b ) assert( (a) && (b) )
#   else
#       define GASSAssert( a, b ) if( !(a) ) GASS_EXCEPT( GASS::Exception::ERR_RT_ASSERTION_FAILED, (b), "no function info")
#   endif
// EXCEPTIONS mode
#elif GASS_ASSERT_MODE == 2
#       define GASSAssert( a, b ) if( !(a) ) GASS_EXCEPT( Ogre::Exception::ERR_RT_ASSERTION_FAILED, (b), "no function info")

// STANDARD mode
#else
#   define GASSAssert( a, b ) assert( (a) && (b) )
#endif

namespace GASS {
	/** \addtogroup Core
	*  @{
	*/
	/** \addtogroup Utils
	*  @{
	*/
	/** When thrown, provides information about an error that has occurred inside the engine.
        @remarks
            GASS never uses return values to indicate errors. Instead, if an
            error occurs, an exception is thrown, and this is the object that
            encapsulates the detail of the problem. The application using
            GASS should always ensure that the exceptions are caught, so all
            GASS engine functions should occur within a
            try{} catch(GASS::Exception& e) {} block.
        @par
            The user application should never create any instances of this
            object unless it wishes to unify its error handling using the
            same object.
    */
	class GASSCoreExport Exception : public std::exception
    {
    protected:
        long line;
        int number;
		std::string typeName;
        std::string description;
        std::string source;
        std::string file;
		mutable std::string fullDesc;
    public:
        /** Static definitions of error codes.
            @todo
                Add many more exception codes, since we want the user to be able
                to catch most of them.
        */
        enum ExceptionCodes {
            ERR_CANNOT_WRITE_TO_FILE,
            ERR_INVALID_STATE,
            ERR_INVALIDPARAMS,
            ERR_DUPLICATE_ITEM,
            ERR_ITEM_NOT_FOUND,
            ERR_FILE_NOT_FOUND,
			ERR_CANNOT_READ_FILE,
            ERR_INTERNAL_ERROR,
            ERR_RT_ASSERTION_FAILED, 
			ERR_NOT_IMPLEMENTED
        };

        /** Default constructor.
        */
        Exception( int number, const std::string & description, const std::string & source );

        /** Advanced constructor.
        */
        Exception( int number, const std::string & description, const std::string & source, const char* type, const char* file, long line );

        /** Copy constructor.
        */
        Exception(const Exception& rhs);

		/// Needed for  compatibility with std::exception
		~Exception() throw() override {}

        /** Assignment operator.
        */
        void operator = (const Exception& rhs);

        /** Returns a string with the full description of this error.
            @remarks
                The description contains the error number, the description
                supplied by the thrower, what routine threw the exception,
                and will also supply extra platform-specific information
                where applicable.
        */
        virtual const std::string & getFullDescription() const;

        /** Gets the error code.
        */
        virtual int getNumber() const throw();

        /** Gets the source function.
        */
        virtual const std::string &getSource() const { return source; }

        /** Gets source file name.
        */
        virtual const std::string &getFile() const { return file; }

        /** Gets line number.
        */
        virtual long getLine() const { return line; }

		/** Returns a string with only the 'description' field of this exception. Use 
			getFullDescriptionto get a full description of the error including line number,
			error number and what function threw the exception.
        */
		virtual const std::string &getDescription() const { return description; }

		/// Override std::exception::what
		const char* what() const throw() override { return getFullDescription().c_str(); }
        
    };


	/** Template struct which creates a distinct type for each exception code.
	@note
	This is useful because it allows us to create an overloaded method
	for returning different exception types by value without ambiguity. 
	From 'Modern C++ Design' (Alexandrescu 2001).
	*/
	template <int num>
	struct ExceptionCodeType
	{
		enum { number = num };
	};

	// Specialised exceptions allowing each to be caught specifically
	// backwards-compatible since exception codes still used

	class GASSCoreExport UnimplementedException : public Exception 
	{
	public:
		UnimplementedException(int inNumber, const std::string & inDescription, const std::string & inSource, const char* inFile, long inLine)
			: Exception(inNumber, inDescription, inSource, "UnimplementedException", inFile, inLine) {}
	};
	
	class GASSCoreExport FileNotFoundException : public Exception
	{
	public:
		FileNotFoundException(int inNumber, const std::string & inDescription, const std::string & inSource, const char* inFile, long inLine)
			: Exception(inNumber, inDescription, inSource, "FileNotFoundException", inFile, inLine) {}
	};

	class GASSCoreExport FileReadException : public Exception
	{
	public:
		FileReadException(int inNumber, const std::string & inDescription, const std::string & inSource, const char* inFile, long inLine)
			: Exception(inNumber, inDescription, inSource, "FileReadException", inFile, inLine) {}
	};

	class GASSCoreExport IOException : public Exception
	{
	public:
		IOException(int inNumber, const std::string & inDescription, const std::string & inSource, const char* inFile, long inLine)
			: Exception(inNumber, inDescription, inSource, "IOException", inFile, inLine) {}
	};
	class GASSCoreExport InvalidStateException : public Exception
	{
	public:
		InvalidStateException(int inNumber, const std::string & inDescription, const std::string & inSource, const char* inFile, long inLine)
			: Exception(inNumber, inDescription, inSource, "InvalidStateException", inFile, inLine) {}
	};
	class GASSCoreExport InvalidParametersException : public Exception
	{
	public:
		InvalidParametersException(int inNumber, const std::string & inDescription, const std::string & inSource, const char* inFile, long inLine)
			: Exception(inNumber, inDescription, inSource, "InvalidParametersException", inFile, inLine) {}
	};
	class GASSCoreExport ItemIdentityException : public Exception
	{
	public:
		ItemIdentityException(int inNumber, const std::string & inDescription, const std::string & inSource, const char* inFile, long inLine)
			: Exception(inNumber, inDescription, inSource, "ItemIdentityException", inFile, inLine) {}
	};
	class GASSCoreExport InternalErrorException : public Exception
	{
	public:
		InternalErrorException(int inNumber, const std::string & inDescription, const std::string & inSource, const char* inFile, long inLine)
			: Exception(inNumber, inDescription, inSource, "InternalErrorException", inFile, inLine) {}
	};

	class GASSCoreExport RuntimeAssertionException : public Exception
	{
	public:
		RuntimeAssertionException(int inNumber, const std::string & inDescription, const std::string & inSource, const char* inFile, long inLine)
			: Exception(inNumber, inDescription, inSource, "RuntimeAssertionException", inFile, inLine) {}
	};


	/** Class implementing dispatch methods in order to construct by-value
		exceptions of a derived type based just on an exception code.
	@remarks
		This nicely handles construction of derived Exceptions by value (needed
		for throwing) without suffering from ambiguity - each code is turned into
		a distinct type so that methods can be overloaded. This allows GASS_EXCEPT
		to stay small in implementation (desirable since it is embedded) whilst
		still performing rich code-to-type mapping. 
	*/
	class ExceptionFactory
	{
	private:
		/// Private constructor, no construction
		ExceptionFactory() {}
	public:
		static UnimplementedException create(
			ExceptionCodeType<Exception::ERR_NOT_IMPLEMENTED> code, 
			const std::string & desc, 
			const std::string & src, const char* file, long line)
		{
			(void) code;
			return UnimplementedException(code.number, desc, src, file, line);
		}
		static FileNotFoundException create(
			ExceptionCodeType<Exception::ERR_FILE_NOT_FOUND> code, 
			const std::string & desc, 
			const std::string & src, const char* file, long line)
		{
			(void) code;
			return FileNotFoundException(code.number, desc, src, file, line);
		}
		static FileReadException create(
			ExceptionCodeType<Exception::ERR_CANNOT_READ_FILE> code, 
			const std::string & desc, 
			const std::string & src, const char* file, long line)
		{
			(void) code;
			return FileReadException(code.number, desc, src, file, line);
		}

		static IOException create(
			ExceptionCodeType<Exception::ERR_CANNOT_WRITE_TO_FILE> code, 
			const std::string & desc, 
			const std::string & src, const char* file, long line)
		{
			(void) code;
			return IOException(code.number, desc, src, file, line);
		}
		static InvalidStateException create(
			ExceptionCodeType<Exception::ERR_INVALID_STATE> code, 
			const std::string & desc, 
			const std::string & src, const char* file, long line)
		{
			(void) code;
			return InvalidStateException(code.number, desc, src, file, line);
		}
		static InvalidParametersException create(
			ExceptionCodeType<Exception::ERR_INVALIDPARAMS> code, 
			const std::string & desc, 
			const std::string & src, const char* file, long line)
		{
			(void) code;
			return InvalidParametersException(code.number, desc, src, file, line);
		}
		static ItemIdentityException create(
			ExceptionCodeType<Exception::ERR_ITEM_NOT_FOUND> code, 
			const std::string & desc, 
			const std::string & src, const char* file, long line)
		{
			(void) code;
			return ItemIdentityException(code.number, desc, src, file, line);
		}
		static ItemIdentityException create(
			ExceptionCodeType<Exception::ERR_DUPLICATE_ITEM> code, 
			const std::string & desc, 
			const std::string & src, const char* file, long line)
		{
			(void) code;
			return ItemIdentityException(code.number, desc, src, file, line);
		}
		static InternalErrorException create(
			ExceptionCodeType<Exception::ERR_INTERNAL_ERROR> code, 
			const std::string & desc, 
			const std::string & src, const char* file, long line)
		{
			(void) code;
			return InternalErrorException(code.number, desc, src, file, line);
		}
		static RuntimeAssertionException create(
			ExceptionCodeType<Exception::ERR_RT_ASSERTION_FAILED> code, 
			const std::string & desc, 
			const std::string & src, const char* file, long line)
		{
			(void) code;
			return RuntimeAssertionException(code.number, desc, src, file, line);
		}

	};
	

	
#ifndef GASS_EXCEPT
#define GASS_EXCEPT(num, desc, src) throw GASS::ExceptionFactory::create( \
	GASS::ExceptionCodeType<num>(), desc, src, __FILE__, __LINE__ );
#endif
	/** @} */
	/** @} */

} // Namespace GASS

#endif
