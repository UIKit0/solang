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

#ifndef SOLANG_SLIDESHOW_RENDERER_H
#define SOLANG_SLIDESHOW_RENDERER_H

#include <string>

#include <glibmm.h>
#include <gtkmm.h>
#include <sigc++/sigc++.h>

#include "i-renderer.h"
#include "plugin.h"
#include "types.h"

namespace Solang
{

class SlideshowRenderer :
    public IRenderer,
    public Plugin
{
    public:
        SlideshowRenderer() throw();

        virtual
        ~SlideshowRenderer() throw();

        virtual void
        init(Application & application) throw();

        virtual void
        final(Application & application) throw();

        virtual void
        visit_renderer(BrowserRenderer & browser_renderer) throw();

        virtual void
        visit_renderer(EnlargedRenderer & enlarged_renderer) throw();

        virtual void
        visit_renderer(SlideshowRenderer & slideshow_renderer) throw();

        virtual void
        render(const PhotoPtr & photo) throw();

        virtual void
        render(const PhotoList & photos) throw();

        virtual PhotoList
        get_current_selection() throw();

        virtual TreePathList
        get_selected_paths() const throw();

        virtual void
        present() throw();

        virtual void
        receive_plugin(IPlugin & plugin) throw();

        virtual IRendererPtr
        receive_selector(IRendererSelector & selector,
                         const IRendererPtr & renderer) throw();

    protected:
        void
        create_action_group() throw();

        void
        create_slideshow_window() throw();

        void
        on_action_go_first() throw();

        void
        on_action_go_last() throw();

        void
        on_action_go_next() throw();

        void
        on_action_go_previous() throw();

        void
        on_action_stop_slideshow() throw();

        bool
        on_timeout() throw();

        void
        prepare_for_first_use() throw();

        ApplicationPtr application_;

        bool firstUse_;

        ActionGroupPtr actionGroup_;

        Gtk::UIManager::ui_merge_id uiID_;

        Gtk::TreeModel::iterator modelIter_;

        IRendererPtr previousRenderer_;

        SlideshowWindowPtr slideshowWindow_;

        sigc::connection signalTimeout_;

    private:
};

} // namespace Solang

#endif // SOLANG_SLIDESHOW_RENDERER_H
