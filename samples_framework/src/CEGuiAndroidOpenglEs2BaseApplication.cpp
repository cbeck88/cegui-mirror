/***********************************************************************
    created:    27/7/2015
    author:     Yaron Cohen-Tal
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

#include "CEGuiAndroidOpenglEs2BaseApplication.h"

#include "CEGUI/RendererModules/OpenGL/GL3Renderer.h"

CEGuiAndroidOpenglEs2BaseApplication::CEGuiAndroidOpenglEs2BaseApplication()
{
    d_renderer = &CEGUI::OpenGL3Renderer::create();
}
    
bool CEGuiAndroidOpenglEs2BaseApplication::init(SamplesFrameworkBase* sampleApp,
  const CEGUI::String &logFile, const CEGUI::String &dataPathPrefixOverride)
{
    if (!CEGuiBaseApplication::init(sampleApp, logFile, dataPathPrefixOverride))
        return false;
    glClearColor(0.f, 0.f, 0.f, 0.f);
    return true;
}

void CEGuiAndroidOpenglEs2BaseApplication::cleanup()
{
    CEGuiBaseApplication::cleanup();
}

void CEGuiAndroidOpenglEs2BaseApplication::beginRendering(const float elapsed)
{
    CEGUI_UNUSED(elapsed);
    glClear(GL_COLOR_BUFFER_BIT);
}
