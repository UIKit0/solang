/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * Copyright (C) 2009, 2010 Debarshi Ray <rishi@gnu.org>
 *
 * Solang is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Solang is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SOLANG_I_RENDERER_H
#define SOLANG_I_RENDERER_H

#include "non-copyable.h"
#include "types.h"

namespace Solang
{

class Application;
class IPlugin;
class IRendererSelector;

class IRenderer :
    private NonCopyable
{
    public:
        virtual
        ~IRenderer() throw() = 0;

        virtual void
        render(const PhotoPtr & photo) throw() = 0;

        virtual void
        render(const PhotoList & photos) throw() = 0;

        virtual PhotoList
        get_current_selection() throw() = 0;

        virtual TreePathList
        get_selected_paths() const throw() = 0;

        virtual void
        present() throw() = 0;

        virtual void
        receive_plugin(IPlugin & plugin) throw() = 0;

        virtual IRendererPtr
        receive_selector(IRendererSelector & selector,
                         const IRendererPtr & renderer) throw() = 0;

    protected:
        IRenderer() throw();

    private:
};

} //namespace Solang

#endif // SOLANG_I_RENDERER_H
