/***********************************************************************
	created:	13/2/2011
	author:		Martin Preisler (reworked from code by Paul D Turner)
	
	purpose:	Defines interfaces for UVector classes
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2015 Paul D Turner & The CEGUI Development Team
 *
 *   Permission is hereby granted, free of charge, to any person obtaining
 *   a copy of this software and associated documentation files (the
 *   "Software"), to deal in the Software without restriction, including
 *   without limitation the rights to use, copy, modify, merge, publish,
 *   distribute, sublicense, and/or sell copies of the Software, and to
 *   permit persons to whom the Software is furnished to do so, subject to
 *   the following conditions:
 *
 *   The above copyright notice and this permission notice shall be
 *   included in all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *   OTHER DEALINGS IN THE SOFTWARE.
 ***************************************************************************/
#ifndef _CEGUIUVector_h_
#define _CEGUIUVector_h_

#include "CEGUI/UDim.h"
#include "CEGUI/StreamHelper.h"

#include <typeinfo>
#include <ostream>

// Start of CEGUI namespace section
namespace CEGUI
{

/*!
\brief
    Class used as a two dimensional vector (aka a Point)

\par
    A Vector class featuring two UDims as elements.
*/
class UVector2
{
public:
    inline UVector2()
    {}
    
    inline UVector2(const UDim x, const UDim y):
        d_x(x),
        d_y(y)
    {}

    inline UVector2(const UVector2& v):
        d_x(v.d_x),
        d_y(v.d_y)
    {}

    inline UVector2& operator*=(const UVector2& vec)
    {
        d_x *= vec.d_x;
        d_y *= vec.d_y;

        return *this;
    }

    inline UVector2& operator/=(const UVector2& vec)
    {
        d_x /= vec.d_x;
        d_y /= vec.d_y;

        return *this;
    }

    inline UVector2& operator+=(const UVector2& vec)
    {
        d_x += vec.d_x;
        d_y += vec.d_y;

        return *this;
    }

    inline UVector2& operator-=(const UVector2& vec)
    {
        d_x -= vec.d_x;
        d_y -= vec.d_y;

        return *this;
    }

    inline UVector2 operator+(const UVector2& vec) const
    {
        return UVector2(d_x + vec.d_x, d_y + vec.d_y);
    }

    inline UVector2 operator-(const UVector2& vec) const
    {
        return UVector2(d_x - vec.d_x, d_y - vec.d_y);
    }

    inline UVector2 operator*(const UVector2& vec) const
    {
        return UVector2(d_x * vec.d_x, d_y * vec.d_y);
    }

    inline UVector2 operator* (const float c)
    {
        return UVector2(d_x * c, d_y * c);
    }

    inline UVector2 operator/(const UVector2& vec) const
    {
        return UVector2(d_x / vec.d_x, d_y / vec.d_y);
    }

    inline UVector2 operator*(const UDim c) const
    {
        return UVector2(d_x * c, d_y * c);
    }

    inline UVector2& operator*=(const UDim c)
    {
        d_x *= c;
        d_y *= c;

        return *this;
    }

    inline UVector2 operator/(const UDim c) const
    {
        return UVector2(d_x / c, d_y / c);
    }

    inline UVector2 operator/=(const UDim c)
    {
        d_x /= c;
        d_y /= c;

        return *this;
    }

    inline bool operator==(const UVector2& vec) const
    {
        return ((d_x == vec.d_x) && (d_y == vec.d_y));
    }

    inline bool operator!=(const UVector2& vec) const
    {
        return !(operator==(vec));
    }
    
    /*!
    \brief Writes a Vector2 to a stream
    */
    inline friend std::ostream& operator << (std::ostream& s, const UVector2& val)
    {
        s << val.d_x << "," << val.d_y;
        return s;
    }

    /*!
    \brief Extracts a UVector2 from a stream
    */
    inline friend std::istream& operator >> (std::istream& s, UVector2& val)
    {
        s >> optionalChar<'{'> >> val.d_x >> optionalChar<','> >> val.d_y >> optionalChar<'}'>;
        return s;
    }

    //! \brief finger saving alias for UVector2(0, 0)
    inline static UVector2 zero()
    {
        return UVector2(TypeSensitiveZero<UDim>(), TypeSensitiveZero<UDim>());
    }

    //! \brief finger saving alias for UVector2(1, 1)
    inline static UVector2 one()
    {
        return UVector2(TypeSensitiveOne<UDim>(), TypeSensitiveOne<UDim>());
    }
    
    //! \brief finger saving alias for UVector2(1, 0)
    inline static UVector2 one_x()
    {
        return UVector2(TypeSensitiveOne<UDim>(), TypeSensitiveZero<UDim>());
    }
    
    //! \brief finger saving alias for UVector2(0, 1)
    inline static UVector2 one_y()
    {
        return UVector2(TypeSensitiveZero<UDim>(), TypeSensitiveOne<UDim>());
    }

    UDim d_x;
    UDim d_y;
};

} // End of  CEGUI namespace section

#endif
