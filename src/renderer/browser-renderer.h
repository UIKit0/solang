/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * Copyright (C) 2009, 2010 Debarshi Ray <rishi@gnu.org>
 * Copyright (C) 2009 Santanu Sinha <santanu.sinha@gmail.com>
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

#ifndef SOLANG_BROWSER_RENDERER_H
#define SOLANG_BROWSER_RENDERER_H

#include <string>

#include <gdl/gdl.h>
#include <glibmm.h>
#include <gtkmm.h>
#include <sigc++/sigc++.h>

#include "i-renderer.h"
#include "pagination-bar.h"
#include "plugin.h"
#include "thumbnail-view.h"
#include "types.h"

namespace Solang
{

class BrowserRenderer :
    public IRenderer,
    public Plugin
{
    public:
        BrowserRenderer() throw();

        virtual
        ~BrowserRenderer() throw();

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
        on_selection_changed() throw();

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
        clear_thumbnails() throw();

        void
        on_action_add_to_export_queue() throw();

        void
        on_action_view_slideshow() throw();

        void
        on_action_view_zoom_changed(
            const ScaleActionPtr & scale_action) throw();

        void
        on_action_view_zoom_in(const ScaleActionPtr & scale_action)
                               throw();

        void
        on_action_view_zoom_out(const ScaleActionPtr & scale_action)
                                throw();

        void
        on_action_go_previous() throw();

        void
        on_action_go_next() throw();

        void
        on_action_go_first() throw();

        void
        on_action_go_last() throw();

        void
        on_action_change_background_color(Glib::ustring color_code) throw();

        void
        on_action_unset_background_color() throw();

        void
        on_dock_item_parent_changed(Gtk::Widget * previous_parent)
                                    throw();

        void
        on_item_activated(const Gtk::TreeModel::Path & path) throw();

        void
        on_limits_changed() throw();

        void
        on_list_store_change_begin(Application & application) throw();

        void
        on_list_store_change_end(Application & application) throw();

        void
        on_switch_page(GtkNotebookPage * notebook_page, guint page_num)
                       throw();

        bool
        on_visible(const Gtk::TreeModel::const_iterator & iter)
                   throw();

        void
        reload() throw();

        void
        set_thumbnail_size() throw();

        void
        status_pop() throw();

        void
        status_push() throw();

        ApplicationPtr application_;

        IconFactoryPtr iconFactory_;

        ActionGroupPtr actionGroup_;

        Gtk::UIManager::ui_merge_id uiID_;

        Gtk::UIManager::ui_merge_id uiIDThumbnail_;

        const std::string dockItemName_;
    
        const Glib::ustring dockItemTitle_;
    
        GdlDockItemBehavior dockItemBehaviour_;

        GtkWidget * dockItem_;

        Gtk::VBox vBox_;

        Gtk::HBox hBox_;

        PaginationBar paginationBar_;

        Gtk::Label dummyLabel_;

        Gtk::ScrolledWindow scrolledWindow_;

        TreeModelFilterPtr treeModelFilter_;

        ThumbnailView thumbnailView_;

        double zoomValue_;

        gint pageNum_;

        guint contextID_;

        GSettings * settings_;

        sigc::connection signalInitEnd_;

        sigc::connection signalItemActivated_;

        sigc::connection signalListStoreChangeBegin_;

        sigc::connection signalListStoreChangeEnd_;

        sigc::connection signalSelectionChanged_;

        sigc::connection signalSwitchPage_;

    private:
};

} // namespace Solang

#endif // SOLANG_BROWSER_RENDERER_H
