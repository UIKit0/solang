/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * Copyright (C) 2009, 2010 Debarshi Ray <rishi@gnu.org>
 * Copyright (C) 2009 Santanu Sinha <santanu.sinha@gmail.com>
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

#include <sstream>
#include <vector>

#include <glibmm/i18n.h>

#include "application.h"
#include "browser-model-column-record.h"
#include "browser-renderer.h"
#include "engine.h"
#include "export-queue-operations.h"
#include "i-plugin.h"
#include "i-renderer-selector.h"
#include "main-window.h"
#include "photo.h"
#include "scale-action.h"

namespace Solang
{

static const guint lowerZoomValue = 20;
static const guint higherZoomValue = 100;
static const guint initialZoomValue
                       = lowerZoomValue
                         + (higherZoomValue - lowerZoomValue) / 2;
static const guint stepZoomValue = 5;

static const gint thumbnailRendererWidth = 156;
static const gint thumbnailRendererHeight = 118;

static const double ratioWidth
                        = static_cast<double>(thumbnailRendererWidth)
                          / static_cast<double>(initialZoomValue);
static const double ratioHeight
                        = static_cast<double>(thumbnailRendererHeight)
                          / static_cast<double>(initialZoomValue);

static const std::string uiFile
    = PACKAGE_DATA_DIR G_DIR_SEPARATOR_S "ui" G_DIR_SEPARATOR_S
          PACKAGE_TARNAME"-browser-renderer.ui";
static const std::string uiFileThumbnail
    = PACKAGE_DATA_DIR G_DIR_SEPARATOR_S "ui" G_DIR_SEPARATOR_S
          PACKAGE_TARNAME"-browser-renderer-thumbnail.ui";

BrowserRenderer::BrowserRenderer() throw() :
    IRenderer(),
    Plugin(),
    application_(NULL),
    iconFactory_(Gtk::IconFactory::create()),
    actionGroup_(Gtk::ActionGroup::create(
                     Glib::ustring::compose("%1:%2",
                                            __FILE__,
                                            __LINE__))),
    uiID_(0),
    uiIDThumbnail_(0),
    dockItemName_("browser-dock-item"),
    dockItemTitle_("Browser"),
    dockItemBehaviour_(GDL_DOCK_ITEM_BEH_NO_GRIP),
    dockItem_(NULL),
    vBox_(false, 6),
    hBox_(false, 12),
    paginationBar_(),
    dummyLabel_(),
    scrolledWindow_(),
    treeModelFilter_(),
    thumbnailView_(thumbnailRendererWidth, thumbnailRendererHeight),
    zoomValue_(initialZoomValue),
    pageNum_(-1),
    contextID_(),
    signalInitEnd_(),
    signalListStoreChangeBegin_(),
    signalListStoreChangeEnd_(),
    signalSelectionChanged_(),
    signalSwitchPage_()
{
    Gtk::IconSource icon_source;
    Gtk::IconSet icon_set_mode_browse;

    icon_source.set_filename(PACKAGE_DATA_DIR G_DIR_SEPARATOR_S
                             "pixmaps" G_DIR_SEPARATOR_S
                             "mode-browse-16.png");
    icon_source.set_size(Gtk::IconSize(16));
    icon_set_mode_browse.add_source(icon_source);

    icon_source.set_filename(PACKAGE_DATA_DIR G_DIR_SEPARATOR_S
                             "pixmaps" G_DIR_SEPARATOR_S
                             "mode-browse-22.png");
    icon_source.set_size(Gtk::IconSize(22));
    icon_set_mode_browse.add_source(icon_source);

    icon_source.set_filename(PACKAGE_DATA_DIR G_DIR_SEPARATOR_S
                             "pixmaps" G_DIR_SEPARATOR_S
                             "tag-new-24.png");
    icon_source.set_size(Gtk::IconSize(24));
    icon_set_mode_browse.add_source(icon_source);

    icon_source.set_filename(PACKAGE_DATA_DIR G_DIR_SEPARATOR_S
                             "pixmaps" G_DIR_SEPARATOR_S
                             "mode-browse-32.png");
    icon_source.set_size(Gtk::IconSize(32));
    icon_set_mode_browse.add_source(icon_source);

    iconFactory_->add(Gtk::StockID(PACKAGE_TARNAME"-mode-browse"),
                      icon_set_mode_browse);
    iconFactory_->add_default();

    actionGroup_->add(
        Gtk::Action::create(
            "ActionMenuEdit", _("_Edit")));

    actionGroup_->add(
        Gtk::Action::create(
            "ActionEditAddToExportQueue", Gtk::Stock::ADD,
            _("_Add to Export Queue")),
        sigc::mem_fun(*this,
                      &BrowserRenderer::on_action_add_to_export_queue));

    actionGroup_->add(
        Gtk::Action::create(
            "ActionViewBrowserSlideshow",
            Gtk::StockID(PACKAGE_TARNAME"-slideshow-play"),
            _("_Slideshow"),
            _("Start a slideshow view of the photos")),
        Gtk::AccelKey("F5"),
        sigc::mem_fun(*this,
                      &BrowserRenderer::on_action_view_slideshow));

    {
        const Glib::RefPtr<ScaleAction> scale_action
            = ScaleAction::create(
                  "ActionViewBrowserZoom",
                  _("Enlarge or shrink the thumbnails"),
                  Gtk::Adjustment(initialZoomValue,
                                  lowerZoomValue,
                                  higherZoomValue,
                                  stepZoomValue,
                                  10 * stepZoomValue,
                                  0));

        scale_action->reference();

        actionGroup_->add(scale_action);

        scale_action->signal_value_changed().connect(
            sigc::bind(
                sigc::mem_fun(
                    *this,
                    &BrowserRenderer::on_action_view_zoom_changed),
                scale_action));

        actionGroup_->add(
            Gtk::Action::create(
                "ActionViewBrowserZoomIn", Gtk::Stock::ZOOM_IN,
                _("_Zoom In"),
                _("Enlarge the thumbnails")),
            Gtk::AccelKey("<control>plus"),
            sigc::bind(
                sigc::mem_fun(
                    *this,
                    &BrowserRenderer::on_action_view_zoom_in),
                scale_action));

        actionGroup_->add(
            Gtk::Action::create(
                "ActionViewBrowserZoomOut", Gtk::Stock::ZOOM_OUT,
                _("_Zoom Out"),
                _("Shrink the thumbnails")),
            Gtk::AccelKey("<control>minus"),
            sigc::bind(
                sigc::mem_fun(
                    *this,
                    &BrowserRenderer::on_action_view_zoom_out),
                scale_action));
    }

    actionGroup_->add(
        Gtk::Action::create(
            "ActionBrowserGoMenu", _("_Go")));

    {
        ActionPtr action = Gtk::Action::create(
            "ActionGoBrowserPreviousPage", Gtk::Stock::GO_BACK,
            _("_Previous Page"),
            _("Go to the previous page in the collection"));

        action->property_short_label().set_value(
            C_("Navigation", "Previous"));
        action->property_is_important().set_value(true);
        action->set_sensitive(false);
        actionGroup_->add(
            action, Gtk::AccelKey("<alt>Page_Up"),
            sigc::mem_fun(*this,
                          &BrowserRenderer::on_action_go_previous));
    }

    {
        ActionPtr action = Gtk::Action::create(
            "ActionGoBrowserNextPage", Gtk::Stock::GO_FORWARD,
            _("_Next Page"),
            _("Go to the next page in the collection"));

        action->property_short_label().set_value(
            C_("Navigation", "Next"));
        action->property_is_important().set_value(true);
        actionGroup_->add(
            action, Gtk::AccelKey("<alt>Page_Down"),
            sigc::mem_fun(*this,
                          &BrowserRenderer::on_action_go_next));
    }

    {
        ActionPtr action = Gtk::Action::create(
            "ActionGoBrowserFirstPage", Gtk::Stock::GOTO_FIRST,
            _("_First Page"),
            _("Go to the first page in the collection"));

        action->set_sensitive(false);
        actionGroup_->add(
            action, Gtk::AccelKey("<alt>Home"),
            sigc::mem_fun(*this,
                          &BrowserRenderer::on_action_go_first));
    }

    actionGroup_->add(
        Gtk::Action::create(
            "ActionGoBrowserLastPage", Gtk::Stock::GOTO_LAST,
            _("_Last Page"),
            _("Go to the last page in the collection")),
        Gtk::AccelKey("<alt>End"),
        sigc::mem_fun(*this, &BrowserRenderer::on_action_go_last));

    vBox_.pack_start(hBox_, Gtk::PACK_SHRINK, 0);

    scrolledWindow_.set_policy(Gtk::POLICY_AUTOMATIC,
                               Gtk::POLICY_AUTOMATIC);
    scrolledWindow_.set_shadow_type(Gtk::SHADOW_IN);

    scrolledWindow_.add(thumbnailView_);
    vBox_.pack_start(scrolledWindow_, Gtk::PACK_EXPAND_WIDGET, 0);

    dockItem_ = gdl_dock_item_new_with_stock(
                    dockItemName_.c_str(),
                    dockItemTitle_.c_str(),
                    PACKAGE_TARNAME"-mode-browse",
                    dockItemBehaviour_);
    gtk_container_add(GTK_CONTAINER(dockItem_),
                      GTK_WIDGET(vBox_.gobj()));

    Gtk::Widget * const dock_item = Glib::wrap(dockItem_, false);
    dock_item->signal_parent_changed().connect(
        sigc::mem_fun(*this,
                      &BrowserRenderer::on_dock_item_parent_changed));

    paginationBar_.limits_changed().connect(
        sigc::mem_fun(*this,
                      &BrowserRenderer::on_limits_changed));

    const UIManagerPtr & ui_manager = thumbnailView_.get_ui_manager();
    uiIDThumbnail_ = ui_manager->add_ui_from_file(uiFileThumbnail);

    if (0 != uiIDThumbnail_)
    {
        ui_manager->insert_action_group(actionGroup_);
    }
}

BrowserRenderer::~BrowserRenderer() throw()
{
    //g_object_unref(dockItem_);
    iconFactory_->remove_default();

    const UIManagerPtr & ui_manager = thumbnailView_.get_ui_manager();

    if (0 != uiIDThumbnail_)
    {
        ui_manager->remove_action_group(actionGroup_);
        ui_manager->remove_ui(uiIDThumbnail_);
        uiIDThumbnail_ = 0;
    }
}

void
BrowserRenderer::init(Application & application) throw()
{
    application_ = &application;

    RendererRegistry & renderer_registry
        = application.get_renderer_registry();
    renderer_registry.add(this);

    const ListStorePtr & list_store = application.get_list_store();

    treeModelFilter_ = Gtk::TreeModelFilter::create(list_store);
    treeModelFilter_->set_visible_func(sigc::mem_fun(*this,
                                       &BrowserRenderer::on_visible));
    thumbnailView_.set_model(treeModelFilter_);

    MainWindow & main_window = application.get_main_window();
    main_window.add_dock_object_center(GDL_DOCK_OBJECT(dockItem_));

    Gtk::Statusbar & status_bar = main_window.get_status_bar();
    contextID_ = status_bar.get_context_id(
                     Glib::ustring::compose("%1", G_STRLOC));

    const UIManagerPtr & ui_manager = main_window.get_ui_manager();

    uiID_ = ui_manager->add_ui_from_file(uiFile);
    if (0 == uiID_)
    {
        // FIXME: error condition.
    }

    ui_manager->insert_action_group(actionGroup_);

    // NB: This should not be done in the constructor because if
    //     'false == application_' then the handler will crash.
    //     Better safe than sorry.
    signalItemActivated_
        = thumbnailView_.signal_item_activated().connect(
              sigc::mem_fun(*this,
                            &BrowserRenderer::on_item_activated));

    signalListStoreChangeBegin_
        = application.list_store_change_begin().connect(
              sigc::mem_fun(*this,
                            &BrowserRenderer::on_list_store_change_begin));

    signalListStoreChangeEnd_
        = application.list_store_change_end().connect(
              sigc::mem_fun(*this,
                            &BrowserRenderer::on_list_store_change_end));

    signalSelectionChanged_
        = thumbnailView_.signal_selection_changed().connect(
              sigc::mem_fun(*this,
                            &BrowserRenderer::on_selection_changed));

    // initialized_.emit(*this);
}

void
BrowserRenderer::final(Application & application) throw()
{
    signalItemActivated_.disconnect();
    signalListStoreChangeBegin_.disconnect();
    signalListStoreChangeEnd_.disconnect();
    signalSelectionChanged_.disconnect();
    signalSwitchPage_.disconnect();

    MainWindow & main_window = application.get_main_window();
    const UIManagerPtr & ui_manager = main_window.get_ui_manager();

    if (0 != uiID_)
    {
        ui_manager->remove_action_group(actionGroup_);
        ui_manager->remove_ui(uiID_);
        uiID_ = 0;
    }

    treeModelFilter_.reset();

    main_window.undock_object_center(GDL_DOCK_OBJECT(dockItem_));

    RendererRegistry & renderer_registry
        = application.get_renderer_registry();
    renderer_registry.remove(this);

    // finalized_.emit(*this);
}

void
BrowserRenderer::visit_renderer(BrowserRenderer & browser_renderer)
                                throw()
{
}

void
BrowserRenderer::visit_renderer(EnlargedRenderer & enlarged_renderer)
                                throw()
{
}

void
BrowserRenderer::visit_renderer(SlideshowRenderer & slideshow_renderer)
                                throw()
{
}

void
BrowserRenderer::render(const PhotoPtr & photo) throw()
{
}

void
BrowserRenderer::render(const PhotoList & photos) throw()
{
}

void
BrowserRenderer::clear_thumbnails() throw()
{
    Gtk::TreeModel::Children children
        = treeModelFilter_->get_model()->children();
    Gtk::TreeModel::iterator iter;

    for (iter = children.begin(); children.end() != iter; iter++)
    {
        BrowserModelColumnRecord model_column_record;
        Gtk::TreeModel::Row row = *iter;

        PhotoPtr photo = row[model_column_record.get_column_photo()];
        if( !photo->get_has_unsaved_data() )
        {
            photo->set_thumbnail_buffer( PixbufPtr(0) );
        }

        while (true == Gtk::Main::events_pending())
        {
            Gtk::Main::iteration();
        }
    }
}

PhotoList
BrowserRenderer::get_current_selection() throw()
{
    return thumbnailView_.get_selected_photos();
}

void
BrowserRenderer::present() throw()
{
    MainWindow & main_window = application_->get_main_window();
    main_window.present_dock_object(GDL_DOCK_OBJECT(dockItem_));
}

void
BrowserRenderer::receive_plugin(IPlugin & plugin) throw()
{
    plugin.visit_renderer(*this);
}

IRendererPtr
BrowserRenderer::receive_selector(IRendererSelector & selector,
                                  const IRendererPtr & renderer)
                                  throw()
{
    return selector.select(*this, renderer);
}

void
BrowserRenderer::on_action_add_to_export_queue() throw()
{
    ExportQueueInserter export_queue_inserter(*application_);
    export_queue_inserter();
}

void
BrowserRenderer::on_action_go_previous() throw()
{
    paginationBar_.go_previous();
}

void
BrowserRenderer::on_action_go_next() throw()
{
    paginationBar_.go_next();
}

void
BrowserRenderer::on_action_go_first() throw()
{
    paginationBar_.go_first();
}

void
BrowserRenderer::on_action_go_last() throw()
{
    paginationBar_.go_last();
}

void
BrowserRenderer::on_action_view_slideshow() throw()
{
    RendererRegistry & renderer_registry
        = application_->get_renderer_registry();
    const IRendererPtr slideshow_renderer
        = renderer_registry.select<SlideshowRenderer>();

    if (0 == slideshow_renderer)
    {
        return;
    }

    const std::vector<Gtk::TreeModel::Path> items
        = thumbnailView_.get_selected_items();

    const Gtk::TreeModel::iterator filter_iter
        = (true == items.empty())
          ? treeModelFilter_->children().begin()
          : treeModelFilter_->get_iter(items.front());

    if (false == filter_iter)
    {
        return;
    }

    const Gtk::TreeModel::iterator model_iter
        = treeModelFilter_->convert_iter_to_child_iter(filter_iter);

    application_->set_list_store_iter(model_iter);

    Gtk::TreeModel::Row row = *model_iter;
    BrowserModelColumnRecord model_column_record;
    const PhotoPtr photo = row[model_column_record.get_column_photo()];

    slideshow_renderer->present();
    slideshow_renderer->render(photo);
}

void
BrowserRenderer::on_selection_changed() throw()
{
    application_->get_engine().selection_changed().emit();
}

void
BrowserRenderer::on_dock_item_parent_changed(
                     Gtk::Widget * previous_parent) throw()
{
    signalSwitchPage_.disconnect();

    Gtk::Widget * const parent = Glib::wrap(gtk_widget_get_parent(
                                                dockItem_), false);
    Gtk::Notebook * const notebook = dynamic_cast<Gtk::Notebook *>(
                                         parent);

    if (0 == notebook)
    {
        pageNum_ = 0;
        return;
    }

    pageNum_ = notebook->page_num(*Glib::wrap(dockItem_, false));

    signalSwitchPage_
        = notebook->signal_switch_page().connect(
              sigc::mem_fun(*this,
                            &BrowserRenderer::on_switch_page));
}

void
BrowserRenderer::on_item_activated(const Gtk::TreeModel::Path & path)
                                   throw()
{
    RendererRegistry & renderer_registry
        = application_->get_renderer_registry();
    const IRendererPtr enlarged_renderer
        = renderer_registry.select<EnlargedRenderer>();

    if (0 == enlarged_renderer)
    {
        return;
    }

    const Gtk::TreeModel::iterator filter_iter
                                       = treeModelFilter_->get_iter(path);
    const Gtk::TreeModel::iterator model_iter
        = treeModelFilter_->convert_iter_to_child_iter(filter_iter);

    application_->set_list_store_iter(model_iter);

    Gtk::TreeModel::Row row = *model_iter;
    BrowserModelColumnRecord model_column_record;
    const PhotoPtr photo = row[model_column_record.get_column_photo()];

    enlarged_renderer->present();
    enlarged_renderer->render(photo);
}

void
BrowserRenderer::on_limits_changed() throw()
{
    const guint total = paginationBar_.get_total();
    const guint lower_limit = paginationBar_.get_lower_limit();
    const guint upper_limit = paginationBar_.get_upper_limit();

    {
        const ActionPtr action = actionGroup_->get_action(
                                     "ActionGoBrowserPreviousPage");
        action->set_sensitive((0 < lower_limit) ? true : false);
    }

    {
        const ActionPtr action = actionGroup_->get_action(
                                     "ActionGoBrowserFirstPage");
        action->set_sensitive((0 < lower_limit) ? true : false);
    }

    {
        const ActionPtr action = actionGroup_->get_action(
                                     "ActionGoBrowserNextPage");
        action->set_sensitive((total > upper_limit) ? true : false);
    }

    {
        const ActionPtr action = actionGroup_->get_action(
                                     "ActionGoBrowserLastPage");
        action->set_sensitive((total > upper_limit) ? true : false);
    }

    set_thumbnail_size();
    treeModelFilter_->refilter();

    status_pop();
    status_push();

    const Gtk::TreeModel::Children children
                                       = treeModelFilter_->children();
    if (true == children.empty())
    {
        return;
    }

    thumbnailView_.select_path(treeModelFilter_->get_path(
                                   children.begin()));
}

void
BrowserRenderer::on_list_store_change_begin(Application & application)
                                            throw()
{
    thumbnailView_.set_model(TreeModelPtr());
}

void
BrowserRenderer::on_list_store_change_end(Application & application)
                                          throw()
{
    const ListStorePtr & list_store = application.get_list_store();
    const Gtk::TreeModel::Children children = list_store->children();
    guint total;

    if (true == children.empty())
    {
        total = 0;
    }
    else
    {
        total = children.size();
    }

    thumbnailView_.set_model(treeModelFilter_);
    paginationBar_.set_total(total);
}

void
BrowserRenderer::on_action_view_zoom_changed(
                     const ScaleActionPtr & scale_action) throw()
{
    zoomValue_ = scale_action->get_value();

    static sigc::connection connection;

    connection.disconnect();
    connection
        = Glib::signal_timeout().connect_seconds(
              sigc::bind_return(
                  sigc::mem_fun(*this,
                                &BrowserRenderer::reload), false),
              1, Glib::PRIORITY_DEFAULT);
}

void
BrowserRenderer::on_action_view_zoom_in(
                     const ScaleActionPtr & scale_action) throw()
{
    scale_action->set_value(zoomValue_ + stepZoomValue);
}

void
BrowserRenderer::on_action_view_zoom_out(
                     const ScaleActionPtr & scale_action) throw()
{
    scale_action->set_value(zoomValue_ - stepZoomValue);
}

void
BrowserRenderer::on_switch_page(GtkNotebookPage * notebook_page,
                                guint page_num) throw()
{
    MainWindow & main_window = application_->get_main_window();
    const UIManagerPtr & ui_manager = main_window.get_ui_manager();

    // NB: Sometimes this gets invoked more than once consecutively
    //     -- no idea why (FIXME). Better safe than sorry.

    if (pageNum_ == static_cast<gint>(page_num))
    {
        RendererRegistry & renderer_registry
            = application_->get_renderer_registry();
        const IRendererPtr browser_renderer
            = renderer_registry.select<BrowserRenderer>();

        renderer_registry.set_current(browser_renderer);

        const Gtk::TreeModel::iterator & model_iter
            = application_->get_list_store_iter();

        if (true == model_iter)
        {
            const Gtk::TreeModel::Row row = *model_iter;
            BrowserModelColumnRecord model_column_record;
            const guint serial
                = row[model_column_record.get_column_serial()];

            paginationBar_.scroll_to_position(serial);

            const Gtk::TreeModel::iterator filter_iter
                = treeModelFilter_->convert_child_iter_to_iter(
                                        model_iter);
            const Gtk::TreeModel::Path path
                = treeModelFilter_->get_path(filter_iter);

            if (false == path.empty())
            {
                thumbnailView_.unselect_all();
                thumbnailView_.scroll_to_path(path, false, 0, 0);
                thumbnailView_.select_path(path);
            }
        }

        signalSelectionChanged_.unblock();
        status_push();

        if (0 == uiID_)
        {
            uiID_ = ui_manager->add_ui_from_file(uiFile);
            if (0 == uiID_)
            {
                // FIXME: error condition.
            }

            ui_manager->insert_action_group(actionGroup_);
        }
    }
    else
    {
        if (0 != uiID_)
        {
            ui_manager->remove_action_group(actionGroup_);
            ui_manager->remove_ui(uiID_);
            uiID_ = 0;
        }

        signalSelectionChanged_.block();
        status_pop();
    }
}

bool
BrowserRenderer::on_visible(
                     const Gtk::TreeModel::const_iterator & iter)
                     throw()
{
    const Gtk::TreeModel::Row row = *iter;
    BrowserModelColumnRecord model_column_record;
    const guint serial = row[model_column_record.get_column_serial()];

    return (serial >= paginationBar_.get_lower_limit()
            && serial < paginationBar_.get_upper_limit())
            ? true : false;
}

void
BrowserRenderer::reload() throw()
{
    set_thumbnail_size();
    clear_thumbnails();
    treeModelFilter_->refilter();
}

void
BrowserRenderer::set_thumbnail_size() throw()
{
    const guint thumbnail_renderer_width
                    = static_cast<guint>(
                          ratioWidth
                          * static_cast<double>(zoomValue_));
    const guint thumbnail_renderer_height
                    = static_cast<guint>(
                          ratioHeight
                          * static_cast<double>(zoomValue_));

    thumbnailView_.set_thumbnail_width(thumbnail_renderer_width);
    thumbnailView_.set_thumbnail_height(thumbnail_renderer_height);
}

void
BrowserRenderer::status_pop() throw()
{
    MainWindow & main_window = application_->get_main_window();
    Gtk::Statusbar & status_bar = main_window.get_status_bar();
    status_bar.pop(contextID_);
}

void
BrowserRenderer::status_push() throw()
{
    const guint total = paginationBar_.get_total();
    const guint lower_limit = paginationBar_.get_lower_limit();
    const guint upper_limit = paginationBar_.get_upper_limit();

    MainWindow & main_window = application_->get_main_window();
    Gtk::Statusbar & status_bar = main_window.get_status_bar();

    std::ostringstream sout;
    sout << lower_limit << " - " << upper_limit << " of " << total;
    status_bar.push(sout.str(), contextID_);
}

} // namespace Solang
