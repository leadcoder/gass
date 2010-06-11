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

#ifndef ITERATORS_H
#define ITERATORS_H

namespace GASS {

    /** Wraps iteration over a vector.
    @remarks
        This class is here just to allow clients to iterate over an internal
        vector of a class without having to have access to the vector itself
        (typically to iterate you need both the iterator and the end() iterator
        to test for the end condition, which is messy).
        No updates are allowed through this interface, it is purely for 
        iterating and reading.
    @par
        Note that like STL iterators, these iterators are only valid whilst no 
        updates are made to the underlying collection. You should not attempt to
        use this iterator if a change is made to the collection. In fact, treat this
        iterator as a transient object, do NOT store it and try to use it repeatedly.
    */
    template <class T>
    class VectorIterator
    {
    private:
        typename T::iterator mCurrent;
        typename T::iterator mEnd;
		/// Private constructor since only the parameterised constructor should be used
        VectorIterator() {};
    public:
        typedef typename T::value_type ValueType;

        /** Constructor.
        @remarks
            Provide a start and end iterator to initialise.
        */
        VectorIterator(typename T::iterator start, typename T::iterator end)
            : mCurrent(start), mEnd(end)
        {
        }

        /** Constructor.
        @remarks
            Provide a container to initialise.
        */
        explicit VectorIterator(T& c)
            : mCurrent(c.begin()), mEnd(c.end())
        {
        }

        /** Returns true if there are more items in the collection. */
        bool hasMoreElements(void) const
        {
            return mCurrent != mEnd;
        }

        /** Returns the next element in the collection, and advances to the next. */
        typename T::value_type getNext(void)
        {
            return *mCurrent++;
        }
        /** Returns the next element in the collection, without advancing to the next. */
        typename T::value_type peekNext(void)
        {
            return *mCurrent;
        }
        /** Returns a pointer to the next element in the collection, without advancing to the next afterwards. */
        typename T::pointer peekNextPtr(void)
        {
            return &(*mCurrent);
        }
        /** Moves the iterator on one element. */
        void moveNext(void)
        {
            ++mCurrent;
        }
    };
}
#endif