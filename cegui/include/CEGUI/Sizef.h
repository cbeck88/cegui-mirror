/***********************************************************************
    created:    19th August 2015
    author:     Lukas Meindl (based on code by Paul D Turner)
    
    purpose:    Defines interface for Sizef class
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
#ifndef _CEGUISizef_h_
#define _CEGUISizef_h_

#include "CEGUI/AspectMode.h"
#include "CEGUI/StreamHelper.h"
#include <typeinfo>

#include <glm/glm.hpp>

// Start of CEGUI namespace section
namespace CEGUI
{
/*

/ *!
\brief
    How aspect ratio should be maintained
* /
enum AspectMode
{
    //! Ignores the target aspect (default)
    AM_IGNORE,
    / *!
    Satisfies the aspect ratio by shrinking the size as little
    as possible to fit inside it
    * /
    AM_SHRINK,
    / *!
    Satisfies the aspect ratio by expanding the widget as little
    as possible outside it
    * /
    AM_EXPAND
};
*/

/*!
\brief
    Class that holds a size (width & height) in float values.
*/
class Sizef
{
public:
    inline Sizef()
    {}

    inline Sizef(const float width, const float height):
        d_width(width),
        d_height(height)
    {}

    inline Sizef(const Sizef& v):
        d_width(v.d_width),
        d_height(v.d_height)
    {}

    inline bool operator==(const Sizef& other) const
    {
        return d_width == other.d_width && d_height == other.d_height;
    }

    inline bool operator!=(const Sizef& other) const
    {
        return !operator==(other);
    }

    inline Sizef operator*(const float c) const
    {
        return Sizef(d_width * c, d_height * c);
    }

    inline Sizef operator*(const Sizef& s) const
    {
        return Sizef(d_width * s.d_width, d_height * s.d_height);
    }

    inline Sizef operator*(const glm::vec2& vec) const
    {
        return Sizef(d_width * vec.x, d_height * vec.y);
    }

    inline Sizef operator+(const Sizef& s) const
    {
        return Sizef(d_width + s.d_width, d_height + s.d_height);
    }

    inline Sizef operator-(const Sizef& s) const
    {
        return Sizef(d_width - s.d_width, d_height - s.d_height);
    }

    /*!
    \brief Writes a Sizef to a stream
    */
    inline friend std::ostream& operator << (std::ostream& s, const Sizef& val)
    {
        s << "w:" << val.d_width << " h:" << val.d_height;
        
        return s;
    }

    /*!
    \brief Extracts a Sizef from a stream
    */
    inline friend std::istream& operator >> (std::istream& s, Sizef& val)
    {
        s >> MandatoryString(" w :") >> val.d_width >> MandatoryString(" h :") >> val.d_height;

        return s;
    }

    inline void clamp(const Sizef& min, const Sizef& max)
    {
        assert(min.d_width <= max.d_width);
        assert(min.d_height <= max.d_height);
        
        if (d_width < min.d_width)
            d_width = min.d_width;
        else if (d_width > max.d_width)
            d_width = max.d_width;
        
        if (d_height < min.d_height)
            d_height = min.d_height;
        else if (d_height > max.d_height)
            d_height = max.d_height;
    }

    inline void scaleToAspect(AspectMode mode, float ratio)
    {
        if (mode == AM_IGNORE)
            return;

        if(d_width <= 0 && d_height <= 0)
            return;

        assert(ratio > 0);

        const float expectedWidth = d_height * ratio;
        const bool keepHeight = (mode == AM_SHRINK) ?
                expectedWidth <= d_width : expectedWidth >= d_width;

        if (keepHeight)
        {
            d_width = expectedWidth;
        }
        else
        {
            d_height = d_width / ratio;
        }
    }

    //! \brief finger saving alias for Sizef(side, side)
    inline static Sizef square(const float side)
    {
        return Sizef(side, side);
    }

    //! \brief finger saving alias for Sizef(0, 0)
    inline static Sizef zero()
    {
        return square(0.0f);
    }
    
    //! \brief finger saving alias for Sizef(1, 1)
    inline static Sizef one()
    {
        return square(1.0f);
    }
    
    //! \brief finger saving alias for Sizef(1, 0)
    inline static Sizef one_width()
    {
        return Sizef(1.0f, 0.0f);
    }
    
    //! \brief finger saving alias for Sizef(0, 1)
    inline static Sizef one_height()
    {
        return Sizef(0.0f, 1.0f);
    }

    float d_width;
    float d_height;
};


} // End of  CEGUI namespace section

#endif
