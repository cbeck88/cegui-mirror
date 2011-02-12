/***********************************************************************
    filename:   FalTabTabButton.cpp
    created:    Fri Jul 8 2005
    author:     Paul D Turner <paul@cegui.org.uk>
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2006 Paul D Turner & The CEGUI Development Team
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
#include "FalTabButton.h"
#include "falagard/CEGUIFalWidgetLookManager.h"
#include "falagard/CEGUIFalWidgetLookFeel.h"
#include "elements/CEGUITabButton.h"
#include "elements/CEGUITabControl.h"

// Start of CEGUI namespace section
namespace CEGUI
{
    const String FalagardTabButton::TypeName("Falagard/TabButton");

    FalagardTabButton::FalagardTabButton(const String& type) :
        WindowRenderer(type, "TabButton")
    {
    }

    void FalagardTabButton::render()
    {
        TabButton* w = (TabButton*)d_window;
        // get WidgetLookFeel for the assigned look.
        const WidgetLookFeel& wlf = getLookNFeel();

		TabControl* tc = static_cast<TabControl*>(w->getParent()->getParent());

        String state;
		String prefix((tc->getTabPanePosition() == TabControl::Top) ? "Top" : "Bottom");

		if (w->isDisabled())
		    state = "Disabled";
		else if (w->isSelected())
		    state = "Selected";
		else if (w->isPushed())
		    state = "Pushed";
		else if (w->isHovering())
		    state = "Hover";
		else
		    state = "Normal";

        if (!wlf.isStateImageryPresent(prefix + state))
        {
            state = "Normal";
			if (!wlf.isStateImageryPresent(prefix + state))
				prefix = "";
        }

        wlf.getStateImagery(prefix + state).render(*w);
    }

} // End of  CEGUI namespace section
