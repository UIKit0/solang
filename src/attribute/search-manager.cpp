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

#include <vector>

#include <glibmm/i18n.h>
#include <gdkmm.h>

#include "application.h"
#include "browser-renderer.h"
#include "engine.h"
#include "enlarged-renderer.h"
#include "free-text.h"
#include "main-window.h"
#include "search-basket-column-record.h"
#include "search-manager.h"

namespace Solang
{

const std::string SearchManager::uiFile_
    = PACKAGE_DATA_DIR G_DIR_SEPARATOR_S "ui" G_DIR_SEPARATOR_S
          PACKAGE_TARNAME"-search-manager.ui";

SearchManager::SearchManager() throw() :
    Plugin(),
    actionGroup_(Gtk::ActionGroup::create(
                     Glib::ustring::compose("%1:%2",
                                            __FILE__,
                                            __LINE__))),
    uiIDBasket_(0),
    dockItemName_("search-basket-dock-item"),
    dockItemTitle_(_("Search")),
    dockItemBehaviour_(GDL_DOCK_ITEM_BEH_NORMAL),
    dockItem_(NULL),
    vBox_( false, 6 ),
    entry_(),
    scrolledWindow_(),
    listStore_(Gtk::ListStore::create(SearchBasketColumnRecord())),
    searchBasket_(listStore_),
    application_( NULL ),
    signalRendererChanged_()
{
    actionGroup_->add(
        Gtk::Action::create(
            "ActionSelectAll", Gtk::Stock::SELECT_ALL,
            _("Select _All"),
            _("Select all the filters in this list")),
        Gtk::AccelKey("<control>A"),
        sigc::mem_fun(*this,
                      &SearchManager::on_action_select_all));

    {
        const ActionPtr action = Gtk::Action::create(
            "ActionRemoveSelected", Gtk::Stock::REMOVE,
            _("_Remove"),
            _("Remove the selected filters from this list"));

        action->set_visible(false);
        actionGroup_->add(
            action, Gtk::AccelKey("Delete"),
            sigc::mem_fun(*this,
                          &SearchManager::on_action_remove_selected));
    }

    vBox_.pack_start(entry_, Gtk::PACK_SHRINK, 0);

    scrolledWindow_.set_policy(Gtk::POLICY_AUTOMATIC,
                               Gtk::POLICY_AUTOMATIC);
    scrolledWindow_.set_shadow_type(Gtk::SHADOW_IN);

    SearchBasketColumnRecord model_column_record;
    searchBasket_.append_column("",
        model_column_record.get_column_pixbuf());
    searchBasket_.append_column("",
        model_column_record.get_column_description());

    SearchBasketColumnRecord tmp;
    searchBasket_.set_tooltip_column( tmp.get_column_description_num() );

    scrolledWindow_.add(searchBasket_);

    dockItem_ = gdl_dock_item_new_with_stock(dockItemName_.c_str(),
                                             dockItemTitle_.c_str(),
                                             GTK_STOCK_FIND,
                                             dockItemBehaviour_);
    gtk_container_add(GTK_CONTAINER(dockItem_),
                      GTK_WIDGET(vBox_.gobj()));

    vBox_.pack_start( scrolledWindow_, Gtk::PACK_EXPAND_WIDGET,0 );

    entry_.signal_changed().connect(
        sigc::mem_fun(*this, &SearchManager::on_entry_changed));

    std::vector<Gtk::TargetEntry> targets;
    targets.push_back(Gtk::TargetEntry("STRING",
                                       Gtk::TARGET_SAME_APP, 0));
    targets.push_back(Gtk::TargetEntry("UTF8_STRING",
                                       Gtk::TARGET_SAME_APP, 0));
    searchBasket_.enable_model_drag_dest(targets, Gdk::ACTION_COPY);

    searchBasket_.signal_drag_data_received().connect_notify(
        sigc::mem_fun(
            *this, &SearchManager::on_drag_data_received));

    const TreeSelectionPtr selection = searchBasket_.get_selection();
    selection->signal_changed().connect(
        sigc::mem_fun(*this, &SearchManager::on_selection_changed));

    const UIManagerPtr & ui_manager = searchBasket_.get_ui_manager();
    uiIDBasket_ = ui_manager->add_ui_from_file(uiFile_);

    if (0 != uiIDBasket_)
    {
        ui_manager->insert_action_group(actionGroup_);
    }
}

SearchManager::~SearchManager() throw()
{
    //g_object_unref(dockItem_);

    const UIManagerPtr & ui_manager = searchBasket_.get_ui_manager();

    if (0 != uiIDBasket_)
    {
        ui_manager->remove_action_group(actionGroup_);
        ui_manager->remove_ui(uiIDBasket_);
        uiIDBasket_ = 0;
    }
}

void
SearchManager::init(Application & application) throw()
{
    application_ = &application;
    Engine & engine = application.get_engine();
    engine.get_criterion_repo().register_source( this );

    RendererRegistry & renderer_registry
                           = application.get_renderer_registry();

    signalRendererChanged_
        = renderer_registry.changed().connect(
              sigc::mem_fun(*this,
                            &SearchManager::on_renderer_changed));

    MainWindow & main_window = application.get_main_window();
    main_window.add_dock_object_left_bottom(
                    GDL_DOCK_OBJECT(dockItem_));

    initialized_.emit(*this);
}

void
SearchManager::final(Application & application) throw()
{
    signalRendererChanged_.disconnect();

    finalized_.emit(*this);
}

void
SearchManager::visit_renderer(BrowserRenderer & browser_renderer)
                             throw()
{
    ui_show();
}

void
SearchManager::visit_renderer(EnlargedRenderer & enlarged_renderer)
                             throw()
{
    ui_show();
}

void
SearchManager::visit_renderer(SlideshowRenderer & slideshow_renderer)
                             throw()
{
}

void
SearchManager::on_action_select_all() throw()
{
    const TreeSelectionPtr selection = searchBasket_.get_selection();
    selection->select_all();
}

void
SearchManager::on_action_remove_selected() throw()
{
    const ConstTreeSelectionPtr selection
                                    = searchBasket_.get_selection();

    const TreePathList selected_rows = selection->get_selected_rows();

    // NB: Erase the paths by traversing the list backwards. Otherwise
    //     on erasing each path the remaining ones will be
    //     invalidated.

    for (TreePathList::const_reverse_iterator iter
             = selected_rows.rbegin();
         selected_rows.rend() != iter;
         iter++)
    {
        const Gtk::TreeModel::iterator model_iter
            = listStore_->get_iter(*iter);
        listStore_->erase(model_iter);
    }

    apply_criterion();
    return;
}

void
SearchManager::on_drag_data_received(
                  const DragContextPtr & drag_context,
                  gint x, gint y,
                  const Gtk::SelectionData & data,
                  guint info, guint time) throw()
{
    Glib::ustring key = data.get_data_as_string();
    bool status = add_item_to_list( key );
    drag_context->drag_finish(status, false, 0);
    return;
}

void
SearchManager::on_entry_changed() throw()
{
    static sigc::connection connection;

    connection.disconnect();

    connection
        = Glib::signal_timeout().connect_seconds(
              sigc::bind_return(
                  sigc::mem_fun(*this,
                                &SearchManager::apply_criterion),
                  false),
              1, Glib::PRIORITY_DEFAULT);
}

void
SearchManager::on_renderer_changed(
                  RendererRegistry & renderer_registry) throw()
{
    if (false == gdl_dock_object_is_bound(GDL_DOCK_OBJECT(dockItem_)))
    {
        return;
    }

    const IRendererPtr & renderer = renderer_registry.get_current();
    renderer->receive_plugin(*this);
}

void
SearchManager::on_selection_changed() throw()
{
    const ActionPtr action = actionGroup_->get_action(
                                 "ActionRemoveSelected");
    const TreeSelectionPtr selection = searchBasket_.get_selection();

    action->set_visible((0 == selection->count_selected_rows())
                        ? false
                        : true);
}

bool
SearchManager::add_item_to_list( const Glib::ustring &key )
{
    DragDropCriteriaMap &dragItemMap
                    = application_->get_drag_drop_map();
    DragDropCriteriaMap::iterator it = dragItemMap.find( key );

    if( it == dragItemMap.end() )
        return false;

    SearchBasketColumnRecord model_column_record;

    IPhotoSearchCriteriaPtr criteria = (*it).second;
    Glib::ustring iconPath = criteria->get_criteria_icon_path();
    Glib::ustring description
                    = criteria->get_criteria_description();

    //Check for duplicate
    Gtk::TreeModel::Children children = listStore_->children();
    for( Gtk::TreeModel::const_iterator current = children.begin();
                                current != children.end(); current++)
    {
        Gtk::TreeModel::Row row = (*current);
        IPhotoSearchCriteriaPtr tag
                = row[ model_column_record.get_column_criteria()];

        if( tag->get_criteria_description() == description )
        {
            return false;
        }
    }

    //Add
    Gtk::TreeModel::iterator model_iter = listStore_->append();
    Gtk::TreeModel::Row row = *model_iter;
    PixbufPtr icon = Gdk::Pixbuf::create_from_file( iconPath );

    row[ model_column_record.get_column_pixbuf() ] = icon;
    row[ model_column_record.get_column_description() ] = description;
    row[ model_column_record.get_column_criteria() ] = criteria;

    //remove this entry from the map
    dragItemMap.erase( it );

    apply_criterion();
    return true;
}

void
SearchManager::apply_criterion()
{
    Engine & engine = application_->get_engine();
    engine.criteria_changed();
    return;

}

void
SearchManager::get_criterion(
                    IPhotoSearchCriteriaList &criterion) const throw()
{
    SearchBasketColumnRecord model_column_record;
    Gtk::TreeModel::Children children = listStore_->children();
    for( Gtk::TreeModel::const_iterator current = children.begin();
                                current != children.end(); current++)
    {
        Gtk::TreeModel::Row row = (*current);
        IPhotoSearchCriteriaPtr tag
                = row[ model_column_record.get_column_criteria()];
        criterion.push_back( tag );
    }

    if (0 != entry_.get_text_length())
    {
        const IPhotoSearchCriteriaPtr free_text(
            new FreeText(entry_.get_text()));
        criterion.push_back(free_text);
    }

    return;
}

void
SearchManager::ui_hide() throw()
{
    GdlDockItem * const dock_item = GDL_DOCK_ITEM(dockItem_);

    if (true == GDL_DOCK_OBJECT_ATTACHED(dock_item))
    {
        gdl_dock_item_hide_item(GDL_DOCK_ITEM(dockItem_));
    }
}

void
SearchManager::ui_show() throw()
{
    GdlDockItem * const dock_item = GDL_DOCK_ITEM(dockItem_);

    if (false == GDL_DOCK_OBJECT_ATTACHED(dock_item))
    {
        gdl_dock_item_show_item(GDL_DOCK_ITEM(dockItem_));
    }
}

} // namespace Solang
