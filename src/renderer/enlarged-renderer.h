/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * Copyright (C) 2009, 2010 Debarshi Ray <rishi@gnu.org>
 * Copyright (C) 2010 Florent Thévenet <feuloren@free.fr>
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

#ifndef SOLANG_ENLARGED_RENDERER_H
#define SOLANG_ENLARGED_RENDERER_H

#include <string>

#include <gdl/gdl.h>
#include <glibmm.h>
#include <gtkmm.h>
#include <sigc++/sigc++.h>

#include "i-renderer.h"
#include "plugin.h"
#include "thumbnail-view.h"
#include "types.h"

namespace Solang
{

class EnlargedRenderer :
    public IRenderer,
    public Plugin
{
    public:
        EnlargedRenderer() throw();

        virtual
        ~EnlargedRenderer() throw();

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

        void
        on_scroll_event(GdkScrollDirection direction) throw();

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

        sigc::signal<void, PhotoList*, Glib::ustring> signal_key_press;

    protected:
        void
        create_action_group() throw();

        void
        on_action_go_previous() throw();

        void
        on_action_go_next() throw();

        void
        on_action_go_first() throw();

        void
        on_action_go_last() throw();

        void
        on_action_quit_enlarged() throw();

        void
        on_action_view_slideshow() throw();

        void
        on_action_view_best_fit() throw();

        void
        on_action_view_normal_size() throw();

        void
        on_action_view_zoom_in() throw();

        void
        on_action_view_zoom_out() throw();

        void
        on_dock_item_parent_changed(Gtk::Widget * previous_parent)
                                    throw();

        void
        on_list_store_change_end(Application & application) throw();

        bool
        on_main_window_state_event(GdkEventWindowState * event)
                                   throw();

        void
        on_pixbuf_maker_async_ready(const PixbufPtr & pixbuf) throw();

        void
        on_switch_page(GtkNotebookPage * notebook_page, guint page_num)
                       throw();

        void
        prepare_for_first_use() throw();

        ApplicationPtr application_;

        bool firstUse_;

        Glib::RefPtr<Gtk::IconFactory> iconFactory_;

        ActionGroupPtr actionGroup_;

        Gtk::UIManager::ui_merge_id uiID_;

        const std::string dockItemName_;

        const Glib::ustring dockItemTitle_;

        GdlDockItemBehavior dockItemBehaviour_;

        GtkWidget * dockItem_;

        GtkWidget * imageView_;

        GtkWidget * imageScrollWin_;

        gint pageNum_;

        PixbufMakerPtr pixbufMaker_;

        sigc::connection signalMainWindowStateEvent_;

        sigc::connection signalListStoreChangeEnd_;

        sigc::connection signalSwitchPage_;

    private:
};

} // namespace Solang

#endif // SOLANG_ENLARGED_RENDERER_H
