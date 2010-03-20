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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "search-basket.h"

namespace Solang
{

const std::string SearchBasket::uiFile_
    = PACKAGE_DATA_DIR G_DIR_SEPARATOR_S "ui" G_DIR_SEPARATOR_S
          PACKAGE_TARNAME"-search-basket.ui";

SearchBasket::SearchBasket(const TreeModelPtr & model) throw() :
    Gtk::TreeView(model),
    uiManager_(Gtk::UIManager::create()),
    uiID_(uiManager_->add_ui_from_file(uiFile_)),
    menu_(0)
{
    set_enable_search(true);
    set_grid_lines(Gtk::TREE_VIEW_GRID_LINES_NONE);
    set_headers_clickable(false);
    set_headers_visible(false);

    const TreeSelectionPtr selection = get_selection();
    selection->set_mode(Gtk::SELECTION_MULTIPLE);

    Gtk::Widget * const menu = uiManager_->get_widget(
                                               "/SearchBasketPopup");
    menu_ = dynamic_cast<Gtk::Menu *>(menu);
    menu_->accelerate(*this);
}

SearchBasket::~SearchBasket() throw()
{
    uiManager_->remove_ui(uiID_);
}

const UIManagerPtr &
SearchBasket::get_ui_manager() throw()
{
    return uiManager_;
}

bool
SearchBasket::on_button_press_event(GdkEventButton * event) throw()
{
    bool return_value;
    const TreeModelPtr model = get_model();
    const TreeSelectionPtr selection = get_selection();

    if (3 == event->button
        && GDK_BUTTON_PRESS == event->type
        && false == model->children().empty())
    {
        Gtk::TreeModel::Path path;
        const bool result = get_path_at_pos(
                                static_cast<gint>(event->x),
                                static_cast<gint>(event->y),
                                path);

        if (true == result && false == path.empty())
        {
            if (false == selection->is_selected(path))
            {
                selection->unselect_all();
                selection->select(path);
                // set_cursor(path, 0, false);
            }
        }
        else
        {
            selection->unselect_all();
        }

        menu_->popup(event->button, event->time);
        return_value = true;
    }
    else
    {
        selection->unselect_all();
        return_value = Gtk::TreeView::on_button_press_event(event);
    }

    return return_value;
}

} // namespace Solang
