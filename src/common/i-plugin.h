/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * i-plugin.h
 * Copyright (C) 2009 Debarshi Ray <rishi@gnu.org>
 * 
 * i-plugin.h is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * i-manager.h is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SOLANG_I_PLUGIN_H
#define SOLANG_I_PLUGIN_H

#include <sigc++/sigc++.h>

#include "non-copyable.h"

namespace Solang
{

class Application;
class BrowserRenderer;
class EnlargedRenderer;
class SlideshowRenderer;

class IPlugin :
    private NonCopyable
{
    public:
        virtual
        ~IPlugin() throw() = 0;

        virtual void
        init(Application & application) throw() = 0;

        virtual void
        final(Application & application) throw() = 0;

        virtual sigc::signal<void, IPlugin &> &
        initialized() throw() = 0;

        virtual sigc::signal<void, IPlugin &> &
        finalized() throw() = 0;

        virtual void
        visit_renderer(BrowserRenderer & browser_renderer)
                       throw() = 0;

        virtual void
        visit_renderer(EnlargedRenderer & editor_renderer)
                       throw() = 0;

        virtual void
        visit_renderer(SlideshowRenderer & editor_renderer)
                       throw() = 0;

    protected:
        IPlugin() throw();

    private:
};

} // namespace Solang

#endif // SOLANG_I_PLUGIN_H
