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

#ifndef SOLANG_SEARCH_BASKET_H
#define SOLANG_SEARCH_BASKET_H

#include <string>

#include <gdkmm.h>
#include <gdl/gdl.h>
#include <glibmm.h>
#include <gtkmm.h>
#include <sigc++/sigc++.h>

#include "plugin.h"
#include "search-criterion-source.h"

namespace Solang
{

class Application;

class SearchBasket :
    public Plugin,
    public sigc::trackable,
    public SearchCriterionSource
{
    public:
        SearchBasket() throw();

        virtual
        ~SearchBasket() throw();

        virtual void
        init(Application & application) throw();

        virtual void
        final(Application & application) throw();

        bool
        add_item_to_list( const Glib::ustring &key );

        virtual void
        get_criterion(PhotoSearchCriteriaList &) const throw();

    protected:
        void
        remove_selected();

        void
        apply_criterion();

        void
        clear_criterion();

        void
        on_drag_data_received(const DragContextPtr & drag_context,
                              gint x, gint y,
                              const Gtk::SelectionData & data,
                              guint info, guint time) throw();

        const std::string dockItemName_;

        const Glib::ustring dockItemTitle_;

        GdlDockItemBehavior dockItemBehaviour_;

        GtkWidget * dockItem_;

        Gtk::VBox vBox_;

        Gtk::CheckButton autoApplyCheck_;

        Gtk::HBox hBox_;

        Gtk::Button applyButton_;
        Gtk::Image  applyImage_;

        Gtk::Button clearButton_;
        Gtk::Image  clearImage_;

        Gtk::Button trashButton_;
        Gtk::Image  trashImage_;

        Gtk::ScrolledWindow scrolledWindow_;

        ListStorePtr listStore_;

        Gtk::TreeView treeView_;

        ApplicationPtr application_;

    private:
};

} // namespace Solang

#endif // SOLANG_SEARCH_BASKET_H