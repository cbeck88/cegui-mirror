/************************************************************************
	filename: 	CEGUIPopupMenu.cpp
	created:	1/4/2005
	author:		Tomas Lindquist Olsen (based on code by Paul D Turner)
	
	purpose:	Implementation of PopupMenu widget base class
*************************************************************************/
/*************************************************************************
    Crazy Eddie's GUI System (http://www.cegui.org.uk)
    Copyright (C)2004 - 2005 Paul D Turner (paul@cegui.org.uk)

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
#include "elements/CEGUIPopupMenu.h"
#include "elements/CEGUIMenuItem.h"

// Start of CEGUI namespace section
namespace CEGUI
{

/*************************************************************************
	Definition of Properties for this class
*************************************************************************/
PopupMenuProperties::FadeInTime		PopupMenu::d_fadeInTimeProperty;
PopupMenuProperties::FadeOutTime	PopupMenu::d_fadeOutTimeProperty;


/*************************************************************************
	Constants
*************************************************************************/
const String PopupMenu::EventNamespace("PopupMenu");


/*************************************************************************
	Constructor for PopupMenu base class.
*************************************************************************/
PopupMenu::PopupMenu(const String& type, const String& name)
	: MenuBase(type, name),
	d_origAlpha(d_alpha),
	d_fadeOutTime(0),
	d_fadeInTime(0),
	d_fading(false),
	d_fadingOut(false),
	d_isOpen(false)
{
	d_itemSpacing = 2;

	addPopupMenuProperties();

	// enable auto resizing
	d_autoResize = true;

	// disable parent clipping
	setClippedByParent(false);

	// hide by default
	hide();
}


/*************************************************************************
	Destructor for PopupMenu base class.
*************************************************************************/
PopupMenu::~PopupMenu(void)
{
}


/*************************************************************************
	Tells the popup menu to open.
*************************************************************************/
void PopupMenu::openPopupMenu(bool notify)
{
    // already open and not fading, or fading in?
    if (d_isOpen && (!d_fading || !d_fadingOut))
    {
        // then don't do anything
        return;
    }

    // should we let the parent menu item initiate the open?
    Window* parent = getParent();
    if (notify && parent && parent->testClassName("MenuItem"))
    {
        static_cast<MenuItem*>(parent)->openPopupMenu();
        return; // the rest will be handled when MenuItem calls us itself
    }

    // we'll handle it ourselves then.
	// are we fading, and fading out?
	if (d_fading && d_fadingOut)
	{
		if (d_fadeInTime>0.0f&&d_fadeOutTime>0.0f)
		{
			// jump to the point of the fade in that has the same alpha as right now - this keeps it smooth
			d_fadeElapsed = ((d_fadeOutTime-d_fadeElapsed)/d_fadeOutTime)*d_fadeInTime;
		}
		else
		{
			// start the fade in from the beginning
			d_fadeElapsed = 0;
		}
		// change to fade in
		d_fadingOut=false;
	}
	// otherwise just start normal fade in!
	else if (d_fadeInTime>0.0f)
	{
		d_fading = true;
		d_fadingOut=false;
		setAlpha(0.0f);
		d_fadeElapsed = 0;
	}
	// should not fade!
	else
	{
		d_fading = false;
		setAlpha(d_origAlpha);
	}
	
	show();
	moveToFront();
}


/*************************************************************************
	Tells the popup menu to close.
*************************************************************************/
void PopupMenu::closePopupMenu(bool notify)
{
    // already closed?
    if (!d_isOpen)
    {
        // then do nothing
        return;
    }

    // should we let the parent menu item close initiate the close?
    Window* parent = getParent();
    if (notify && parent && parent->testClassName("MenuItem"))
    {
        static_cast<MenuItem*>(parent)->closePopupMenu();
        return; // the rest will be handled when MenuItem calls us itself
    }

    // we'll do it our selves then.
	// are we fading, and fading in?
	if (d_fading && !d_fadingOut)
	{
		// make sure the "fade back out" is smooth - if possible !
		if (d_fadeOutTime>0.0f&&d_fadeInTime>0.0f)
		{
			// jump to the point of the fade in that has the same alpha as right now - this keeps it smooth
			d_fadeElapsed = ((d_fadeInTime-d_fadeElapsed)/d_fadeInTime)*d_fadeOutTime;
		}
		else
		{
			// start the fade in from the beginning
			d_fadeElapsed = 0;
		}
		// change to fade out
		d_fadingOut=true;
	}
	// otherwise just start normal fade out!
	else if (d_fadeOutTime>0.0f)
	{
	    d_fading = true;
	    d_fadingOut = true;
	    setAlpha(d_origAlpha);
	    d_fadeElapsed = 0;
	}
	// should not fade!
	else
	{
	    d_fading = false;
	    hide();
	}
}


