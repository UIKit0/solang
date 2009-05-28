/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * Copyright (C) 2009 Debarshi Ray <rishi@gnu.org>
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

#ifndef SOLANG_THUMBNAIL_VIEW_H
#define SOLANG_THUMBNAIL_VIEW_H

#include <glibmm.h>
#include <gtkmm.h>

#include "cell-renderer-info.h"
#include "cell-renderer-thumbnail.h"
#include "types.h"

namespace Solang
{

class ThumbnailView :
    public Gtk::IconView 
{
    public:
        ThumbnailView() throw();

        ThumbnailView(const TreeModelPtr & model) throw();

        virtual
        ~ThumbnailView() throw();

        PhotoList
        get_selected_photos() throw();

    protected:
        PhotoPtr
        get_photo_from_path(const Gtk::TreeModel::Path & path) throw();

        virtual bool
        on_button_press_event(GdkEventButton * event) throw();

        virtual void
        on_drag_data_get_cb(const DragContextPtr & drag_context,
                            Gtk::SelectionData & data,
                            guint info, guint time) throw();

        void
        popup_menu(GdkEventButton * event) throw();

        guint selectedItems_;

        ActionGroupPtr actionGroup_;

        UIManagerPtr uiManager_;

        Gtk::UIManager::ui_merge_id uiID_;

        Gtk::Menu * menu_;

        CellRendererThumbnail rendererThumbnail_;

        CellRendererInfo rendererInfo_;

    private:
        void
        configure() throw();
};

} // namespace Solang

#endif // SOLANG_THUMBNAIL_VIEW_H
