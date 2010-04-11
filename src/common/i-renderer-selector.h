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

#ifndef SOLANG_I_RENDERER_SELECTOR_H
#define SOLANG_I_RENDERER_SELECTOR_H

#include "non-copyable.h"
#include "types.h"

namespace Solang
{

class BrowserRenderer;
class EditorRenderer;
class EnlargedRenderer;
class SlideshowRenderer;

class IRendererSelector :
    private NonCopyable
{
    public:
        virtual
        ~IRendererSelector() throw() = 0;

        virtual IRendererPtr
        select(BrowserRenderer & browser_renderer,
               const IRendererPtr & renderer) throw();

        virtual IRendererPtr
        select(EditorRenderer & editor_renderer,
               const IRendererPtr & renderer) throw();

        virtual IRendererPtr
        select(EnlargedRenderer & enlarged_renderer,
               const IRendererPtr & renderer) throw();

        virtual IRendererPtr
        select(SlideshowRenderer & enlarged_renderer,
               const IRendererPtr & renderer) throw();

    protected:
        IRendererSelector() throw();

    private:
};

} // namespace Solang

#endif // SOLANG_I_RENDERER_SELECTOR_H