/*************************************************************************
    Perform actual update processing for this Window.
*************************************************************************/
void PopupMenu::updateSelf(float elapsed)
{
    MenuBase::updateSelf(elapsed);

    // handle fading
    if (d_fading)
    {
		d_fadeElapsed+=elapsed;

		// fading out
		if (d_fadingOut)
		{
			if (d_fadeElapsed>=d_fadeOutTime)
			{
				hide();
				d_fading=false;
				setAlpha(d_origAlpha); // set real alpha so users can show directly without having to restore it
			}
			else
			{
				setAlpha(d_origAlpha*(d_fadeOutTime-d_fadeElapsed)/d_fadeOutTime);
			}

		}

		// fading in
		else
		{
			if (d_fadeElapsed>=d_fadeInTime)
			{
				d_fading=false;
				setAlpha(d_origAlpha);
			}
			else
			{
				setAlpha(d_origAlpha*d_fadeElapsed/d_fadeInTime);
			}
		}
	}
}


/*************************************************************************
	Sets up sizes and positions for attached ItemEntry children.
*************************************************************************/
void PopupMenu::layoutItemWidgets()
{
	// get render area
	Rect render_rect = getItemRenderArea();

	// get starting position
	const float x0 = PixelAligned(render_rect.d_left);
	float y0 = PixelAligned(render_rect.d_top);

	URect rect;
	UVector2 sz(cegui_absdim(PixelAligned(render_rect.getWidth())), cegui_absdim(0)); // set item width

	// iterate through all items attached to this window
	ItemEntryList::iterator item = d_listItems.begin();
	while ( item != d_listItems.end() )
	{
		// get the "optimal" height of the item and use that!
		sz.d_y.d_offset = PixelAligned((*item)->getItemPixelSize().d_height); // rounding errors ?

		// set destination rect
		rect.setPosition(UVector2(cegui_absdim(x0), cegui_absdim(y0)) );
		rect.setSize( sz );
		(*item)->setWindowArea(rect);

		// next position
		y0 += PixelAligned(sz.d_y.d_offset + d_itemSpacing);

		item++; // next item
	}
}


/*************************************************************************
	Returns the "optimal" size for the content in unclipped pixels
*************************************************************************/
Size PopupMenu::getContentSize()
{
	// find the content sizes
	float widest = 0;
	float total_height = 0;
	
	size_t i = 0;
	size_t max = d_listItems.size();
	while (i < max)
	{
		const Size sz = d_listItems[i]->getItemPixelSize();
		if (sz.d_width > widest)
			widest = sz.d_width;
		total_height += sz.d_height;

		i++;
	}
	
	const float count = float(i);

	// vert item spacing
	if (count >= 2)
	{
	    total_height += (count-1)*d_itemSpacing;
	}

	// return the content size
	return Size(widest, total_height);
}


/*************************************************************************
	Handler for when window alpha changes
*************************************************************************/
void PopupMenu::onAlphaChanged(WindowEventArgs& e)
{
	MenuBase::onAlphaChanged(e);
	
	// if we are not fading, this is a real alpha change request and we save a copy of the value
	if (!d_fading)
	{
		d_origAlpha = d_alpha;
	}
}


/************************************************************************
    Handler for destruction started events
*************************************************************************/
void PopupMenu::onDestructionStarted(WindowEventArgs& e)
{
    // if we are attached to a menuitem, we make sure that gets updated
    Window* p = getParent();
    if (p && p->testClassName("MenuItem"))
    {
        static_cast<MenuItem*>(p)->setPopupMenu(0);
    }
    MenuBase::onDestructionStarted(e);
}


/************************************************************************
    Handler for shown events
*************************************************************************/
void PopupMenu::onShown(WindowEventArgs& e)
{
    d_isOpen = true;
    MenuBase::onShown(e);
}


/************************************************************************
    Handler for hidden events
*************************************************************************/
void PopupMenu::onHidden(WindowEventArgs& e)
{
    d_isOpen = false;
    MenuBase::onHidden(e);
}


/************************************************************************
	Handler for mouse button down events
************************************************************************/
void PopupMenu::onMouseButtonDown(MouseEventArgs& e)
{
        MenuBase::onMouseButtonDown(e);
        // dont reach our parent
        e.handled = true;
}


/************************************************************************
	Handler for mouse button up events
************************************************************************/
void PopupMenu::onMouseButtonUp(MouseEventArgs& e)
{
        MenuBase::onMouseButtonUp(e);
        // dont reach our parent
        e.handled = true;
}


/*************************************************************************
	Add PopupMenu specific properties
*************************************************************************/
void PopupMenu::addPopupMenuProperties(void)
{
	addProperty(&d_fadeInTimeProperty);
    addProperty(&d_fadeOutTimeProperty);
}

} // End of  CEGUI namespace section
