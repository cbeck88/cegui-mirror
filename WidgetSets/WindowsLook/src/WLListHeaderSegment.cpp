/************************************************************************
	filename: 	WLListHeaderSegment.cpp
	created:	16/8/2004
	author:		Paul D Turner
*************************************************************************/
/*************************************************************************
    Crazy Eddie's GUI System (http://crayzedsgui.sourceforge.net)
    Copyright (C)2004 Paul D Turner (crayzed@users.sourceforge.net)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*************************************************************************/
#include "WLListHeaderSegment.h"
#include "CEGUIImagesetManager.h"
#include "CEGUIImageset.h"
#include "CEGUIFont.h"


// Start of CEGUI namespace section
namespace CEGUI
{
/*************************************************************************
	Constants
*************************************************************************/
// image / imageset related
const utf8	WLListHeaderSegment::ImagesetName[]				= "WindowsLook";
const utf8	WLListHeaderSegment::BackdropMainImageName[]	= "HeaderMainBrush";
const utf8	WLListHeaderSegment::BackdropEdgeImageName[]	= "HeaderBottomEdge";
const utf8	WLListHeaderSegment::SplitterImageName[]		= "HeaderSplitter";
const utf8	WLListHeaderSegment::SortUpImageName[]			= "SmallUpArrow";
const utf8	WLListHeaderSegment::SortDownImageName[]		= "SmallDownArrow";
const utf8	WLListHeaderSegment::NormalMouseCursor[]		= "MouseArrow";
const utf8	WLListHeaderSegment::SizingMouseCursor[]		= "MouseEsWeCursor";
const utf8	WLListHeaderSegment::MovingMouseCursor[]		= "MouseMoveCursor";

// colours
const colour WLListHeaderSegment::BackdropNormalColour		= 0xDDDDDD;
const colour WLListHeaderSegment::BackdropHighlightColour	= 0xEFEFEF;
const colour WLListHeaderSegment::SplitterNormalColour		= 0xDDDDDD;
const colour WLListHeaderSegment::SplitterHighlightColour	= 0xEFEFEF;


/*************************************************************************
	Constructor	
*************************************************************************/
WLListHeaderSegment::WLListHeaderSegment(const String& type, const String& name) :
	ListHeaderSegment(type, name)
{
	// init the image cache
	Imageset* iset = ImagesetManager::getSingleton().getImageset(ImagesetName);

	d_backdropMainImage		= &iset->getImage(BackdropMainImageName);
	d_backdropEdgeImage		= &iset->getImage(BackdropEdgeImageName);
	d_splitterImage			= &iset->getImage(SplitterImageName);
	d_sortAscendImage		= &iset->getImage(SortUpImageName);
	d_sortDescendImage		= &iset->getImage(SortDownImageName);
	d_normalMouseCursor		= &iset->getImage(NormalMouseCursor);
	d_sizingMouseCursor		= &iset->getImage(SizingMouseCursor);
	d_movingMouseCursor		= &iset->getImage(MovingMouseCursor);

	setMouseCursor(&iset->getImage(NormalMouseCursor));
}


/*************************************************************************
	Destructor
*************************************************************************/
WLListHeaderSegment::~WLListHeaderSegment(void)
{
}


/*************************************************************************
	Render the widget
*************************************************************************/
void WLListHeaderSegment::drawSelf(float z)
{
	// get the destination screen rect for this window
	Rect absrect(getUnclippedPixelRect());

	// get clipping Rect for window
	Rect clipper(getPixelRect());

	Vector3 pos(absrect.d_left, absrect.d_top, z);

	// if widget is not totally clipped
	if (clipper.getWidth() != 0)
	{
		renderSegmentImagery(pos, getEffectiveAlpha(), clipper);
	}

	// always draw the ghost if the segment is geing dragged.
	if (d_dragMoving)
	{
		clipper = System::getSingleton().getRenderer()->getRect();
		pos.d_x = absrect.d_left + d_dragPosition.d_x;
		pos.d_y = absrect.d_top + d_dragPosition.d_y;
		pos.d_z = 0.0f;
		renderSegmentImagery(pos, getEffectiveAlpha() * 0.5f, clipper);
	}
}


/*************************************************************************
	Render segment	
*************************************************************************/
void WLListHeaderSegment::renderSegmentImagery(Vector3 pos, float alpha, const Rect& clipper)
{
	Rect absrect(pos.d_x, pos.d_y, pos.d_x + getAbsoluteWidth(), pos.d_y + getAbsoluteHeight());
	Rect destrect(absrect);

	// calculate colours to use.
	colour alpha_comp = ((colour)(alpha * 255.0f) << 24);
	ColourRect colours;

	//
	// draw the main images
	//
	if ((d_segmentHover != d_segmentPushed) && !d_splitterHover && isClickable())
	{
		colours.d_top_left = colours.d_top_right = colours.d_bottom_left = colours.d_bottom_right = alpha_comp | BackdropHighlightColour;
	}
	else
	{
		colours.d_top_left = colours.d_top_right = colours.d_bottom_left = colours.d_bottom_right = alpha_comp | BackdropNormalColour;
	}

	// draw main area
	destrect.d_right -= d_splitterImage->getWidth();
	destrect.d_bottom -= d_backdropEdgeImage->getHeight();
	d_backdropMainImage->draw(destrect, pos.d_z, clipper, colours);

	//draw bottom edge
	destrect.d_top = destrect.d_bottom;
	destrect.d_bottom = absrect.d_bottom;
	d_backdropEdgeImage->draw(destrect, pos.d_z, clipper, colours);

	// draw splitter
	destrect.d_top = absrect.d_top;
	destrect.d_left = destrect.d_right;
	destrect.d_right = absrect.d_right;

	if (d_splitterHover)
	{
		colours.d_top_left = colours.d_top_right = colours.d_bottom_left = colours.d_bottom_right = alpha_comp | SplitterHighlightColour;
	}
	else
	{
		colours.d_top_left = colours.d_top_right = colours.d_bottom_left = colours.d_bottom_right = alpha_comp | SplitterNormalColour;
	}

	d_splitterImage->draw(destrect, pos.d_z, clipper, colours);

	// 
	// adjust clipper to be inside the splitter bar imagery
	//
	destrect.d_right = destrect.d_left;

	// pad left by half the width of the 'sort' icon.
	destrect.d_left = absrect.d_left + d_sortAscendImage->getWidth() * 0.5f;
	Rect inner_clip(destrect.getIntersection(clipper));

	//
	// Render 'sort' icon as needed
	//
	colours.d_top_left = colours.d_top_right = colours.d_bottom_left = colours.d_bottom_right = alpha_comp | 0xFFFFFF;

	if (d_sortDir == Ascending)
	{
		d_sortAscendImage->draw(Vector3(destrect.d_left, destrect.d_top + d_sortAscendImage->getHeight() * 0.5f, pos.d_z), inner_clip, colours);
		destrect.d_left += d_sortAscendImage->getWidth() * 1.5f;
	}
	else if (d_sortDir == Descending)
	{
		d_sortDescendImage->draw(Vector3(destrect.d_left, destrect.d_top + d_sortDescendImage->getHeight() * 0.5f, pos.d_z), inner_clip, colours);
		destrect.d_left += d_sortDescendImage->getWidth() * 1.5f;
	}

	//
	// Render the text
	//
	// find a font to be used.
	const Font* fnt;
	if (d_font != NULL)
	{
		fnt = d_font;
	}
	else if (d_parent != NULL)
	{
		fnt = d_parent->getFont();
	}
	else
	{
		fnt = System::getSingleton().getDefaultFont();
	}

	colours.d_top_left = colours.d_top_right = colours.d_bottom_left = colours.d_bottom_right = alpha_comp | 0x000000;

	// centre text vertically
	destrect.d_top += ((destrect.getHeight() - fnt->getLineSpacing()) * 0.5f);
	fnt->drawText(getText(), destrect, pos.d_z, inner_clip, LeftAligned, colours);
}


//////////////////////////////////////////////////////////////////////////
/*************************************************************************

	Factory Methods

*************************************************************************/
//////////////////////////////////////////////////////////////////////////
/*************************************************************************
	Create, initialise and return a WLListHeaderSegment
*************************************************************************/
Window* WLListHeaderSegmentFactory::createWindow(const String& name)
{
	WLListHeaderSegment* wnd = new WLListHeaderSegment(d_type, name);
	wnd->initialise();

	return wnd;
}

} // End of  CEGUI namespace section
