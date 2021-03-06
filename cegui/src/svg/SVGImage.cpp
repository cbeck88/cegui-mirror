﻿/***********************************************************************
    created:    30th July 2013
    author:     Lukas Meindl
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2013 Paul D Turner & The CEGUI Development Team
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
#include "CEGUI/svg/SVGImage.h"
#include "CEGUI/GeometryBuffer.h"
#include "CEGUI/CoordConverter.h"
#include "CEGUI/svg/SVGTesselator.h"
#include "CEGUI/svg/SVGData.h"
#include "CEGUI/svg/SVGBasicShape.h"
#include "CEGUI/svg/SVGDataManager.h"
#include "CEGUI/XMLAttributes.h"



// Start of CEGUI namespace section
namespace CEGUI
{
const String ImageTypeAttribute( "type" );
const String ImageNameAttribute( "name" );
const String ImageSVGDataAttribute( "SVGData" );
const String ImageXPosAttribute( "xPos" );
const String ImageYPosAttribute( "yPos" );
const String ImageWidthAttribute( "width" );
const String ImageHeightAttribute( "height" );
const String ImageXOffsetAttribute( "xOffset" );
const String ImageYOffsetAttribute( "yOffset" );
const String ImageAutoScaledAttribute( "autoScaled" );
const String ImageNativeHorzResAttribute( "nativeHorzRes" );
const String ImageNativeVertResAttribute( "nativeVertRes" );

//----------------------------------------------------------------------------//
SVGImage::SVGImage(const String& name) :
    Image(name),
    d_svgData(0),
    d_useGeometryAntialiasing(true)
{
}

//----------------------------------------------------------------------------//
SVGImage::SVGImage(const String& name, SVGData& svg_data) :
    Image(name,
          glm::vec2(0.0f, 0.0f),
          Rectf(glm::vec2(0.0f, 0.0f),
                glm::vec2(svg_data.getWidth(), svg_data.getHeight())),
          ASM_Disabled,
          Sizef(640, 480)),
    d_svgData(&svg_data),
    d_useGeometryAntialiasing(true)
{
}

//----------------------------------------------------------------------------//
SVGImage::SVGImage(const XMLAttributes& attributes) :
    Image(attributes.getValueAsString(ImageNameAttribute),
          glm::vec2(static_cast<float>(attributes.getValueAsInteger(ImageXOffsetAttribute, 0)),
                    static_cast<float>(attributes.getValueAsInteger(ImageYOffsetAttribute, 0))),
          Rectf(glm::vec2(static_cast<float>(attributes.getValueAsInteger(ImageXPosAttribute, 0)),
                          static_cast<float>(attributes.getValueAsInteger(ImageYPosAttribute, 0))),
                Sizef(static_cast<float>(attributes.getValueAsInteger(ImageWidthAttribute, 0)),
                      static_cast<float>(attributes.getValueAsInteger(ImageHeightAttribute, 0)))),
          PropertyHelper<AutoScaledMode>::fromString(attributes.getValueAsString(ImageAutoScaledAttribute)),
          Sizef(static_cast<float>(attributes.getValueAsInteger(ImageNativeHorzResAttribute, 640)),
                static_cast<float>(attributes.getValueAsInteger(ImageNativeVertResAttribute, 480)))),
    d_svgData(&SVGDataManager::getSingleton().getSVGData(
              attributes.getValueAsString(ImageSVGDataAttribute))),
    d_useGeometryAntialiasing(true)
{
}

//----------------------------------------------------------------------------//
void SVGImage::setSVGData(SVGData* svg_Data)
{
    d_svgData = svg_Data;
}

//----------------------------------------------------------------------------//
SVGData* SVGImage::getSVGData()
{
    return d_svgData;
}

//----------------------------------------------------------------------------//
void SVGImage::render(std::vector<GeometryBuffer*>& image_geometry_buffers,
                      const ImageRenderSettings& render_settings) const
{
    Rectf dest(render_settings.d_destArea);
    // apply rendering offset to the destination Rect
    dest.offset(d_scaledOffset);

    const CEGUI::Rectf*const&  clip_area = render_settings.d_clipArea;
    // Calculate the actual (clipped) area to which we want to render to
    Rectf final_rect(clip_area ? dest.getIntersection(*clip_area) : dest );

    // check if our Image is totally clipped and return if it is
    if ((final_rect.getWidth() == 0) || (final_rect.getHeight() == 0))
        return;

    // Calculate the scale factor for our Image which is the scaling of the Image
    // area to the destination area of our render call
    const glm::vec2 scale_factor(dest.getWidth() / d_imageArea.getWidth(), dest.getHeight() / d_imageArea.getHeight());

    // URGENT FIXME: Shouldn't this be in the hands of the user?
    final_rect.d_min.x = CoordConverter::alignToPixels(final_rect.d_min.x);
    final_rect.d_min.y = CoordConverter::alignToPixels(final_rect.d_min.y);
    final_rect.d_max.x = CoordConverter::alignToPixels(final_rect.d_max.x);
    final_rect.d_max.y = CoordConverter::alignToPixels(final_rect.d_max.y);

    SVGImageRenderSettings svg_render_settings(render_settings,
                                               scale_factor,
                                               d_useGeometryAntialiasing);

    const std::vector<SVGBasicShape*>& shapes = d_svgData->getShapes();
    const unsigned int shape_count = shapes.size();
    for (unsigned int i = 0; i < shape_count; ++i)
        shapes[i]->render(image_geometry_buffers, svg_render_settings);
}

//----------------------------------------------------------------------------//
bool SVGImage::getUsesGeometryAntialiasing() const
{
    return d_useGeometryAntialiasing;
}

//----------------------------------------------------------------------------//
void SVGImage::setUseGeometryAntialiasing(bool use_geometry_antialiasing)
{
    d_useGeometryAntialiasing = use_geometry_antialiasing;
}

//----------------------------------------------------------------------------//
}

