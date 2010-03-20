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

#include <glibmm/i18n.h>
#include <sigc++/sigc++.h>

#include "application.h"
#include "browser-renderer.h"
#include "engine.h"
#include "enlarged-renderer.h"
#include "i-renderer.h"
#include "main-window.h"
#include "photo-tag.h"
#include "tag.h"
#include "tag-manager.h"
#include "tag-new-dialog.h"

namespace Solang
{

static const std::string uiFile
    = PACKAGE_DATA_DIR G_DIR_SEPARATOR_S "ui" G_DIR_SEPARATOR_S
          PACKAGE_TARNAME"-tag-manager.ui";

TagManager::TagManager() throw() :
    Plugin(),
    application_(),
    iconFactory_(Gtk::IconFactory::create()),
    actionGroup_(Gtk::ActionGroup::create(
                     Glib::ustring::compose("%1:%2",
                                            __FILE__,
                                            __LINE__))),
    uiID_(0),
    dockItemName_("tag-managers-dock-item"),
    dockItemTitle_(_("Tags")),
    dockItemBehaviour_(GDL_DOCK_ITEM_BEH_NORMAL),
    dockItem_(NULL),
    vBox_( false, 6 ),
    scrolledWindow_(),
    tagView_( ),
    signalRendererChanged_()
{
    Gtk::IconSource icon_source;
    Gtk::IconSet icon_set_tag;

    icon_source.set_filename(PACKAGE_DATA_DIR G_DIR_SEPARATOR_S
                             "pixmaps" G_DIR_SEPARATOR_S
                             "tag-16.png");
    icon_source.set_size(Gtk::IconSize(16));
    icon_set_tag.add_source(icon_source);

    icon_source.set_filename(PACKAGE_DATA_DIR G_DIR_SEPARATOR_S
                             "pixmaps" G_DIR_SEPARATOR_S
                             "tag-22.png");
    icon_source.set_size(Gtk::IconSize(22));
    icon_set_tag.add_source(icon_source);

    Gtk::IconSet icon_set_tag_new;

    icon_source.set_filename(PACKAGE_DATA_DIR G_DIR_SEPARATOR_S
                             "pixmaps" G_DIR_SEPARATOR_S
                             "tag-new-16.png");
    icon_source.set_size(Gtk::IconSize(16));
    icon_set_tag_new.add_source(icon_source);

    icon_source.set_filename(PACKAGE_DATA_DIR G_DIR_SEPARATOR_S
                             "pixmaps" G_DIR_SEPARATOR_S
                             "tag-new-22.png");
    icon_source.set_size(Gtk::IconSize(22));
    icon_set_tag_new.add_source(icon_source);

    icon_source.set_filename(PACKAGE_DATA_DIR G_DIR_SEPARATOR_S
                             "pixmaps" G_DIR_SEPARATOR_S
                             "tag-new-24.png");
    icon_source.set_size(Gtk::IconSize(24));
    icon_set_tag_new.add_source(icon_source);

    iconFactory_->add(Gtk::StockID(PACKAGE_TARNAME"-tag"),
                      icon_set_tag);
    iconFactory_->add(Gtk::StockID(PACKAGE_TARNAME"-tag-new"),
                      icon_set_tag_new);
    iconFactory_->add_default();

    actionGroup_->add(
        Gtk::Action::create(
            "ActionMenuTags", _("_Tags")));

    actionGroup_->add(
        Gtk::Action::create(
            "ActionTagsNew", Gtk::StockID(PACKAGE_TARNAME"-tag-new"),
            _("Create New _Tag...")),
        Gtk::AccelKey(""),
        sigc::mem_fun(*this, &TagManager::on_action_tag_new));

    actionGroup_->add(
        Gtk::Action::create(
            "ActionTagsEdit", Gtk::Stock::EDIT,
            _("_Edit Selected Tag...")),
        Gtk::AccelKey(""),
        sigc::mem_fun(*this, &TagManager::on_action_tag_edit));

    actionGroup_->add(
        Gtk::Action::create(
            "ActionTagsDelete", Gtk::Stock::DELETE,
            _("_Delete Selected Tag")),
        Gtk::AccelKey(""),
        sigc::mem_fun(*this, &TagManager::on_action_tag_delete));

    actionGroup_->add(
        Gtk::Action::create("ActionTagsAttach", Gtk::Stock::ADD,
                            _("_Attach Tag to Selection")),
        Gtk::AccelKey("<control>T"),
        sigc::mem_fun(*this, &TagManager::on_action_apply_tag));

    actionGroup_->add(
        Gtk::Action::create(
            "ActionTagsRemove", Gtk::Stock::REMOVE,
            _("_Remove Tag From Selection")),
        Gtk::AccelKey(""),
        sigc::mem_fun(*this, &TagManager::on_action_remove_tag));

    scrolledWindow_.set_policy(Gtk::POLICY_AUTOMATIC,
                               Gtk::POLICY_AUTOMATIC);
    scrolledWindow_.set_shadow_type(Gtk::SHADOW_IN);

    scrolledWindow_.add(tagView_);
#if 0
    tagView_.signal_().connect(
                sigc::mem_fun(
                    *this, &TagManager::tag_row_activated ) );
#endif
    dockItem_ = gdl_dock_item_new_with_stock(dockItemName_.c_str(),
                                             dockItemTitle_.c_str(),
                                             PACKAGE_TARNAME"-tag",
                                             dockItemBehaviour_);

    gtk_container_add(GTK_CONTAINER(dockItem_),
                      GTK_WIDGET(vBox_.gobj()));
    vBox_.pack_start( scrolledWindow_ );

}

TagManager::~TagManager() throw()
{
    //g_object_unref(dockItem_);
    iconFactory_->remove_default();
}

void
TagManager::init(Application & application)
    throw()
{
    application_ = &application;
    tagView_.set_application( &application );

    RendererRegistry & renderer_registry
                           = application.get_renderer_registry();

    signalRendererChanged_
        = renderer_registry.changed().connect(
              sigc::mem_fun(*this,
                            &TagManager::on_renderer_changed));

    MainWindow & main_window = application.get_main_window();
    main_window.add_dock_object_left_top(GDL_DOCK_OBJECT(dockItem_));

    const UIManagerPtr & ui_manager = main_window.get_ui_manager();

    uiID_ = ui_manager->add_ui_from_file(uiFile);
    if (0 == uiID_)
    {
        // FIXME: error condition.
    }

    ui_manager->insert_action_group(actionGroup_);

    Glib::signal_idle().connect_once(
        sigc::mem_fun(
            *this,
            &TagManager::populate_view),
        Glib::PRIORITY_DEFAULT_IDLE);

    initialized_.emit(*this);
}

void
TagManager::final(Application & application)
    throw()
{
    MainWindow & main_window = application.get_main_window();
    const UIManagerPtr & ui_manager = main_window.get_ui_manager();

    ui_manager->remove_action_group(actionGroup_);
    ui_manager->remove_ui(uiID_);

    signalRendererChanged_.disconnect();

    finalized_.emit(*this);
}

void
TagManager::visit_renderer(BrowserRenderer & browser_renderer) throw()
{
    ui_show();
}

void
TagManager::visit_renderer(EnlargedRenderer & enlarged_renderer)
                           throw()
{
    ui_show();
}

void
TagManager::visit_renderer(SlideshowRenderer & slideshow_renderer)
                           throw()
{
    ui_show();
}

void
TagManager::on_action_tag_new() throw()
{
    TagNewDialog tag_new_dialog;
    tag_new_dialog.set_transient_for(application_->get_main_window());

    const gint response = tag_new_dialog.run();

    switch (response)
    {
        case Gtk::RESPONSE_OK:
        {
            TagPtr tag(new Tag(tag_new_dialog.get_name(),
                               tag_new_dialog.get_description()));
            DatabasePtr db = application_->get_engine().get_db();
            tag->save_async(
                *db,
                sigc::mem_fun(*this,
                              &TagManager::on_updated_tag));
            break;
        }

        case Gtk::RESPONSE_CANCEL:
        case Gtk::RESPONSE_DELETE_EVENT:
        {
            break;
        }

        default:
        {
            break;
        }
    }
}

void
TagManager::on_action_tag_edit() throw()
{
    const ConstTreeSelectionPtr selected = tagView_.get_selection();

    const TagViewModelColumnRecord &rec
                                = tagView_.get_column_records();

    const TreeModelPtr model = tagView_.get_model();
    const TreePathList selected_rows = selected->get_selected_rows();

    for (TreePathList::const_iterator iter = selected_rows.begin();
         selected_rows.end() != iter;
         iter++)
    {
        Gtk::TreeModel::iterator model_iter = model->get_iter(*iter);
        Gtk::TreeModel::Row row = *model_iter;
        TagPtr tag = row[ rec.get_column_tag() ];

        TagNewDialog tag_new_dialog( tag );
        tag_new_dialog.set_transient_for(
                                application_->get_main_window());

        const gint response = tag_new_dialog.run();

        switch (response)
        {
            case Gtk::RESPONSE_OK:
            {
                DatabasePtr db = application_->get_engine().get_db();
                tag->edit_async(
                    tag_new_dialog.get_name(),
                    tag_new_dialog.get_description(),
                    *db,
                    sigc::mem_fun(*this,
                                  &TagManager::on_updated_tag));
                break;
            }

            case Gtk::RESPONSE_CANCEL:
            case Gtk::RESPONSE_DELETE_EVENT:
            {
                break;
            }

            default:
            {
                break;
            }
        }
    }
}

void
TagManager::on_action_tag_delete() throw()
{
    const ConstTreeSelectionPtr selected = tagView_.get_selection();

    const TagViewModelColumnRecord &rec
                                = tagView_.get_column_records();

    DatabasePtr db = application_->get_engine().get_db();

    const TreeModelPtr model = tagView_.get_model();
    const TreePathList selected_rows = selected->get_selected_rows();

    for (TreePathList::const_iterator iter = selected_rows.begin();
         selected_rows.end() != iter;
         iter++)
    {
        Gtk::TreeModel::iterator model_iter = model->get_iter(*iter);
        Gtk::TreeModel::Row row = *model_iter;
        TagPtr tag = row[ rec.get_column_tag() ];

        tag->delete_async(*db,
                          sigc::mem_fun(*this,
                                        &TagManager::on_updated_tag));
    }
}

void
TagManager::on_action_apply_tag() throw()
{
    const TreeSelectionPtr selected = tagView_.get_selection();
    Gtk::TreeModel::iterator item = selected->get_selected();

    if (false == item)
    {
        return;
    }

    const TagViewModelColumnRecord &rec
                                = tagView_.get_column_records();

    Gtk::TreeModel::Row row= (*item);
    TagPtr tag = row[ rec.get_column_tag() ];

    RendererRegistry & renderer_registry
        = application_->get_renderer_registry();
    const IRendererPtr renderer = renderer_registry.get_current();

    const DatabasePtr db = application_->get_engine().get_db();
    PhotoList photos = renderer->get_current_selection();

    for (PhotoList::const_iterator photos_iter = photos.begin();
         photos.end() != photos_iter;
         photos_iter++)
    {
        PhotoTag photo_tag(*photos_iter, tag);
        photo_tag.save_async(*db, sigc::slot<void>());
    }

    return;
}

void
TagManager::on_action_remove_tag() throw()
{
    const TreeSelectionPtr selected = tagView_.get_selection();
    Gtk::TreeModel::iterator item = selected->get_selected();

    if (false == item)
    {
        return;
    }

    const TagViewModelColumnRecord &rec
                                = tagView_.get_column_records();

    Gtk::TreeModel::Row row= (*item);
    TagPtr tag = row[ rec.get_column_tag() ];

    RendererRegistry & renderer_registry
        = application_->get_renderer_registry();
    const IRendererPtr renderer = renderer_registry.get_current();

    const DatabasePtr db = application_->get_engine().get_db();
    PhotoList photos = renderer->get_current_selection();

    for (PhotoList::const_iterator photos_iter = photos.begin();
         photos.end() != photos_iter;
         photos_iter++)
    {
        PhotoTag photo_tag(*photos_iter, tag);
        photo_tag.delete_async(*db, sigc::slot<void>());
    }

    return;
}

void
TagManager::on_get_tags(TagList & tags) throw()
{
    tagView_.populate(tags);
}

void
TagManager::on_renderer_changed(RendererRegistry & renderer_registry)
                                throw()
{
    if (false == gdl_dock_object_is_bound(GDL_DOCK_OBJECT(dockItem_)))
    {
        return;
    }

    const IRendererPtr & renderer = renderer_registry.get_current();
    renderer->receive_plugin(*this);
}

void
TagManager::on_updated_tag() throw()
{
    populate_view();
}

void
TagManager::populate_view() throw()
{
    Engine & engine = application_->get_engine();
    engine.get_tags_async(sigc::mem_fun(
                              *this,
                              &TagManager::on_get_tags));
}

void
TagManager::ui_hide() throw()
{
    actionGroup_->set_visible(false);

    GdlDockItem * const dock_item = GDL_DOCK_ITEM(dockItem_);

    if (true == GDL_DOCK_OBJECT_ATTACHED(dock_item))
    {
        gdl_dock_item_hide_item(GDL_DOCK_ITEM(dockItem_));
    }
}

void
TagManager::ui_show() throw()
{
    actionGroup_->set_visible(true);

    GdlDockItem * const dock_item = GDL_DOCK_ITEM(dockItem_);

    if (false == GDL_DOCK_OBJECT_ATTACHED(dock_item))
    {
        gdl_dock_item_show_item(GDL_DOCK_ITEM(dockItem_));
    }
}

} // namespace Solang
