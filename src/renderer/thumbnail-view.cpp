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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <vector>
#include <gtk/gtk.h>

#include "browser-model-column-record.h"
#include "photo.h"
#include "thumbnail-view.h"

namespace Solang
{

static const std::string uiFile
    = PACKAGE_DATA_DIR"/"PACKAGE_TARNAME"/ui/"
          PACKAGE_TARNAME"-thumbnail-popup.ui";

static void
thumbnail_view_set_renderer_thumbnail_data(
    GtkCellLayout * cell_layout,
    GtkCellRenderer * cell_renderer,
    GtkTreeModel * tree_model,
    GtkTreeIter * tree_iter,
    gpointer user_data)
{
    CellRendererThumbnail * const renderer_thumbnail
        = dynamic_cast<CellRendererThumbnail *>(Glib::wrap(
                                                    cell_renderer,
                                                    false));
    const TreeModelPtr list_store = Glib::wrap(tree_model, true);

    // Can not convert a GtkTreeIter * to a Gtk::TreeModel::iterator *
    // directly.
    const Gtk::TreePath tree_path(gtk_tree_model_get_path(tree_model,
                                                          tree_iter),
                                  false);
    const Gtk::TreeModel::const_iterator iter = list_store->get_iter(
                                                    tree_path);

    Gtk::TreeModel::Row row = *iter;
    BrowserModelColumnRecord model_column_record;
    const PhotoPtr photo = row[model_column_record.get_column_photo()];
    PixbufPtr pixbuf = photo->get_thumbnail_buffer();

    renderer_thumbnail->property_pixbuf().set_value(pixbuf);
    renderer_thumbnail->set_photo(photo);
}

ThumbnailView::ThumbnailView(gint thumbnail_renderer_width,
                             gint thumbnail_renderer_height) throw() :
    Gtk::IconView(),
    selectedItems_(0),
    actionGroup_(Gtk::ActionGroup::create(
                     Glib::ustring::compose("%1:%2",
                                            __FILE__,
                                            __LINE__))),
    uiManager_(Gtk::UIManager::create()),
    uiID_(uiManager_->add_ui_from_file(uiFile)),
    menu_(NULL),
    rendererThumbnail_()
{
    configure(thumbnail_renderer_width, thumbnail_renderer_height);
}

ThumbnailView::ThumbnailView(const TreeModelPtr & model,
                             gint thumbnail_renderer_width,
                             gint thumbnail_renderer_height) throw() :
    Gtk::IconView(model),
    selectedItems_(0),
    actionGroup_(Gtk::ActionGroup::create(
                     Glib::ustring::compose("%1:%2",
                                            __FILE__,
                                            __LINE__))),
    uiManager_(Gtk::UIManager::create()),
    uiID_(uiManager_->add_ui_from_file(uiFile)),
    menu_(NULL),
    rendererThumbnail_()
{
    configure(thumbnail_renderer_width, thumbnail_renderer_height);
}

ThumbnailView::~ThumbnailView() throw()
{
    uiManager_->remove_action_group(actionGroup_);
    uiManager_->remove_ui(uiID_);
}

void
ThumbnailView::configure(gint thumbnail_renderer_width,
                         gint thumbnail_renderer_height) throw()
{
    set_orientation(Gtk::ORIENTATION_VERTICAL);
    set_selection_mode(Gtk::SELECTION_MULTIPLE);
    set_spacing(0);
    set_column_spacing(1);
    set_row_spacing(1);
    set_has_tooltip(true);

    actionGroup_->add(Gtk::Action::create("ActionOpenWithMenu",
                                          "Open _With"));
    uiManager_->insert_action_group(actionGroup_);

    Gtk::Widget * menu = uiManager_->get_widget("/ThumbnailPopup");
    menu_ = dynamic_cast<Gtk::Menu *>(menu);
    menu_->accelerate(*this);

    GtkCellLayout * const self
        = GTK_CELL_LAYOUT(Gtk::IconView::gobj());
    GtkCellRenderer * const renderer_thumbnail
        = GTK_CELL_RENDERER(rendererThumbnail_.gobj());

    gtk_cell_layout_pack_start(self, renderer_thumbnail, FALSE);
    gtk_cell_layout_set_cell_data_func(
        self,
        renderer_thumbnail,
        thumbnail_view_set_renderer_thumbnail_data,
        0,
        0);

    rendererThumbnail_.property_width().set_value(
                           thumbnail_renderer_width);
    rendererThumbnail_.property_height().set_value(
                           thumbnail_renderer_height);

    std::vector<Gtk::TargetEntry> targets;
    targets.push_back(Gtk::TargetEntry("text/uri-vector",
                                       Gtk::TARGET_OTHER_APP, 0));
    enable_model_drag_source(targets, Gdk::MODIFIER_MASK,
                             Gdk::ACTION_COPY);

    this->signal_drag_data_get().connect(sigc::mem_fun(
        *this, &ThumbnailView::on_drag_data_get_cb));

    show_all_children();
}

PhotoPtr
ThumbnailView::get_photo_from_path(const Gtk::TreeModel::Path & path)
                                   throw()
{
    const TreeModelPtr & model = get_model();
    g_object_ref(model->gobj());

    Gtk::TreeModel::iterator model_iter = model->get_iter(path);
    Gtk::TreeModel::Row row = *model_iter;

    BrowserModelColumnRecord model_column_record;
    const PhotoPtr photo = row[model_column_record.get_column_photo()];

    return photo;
}

PhotoList
ThumbnailView::get_selected_photos() throw()
{
    PhotoList photos;
    const TreePathList items = get_selected_items();
    TreePathList::const_iterator iter;

    for (iter = items.begin(); items.end() != iter; iter++)
    {
        const Gtk::TreeModel::Path & path = *iter;
        photos.push_back(get_photo_from_path(path));
    }

    return photos;
}

gint
ThumbnailView::get_thumbnail_width() const throw()
{
    return rendererThumbnail_.property_width().get_value();
}

gint
ThumbnailView::get_thumbnail_height() const throw()
{
    return rendererThumbnail_.property_height().get_value();
}

const UIManagerPtr &
ThumbnailView::get_ui_manager() throw()
{
    return uiManager_;
}

bool
ThumbnailView::on_button_press_event(GdkEventButton * event) throw()
{
    bool return_value = Gtk::IconView::on_button_press_event(event);

    // Ignore double-clicks and triple-clicks.
    if (3 == event->button && GDK_BUTTON_PRESS == event->type)
    {
        Gtk::TreeModel::Path path = get_path_at_pos(
                                        static_cast<gint>(event->x),
                                        static_cast<gint>(event->y));

        if (true == path.empty())
        {
            return false;
        }

        if (false == path_is_selected(path))
        {
            unselect_all();
            select_path(path);
            // set_cursor(path, 0, false);
        }

        menu_->popup(event->button, event->time);
    }

    return return_value;
}

void
ThumbnailView::on_drag_data_get_cb(const DragContextPtr & drag_context,
                                   Gtk::SelectionData & data,
                                   guint info, guint time) throw()
{
    // Gtk::IconView::on_drag_data_get(drag_context, data, info, time);

    PhotoList photos = get_selected_photos();
    PhotoList::iterator photo_iter;
    std::vector<std::string> uris;

    for (photo_iter = photos.begin(); photos.end() != photo_iter;
         photo_iter++)
    {
        const PhotoPtr photo = *photo_iter;
        uris.push_back(photo->get_uri());
    }

    data.set_uris(uris);
}

void
ThumbnailView::popup_menu(GdkEventButton * event) throw()
{
    guint button;
    guint32 time;

    if (NULL != event)
    {
        button = event->button;
        time = event->time;
    }
    else
    {
         button = 0;
        time = gtk_get_current_event_time();
    }

    menu_->popup(button, time);
}

void
ThumbnailView::set_thumbnail_width(gint width) throw()
{
    rendererThumbnail_.property_width().set_value(width);
}

void
ThumbnailView::set_thumbnail_height(gint height) throw()
{
    rendererThumbnail_.property_height().set_value(height);
}

} // namespace Solang
