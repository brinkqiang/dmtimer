
// Copyright (c) 2018 brinkqiang (brink.qiang@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef __DMQUEUE_H_INCLUDE__
#define __DMQUEUE_H_INCLUDE__

#include <stddef.h>

class CDMQueue {
  public:
    CDMQueue( void )
        : m_pArray( NULL ), m_nHead( 0 ), m_nTail( 0 ), m_nSize( 0 ) {
    }

    ~CDMQueue( void ) {
        delete []m_pArray;
        m_nHead = 0;
        m_nTail = 0;
        m_nSize = 0;
    }

    bool Init( int nSize ) {
        m_nSize = nSize + 1;
        m_pArray = new void* [m_nSize]();
        return true;
    }

    bool PushBack( void* ptr ) {
        int nDist = m_nTail + m_nSize - m_nHead;
        int nUsed = nDist >= m_nSize ? ( nDist - m_nSize ) : nDist;

        if ( nUsed >= m_nSize - 1 ) {
            return false;
        }

        m_pArray[m_nTail] = ptr;

        if ( ++m_nTail >= m_nSize ) {
            m_nTail = 0;
        }

        return true;
    }

    void* PopFront() {
        int nDist = m_nTail + m_nSize - m_nHead;
        int nUsed = nDist >= m_nSize ? ( nDist - m_nSize ) : nDist;

        if ( 0 == nUsed ) {
            return NULL;
        }

        void* ptr = m_pArray[m_nHead];

        if ( ++m_nHead >= m_nSize ) {
            m_nHead = 0;
        }

        return ptr;
    }


  protected:
    void**  m_pArray;
    int   m_nHead;
    int   m_nTail;
    int   m_nSize;
};

#endif // __DMQUEUE_H_INCLUDE__
