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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <gdkmm.h>
#include <glibmm/i18n.h>
#include <gtkimageview/gtkimagescrollwin.h>
#include <gtkimageview/gtkimageview.h>

#include "application.h"
#include "browser-model-column-record.h"
#include "enlarged-renderer.h"
#include "engine.h"
#include "i-plugin.h"
#include "i-renderer-selector.h"
#include "main-window.h"
#include "photo.h"
#include "pixbuf-maker.h"
#include "tag-manager.h"
#include "tag-key-manager.h"
#include "types.h"

namespace Solang
{

static const std::string uiFile
    = PACKAGE_DATA_DIR G_DIR_SEPARATOR_S "ui" G_DIR_SEPARATOR_S
          PACKAGE_TARNAME"-enlarged-renderer.ui";

static void
image_view_on_scroll_event(GtkImageView * view,
                           GdkScrollDirection direction,
                           gpointer user_data) throw()
{
    if (0 == user_data)
    {
        g_warning("Not an instance of EnlargedRenderer");
        return;
    }

    EnlargedRenderer * enlarged_renderer
        = static_cast<EnlargedRenderer *>(user_data);

    enlarged_renderer->on_scroll_event(direction);
}

static void
image_view_on_key_press_event(GtkImageView * view,
                           GdkEventKey * event,
                           gpointer user_data) throw()
{
    if (0 == user_data)
    {
        g_warning("Not an instance of EnlargedRenderer");
        return;
    }

    EnlargedRenderer * enlarged_renderer
        = static_cast<EnlargedRenderer *>(user_data);

    enlarged_renderer->signal_key_press.emit(
        &(enlarged_renderer->get_current_selection()),
        Glib::ustring(event->string));
}

EnlargedRenderer::EnlargedRenderer() throw() :
    IRenderer(),
    Plugin(),
    application_(NULL),
    firstUse_(true),
    iconFactory_(Gtk::IconFactory::create()),
    actionGroup_(0),
    uiID_(0),
    dockItemName_("enlarged-dock-item"),
    dockItemTitle_(C_("Mode or view", "Enlarged")),
    dockItemBehaviour_(GDL_DOCK_ITEM_BEH_NO_GRIP),
    dockItem_(NULL),
    imageView_(0),
    imageScrollWin_(0),
    pageNum_(-1),
    pixbufMaker_(PixbufMaker::create(true)),
    signalMainWindowStateEvent_(),
    signalSwitchPage_()
{
    Gtk::IconSource icon_source;
    Gtk::IconSet icon_set_mode_image_edit;

    icon_source.set_filename(PACKAGE_DATA_DIR G_DIR_SEPARATOR_S
                             "pixmaps" G_DIR_SEPARATOR_S
                             "mode-image-edit-16.png");
    icon_source.set_size(Gtk::IconSize(16));
    icon_set_mode_image_edit.add_source(icon_source);

    icon_source.set_filename(PACKAGE_DATA_DIR G_DIR_SEPARATOR_S
                             "pixmaps" G_DIR_SEPARATOR_S
                             "mode-image-edit-22.png");
    icon_source.set_size(Gtk::IconSize(22));
    icon_set_mode_image_edit.add_source(icon_source);

    icon_source.set_filename(PACKAGE_DATA_DIR G_DIR_SEPARATOR_S
                             "pixmaps" G_DIR_SEPARATOR_S
                             "mode-image-edit-24.png");
    icon_source.set_size(Gtk::IconSize(24));
    icon_set_mode_image_edit.add_source(icon_source);

    iconFactory_->add(Gtk::StockID(PACKAGE_TARNAME"-mode-image-edit"),
                      icon_set_mode_image_edit);
    iconFactory_->add_default();
}

EnlargedRenderer::~EnlargedRenderer() throw()
{
    iconFactory_->remove_default();
}

void
EnlargedRenderer::init(Application & application) throw()
{
    application_ = &application;

    RendererRegistry & renderer_registry
        = application.get_renderer_registry();
    renderer_registry.add(this);

    // initialized_.emit(*this);
}

void
EnlargedRenderer::final(Application & application) throw()
{
    if (true == firstUse_)
    {
        return;
    }

    MainWindow & main_window = application.get_main_window();
    const UIManagerPtr & ui_manager = main_window.get_ui_manager();

    if (0 != uiID_)
    {
        ui_manager->remove_action_group(actionGroup_);
        ui_manager->remove_ui(uiID_);
        uiID_ = 0;
    }

    actionGroup_.reset();

    signalMainWindowStateEvent_.disconnect();
    signalListStoreChangeEnd_.disconnect();
    signalSwitchPage_.disconnect();

    main_window.undock_object_center(GDL_DOCK_OBJECT(dockItem_));

    pageNum_ = -1;
    firstUse_ = true;

    RendererRegistry & renderer_registry
        = application.get_renderer_registry();
    renderer_registry.remove(this);

    // finalized_.emit(*this);
}

void
EnlargedRenderer::visit_renderer(BrowserRenderer & browser_renderer)
                                 throw()
{
}

void
EnlargedRenderer::visit_renderer(EnlargedRenderer & enlarged_renderer)
                                 throw()
{
}

void
EnlargedRenderer::visit_renderer(SlideshowRenderer & slideshow_renderer)
                                 throw()
{
}

void
EnlargedRenderer::render(const PhotoPtr & photo) throw()
{
    pixbufMaker_->stop_async();

    const PixbufPtr & pixbuf = photo->get_buffer();
    if (0 != pixbuf)
    {
        on_pixbuf_maker_async_ready(pixbuf);
        return;
    }

    try
    {
        pixbufMaker_->make_async(
            sigc::mem_fun(
                *this,
                &EnlargedRenderer::on_pixbuf_maker_async_ready),
            photo);

        MainWindow & main_window = application_->get_main_window();
        main_window.set_busy(true);
    }
    catch (const Glib::ConvertError & e)
    {
        g_warning("%s", e.what().c_str());
    }
    catch (const Glib::FileError & e)
    {
        g_warning("%s", e.what().c_str());
    }
    catch (const Gdk::PixbufError & e)
    {
        g_warning("%s", e.what().c_str());
    }
}

void
EnlargedRenderer::on_pixbuf_maker_async_ready(
                      const PixbufPtr & pixbuf) throw()
{
    MainWindow & main_window = application_->get_main_window();
    main_window.set_busy(false);

    if (0 == imageView_)
    {
        imageView_ = gtk_image_view_new();
        if (0 == imageView_)
        {
            return;
        }

        int events = GDK_KEY_PRESS_MASK;
        gtk_widget_add_events(imageView_, static_cast<GdkEventMask>(events));

        gtk_image_view_set_show_frame(GTK_IMAGE_VIEW(imageView_),
                                      FALSE);

        GtkBindingSet * binding_set = gtk_binding_set_by_class(
            GTK_IMAGE_VIEW_GET_CLASS(imageView_));

        // Get rid of some of the default keybindings.

        gtk_binding_entry_remove(binding_set, GDK_KP_Add,
                                 static_cast<GdkModifierType>(0));
        gtk_binding_entry_remove(binding_set, GDK_equal,
                                 static_cast<GdkModifierType>(0));
        gtk_binding_entry_remove(binding_set, GDK_plus,
                                 static_cast<GdkModifierType>(0));
        gtk_binding_entry_remove(binding_set, GDK_KP_Subtract,
                                 static_cast<GdkModifierType>(0));
        gtk_binding_entry_remove(binding_set, GDK_minus,
                                 static_cast<GdkModifierType>(0));

        gtk_binding_entry_remove(binding_set, GDK_Right,
                                 static_cast<GdkModifierType>(0));
        gtk_binding_entry_remove(binding_set, GDK_Left,
                                 static_cast<GdkModifierType>(0));
        gtk_binding_entry_remove(binding_set, GDK_Down,
                                 static_cast<GdkModifierType>(0));
        gtk_binding_entry_remove(binding_set, GDK_Up,
                                 static_cast<GdkModifierType>(0));

        g_signal_connect(GTK_IMAGE_VIEW(imageView_),
                         "mouse-wheel-scroll",
                         G_CALLBACK(image_view_on_scroll_event),
                         this);

        g_signal_connect(GTK_IMAGE_VIEW(imageView_),
                         "key-press-event",
                         G_CALLBACK(image_view_on_key_press_event),
                         this);
    }

    if (0 == imageScrollWin_)
    {
        imageScrollWin_ = gtk_image_scroll_win_new(
                              GTK_IMAGE_VIEW(imageView_));
        if (0 == imageScrollWin_)
        {
            return;
        }

        gtk_widget_show_all(GTK_WIDGET(imageScrollWin_));

        if (0 == dockItem_)
        {
            return;
        }
        gtk_container_add(GTK_CONTAINER(dockItem_), imageScrollWin_);
    }

    gtk_image_view_set_pixbuf(GTK_IMAGE_VIEW(imageView_),
                              pixbuf->gobj(), TRUE);

    gtk_widget_grab_focus(imageView_);
}

void
EnlargedRenderer::render(const PhotoList & photos) throw()
{
}

void
EnlargedRenderer::create_action_group() throw()
{
    actionGroup_ = Gtk::ActionGroup::create(
                       Glib::ustring::compose("%1:%2",
                                              __FILE__,
                                              __LINE__));

    actionGroup_->add(
        Gtk::Action::create_with_icon_name(
            "ActionViewEnlargedSlideshow",
            "slideshow-play",
            _("_Slideshow"),
            _("Start a slideshow view of the photos")),
        Gtk::AccelKey("F5"),
        sigc::mem_fun(*this,
                      &EnlargedRenderer::on_action_view_slideshow));

    actionGroup_->add(
        Gtk::Action::create(
            "ActionEnlargedGoMenu", _("_Go")));

    {
        ActionPtr action = Gtk::Action::create(
            "ActionViewEnlargedZoomIn", Gtk::Stock::ZOOM_IN,
            _("_Zoom In"),
            _("Enlarge the photo"));

        action->property_short_label().set_value(C_("Zoom", "In"));
        action->property_is_important().set_value(true);
        actionGroup_->add(
            action, Gtk::AccelKey("<control>plus"),
            sigc::mem_fun(*this,
                          &EnlargedRenderer::on_action_view_zoom_in));
    }

    {
        ActionPtr action = Gtk::Action::create(
            "ActionViewEnlargedZoomOut", Gtk::Stock::ZOOM_OUT,
            _("Zoom _Out"),
            _("Shrink the photo"));

        action->property_short_label().set_value(C_("Zoom", "Out"));
        action->property_is_important().set_value(true);
        actionGroup_->add(
            action, Gtk::AccelKey("<control>minus"),
            sigc::mem_fun(*this,
                &EnlargedRenderer::on_action_view_zoom_out));
    }

    {
        ActionPtr action = Gtk::Action::create(
            "ActionViewEnlargedNormalSize", Gtk::Stock::ZOOM_100,
            _("_Normal Size"),
            _("Show the photo at its normal size"));

        action->property_short_label().set_value(
                                           C_("Zoom", "Normal"));
        action->property_is_important().set_value(true);
        actionGroup_->add(
            action, Gtk::AccelKey("<control>0"),
            sigc::mem_fun(*this,
                &EnlargedRenderer::on_action_view_normal_size));
    }

    {
        ActionPtr action = Gtk::Action::create(
            "ActionViewEnlargedBestFit", Gtk::Stock::ZOOM_FIT,
            _("Best _Fit"),
            _("Fit the photo to the window"));

        action->property_short_label().set_value(C_("Zoom", "Fit"));
        action->property_is_important().set_value(true);
        actionGroup_->add(
            action,
            sigc::mem_fun(*this,
                &EnlargedRenderer::on_action_view_best_fit));
    }

    {
        ActionPtr action = Gtk::Action::create(
            "ActionGoEnlargedPreviousPhoto", Gtk::Stock::GO_BACK,
            _("_Previous Photo"),
            _("Go to the previous photo in the collection"));

        action->property_short_label().set_value(
            C_("Navigation", "Previous"));
        action->property_is_important().set_value(true);
        actionGroup_->add(
            action, Gtk::AccelKey("<alt>Left"),
            sigc::mem_fun(*this,
                          &EnlargedRenderer::on_action_go_previous));
    }

    {
        ActionPtr action = Gtk::Action::create(
            "ActionGoEnlargedNextPhoto", Gtk::Stock::GO_FORWARD,
            _("_Next Photo"),
            _("Go to the next photo in the collection"));

        action->property_short_label().set_value(
            C_("Navigation", "Next"));
        action->property_is_important().set_value(true);
        actionGroup_->add(
            action, Gtk::AccelKey("<alt>Right"),
            sigc::mem_fun(*this,
                          &EnlargedRenderer::on_action_go_next));
    }

    actionGroup_->add(
        Gtk::Action::create(
            "ActionGoEnlargedFirstPhoto", Gtk::Stock::GOTO_FIRST,
            _("_First Photo"),
            _("Go to the first photo in the collection")),
        Gtk::AccelKey("<alt>Home"),
        sigc::mem_fun(*this, &EnlargedRenderer::on_action_go_first));

    actionGroup_->add(
        Gtk::Action::create(
            "ActionGoEnlargedLastPhoto", Gtk::Stock::GOTO_LAST,
            _("_Last Photo"),
            _("Go to the last photo in the collection")),
        Gtk::AccelKey("<alt>End"),
        sigc::mem_fun(*this, &EnlargedRenderer::on_action_go_last));

    actionGroup_->add(
        Gtk::Action::create(
            "AccelGoEnlargedPreviousPhoto", Gtk::StockID(),
            _("_Previous Photo"),
            _("Go to the previous photo in the collection")),
        Gtk::AccelKey("Left"),
        sigc::mem_fun(*this,
                      &EnlargedRenderer::on_action_go_previous));

    actionGroup_->add(
        Gtk::Action::create(
            "AccelGoEnlargedNextPhoto", Gtk::StockID(),
            _("_Next Photo"),
            _("Go to the next photo in the collection")),
        Gtk::AccelKey("Right"),
        sigc::mem_fun(*this, &EnlargedRenderer::on_action_go_next));

    actionGroup_->add(
        Gtk::Action::create(
            "AccelGoEnlargedFirstPhoto", Gtk::StockID(),
            _("_First Photo"),
            _("Go to the first photo in the collection")),
        Gtk::AccelKey("Home"),
        sigc::mem_fun(*this, &EnlargedRenderer::on_action_go_first));

    actionGroup_->add(
        Gtk::Action::create(
            "AccelGoEnlargedLastPhoto", Gtk::StockID(),
            _("_Last Photo"),
            _("Go to the last photo in the collection")),
        Gtk::AccelKey("End"),
        sigc::mem_fun(*this, &EnlargedRenderer::on_action_go_last));

    actionGroup_->add(
        Gtk::Action::create(
            "AccelViewEnlargedZoomIn", Gtk::StockID(),
            _("_Zoom In"),
            _("Enlarge the photo")),
        Gtk::AccelKey("plus"),
        sigc::mem_fun(*this,
                      &EnlargedRenderer::on_action_view_zoom_in));

    actionGroup_->add(
        Gtk::Action::create(
            "AccelViewEnlargedZoomIn1", Gtk::StockID(),
            _("_Zoom In"),
            _("Enlarge the photo")),
        Gtk::AccelKey("KP_Add"),
        sigc::mem_fun(*this,
                      &EnlargedRenderer::on_action_view_zoom_in));

    actionGroup_->add(
        Gtk::Action::create(
            "AccelViewEnlargedZoomIn2", Gtk::StockID(),
            _("_Zoom In"),
            _("Enlarge the photo")),
        Gtk::AccelKey("equal"),
        sigc::mem_fun(*this,
                      &EnlargedRenderer::on_action_view_zoom_in));

    actionGroup_->add(
        Gtk::Action::create(
            "AccelViewEnlargedZoomOut", Gtk::StockID(),
            _("Zoom _Out"),
            _("Shrink the photo")),
        Gtk::AccelKey("minus"),
        sigc::mem_fun(*this,
                      &EnlargedRenderer::on_action_view_zoom_out));

    actionGroup_->add(
        Gtk::Action::create(
            "AccelViewEnlargedZoomOut1", Gtk::StockID(),
            _("Zoom _Out"),
            _("Shrink the photo")),
        Gtk::AccelKey("KP_Subtract"),
        sigc::mem_fun(*this,
                      &EnlargedRenderer::on_action_view_zoom_out));

    actionGroup_->add(
        Gtk::Action::create(
            "AccelQuitEnlarged", Gtk::StockID()),
        Gtk::AccelKey("Escape"),
        sigc::mem_fun(*this,
                      &EnlargedRenderer::on_action_quit_enlarged));
}

PhotoList
EnlargedRenderer::get_current_selection() throw()
{
    const Gtk::TreeModel::iterator & iter
        = application_->get_list_store_iter();
    Gtk::TreeModel::Row row = *iter;

    BrowserModelColumnRecord model_column_record;
    const PhotoPtr photo = row[model_column_record.get_column_photo()];

    PhotoList photos;
    photos.push_back(photo);

    return photos;
}

TreePathList
EnlargedRenderer::get_selected_paths() const throw()
{
    const TreeModelPtr & tree_model = application_->get_list_store();
    const Gtk::TreeModel::iterator & iter
        = application_->get_list_store_iter();

    TreePathList paths;
    paths.push_back(tree_model->get_path(iter));

    return paths;
}

void
EnlargedRenderer::present() throw()
{
    prepare_for_first_use();

    MainWindow & main_window = application_->get_main_window();
    main_window.present_dock_object(GDL_DOCK_OBJECT(dockItem_));

    gtk_widget_grab_focus(imageView_);
}

void
EnlargedRenderer::receive_plugin(IPlugin & plugin) throw()
{
    plugin.visit_renderer(*this);
}

IRendererPtr
EnlargedRenderer::receive_selector(IRendererSelector & selector,
                                   const IRendererPtr & renderer)
                                   throw()
{
    return selector.select(*this, renderer);
}

void
EnlargedRenderer::on_action_go_previous() throw()
{
    const ListStorePtr & list_store = application_->get_list_store();
    Gtk::TreeModel::iterator & iter
        = application_->get_list_store_iter();

    if (false == iter)
    {
        if (0 != imageScrollWin_)
        {
            gtk_container_remove(GTK_CONTAINER(dockItem_),
                                 imageScrollWin_);
        }

        imageView_ = 0;
        imageScrollWin_ = 0;
        return;
    }

    if (true == iter.equal(list_store->children().begin()))
    {
        iter = list_store->children().end();
    }
    iter--;

    Gtk::TreeModel::Row row = *iter;
    BrowserModelColumnRecord model_column_record;
    const PhotoPtr photo = row[model_column_record.get_column_photo()];

    render(photo);

    Engine & engine = application_->get_engine();
    engine.selection_changed().emit();
}

void
EnlargedRenderer::on_action_go_next() throw()
{
    const ListStorePtr & list_store = application_->get_list_store();
    Gtk::TreeModel::iterator & iter
        = application_->get_list_store_iter();

    if (false == iter)
    {
        if (0 != imageScrollWin_)
        {
            gtk_container_remove(GTK_CONTAINER(dockItem_),
                                 imageScrollWin_);
        }

        imageView_ = 0;
        imageScrollWin_ = 0;
        return;
    }

    iter++;
    if (false == iter)
    {
        iter = list_store->children().begin();
    }

    Gtk::TreeModel::Row row = *iter;
    BrowserModelColumnRecord model_column_record;
    const PhotoPtr photo = row[model_column_record.get_column_photo()];

    render(photo);

    Engine & engine = application_->get_engine();
    engine.selection_changed().emit();
}

void
EnlargedRenderer::on_action_go_first() throw()
{
    const ListStorePtr & list_store = application_->get_list_store();
    Gtk::TreeModel::iterator & iter
        = application_->get_list_store_iter();

    if (false == iter)
    {
        if (0 != imageScrollWin_)
        {
            gtk_container_remove(GTK_CONTAINER(dockItem_),
                                 imageScrollWin_);
        }

        imageView_ = 0;
        imageScrollWin_ = 0;
        return;
    }

    iter = list_store->children().begin();

    Gtk::TreeModel::Row row = *iter;
    BrowserModelColumnRecord model_column_record;
    const PhotoPtr photo = row[model_column_record.get_column_photo()];

    render(photo);

    Engine & engine = application_->get_engine();
    engine.selection_changed().emit();
}

void
EnlargedRenderer::on_action_go_last() throw()
{
    const ListStorePtr & list_store = application_->get_list_store();
    Gtk::TreeModel::iterator & iter
        = application_->get_list_store_iter();

    if (false == iter)
    {
        if (0 != imageScrollWin_)
        {
            gtk_container_remove(GTK_CONTAINER(dockItem_),
                                 imageScrollWin_);
        }

        imageView_ = 0;
        imageScrollWin_ = 0;
        return;
    }

    iter = list_store->children().end();
    iter--;

    Gtk::TreeModel::Row row = *iter;
    BrowserModelColumnRecord model_column_record;
    const PhotoPtr photo = row[model_column_record.get_column_photo()];

    render(photo);

    Engine & engine = application_->get_engine();
    engine.selection_changed().emit();
}

void
EnlargedRenderer::on_action_quit_enlarged() throw()
{
    RendererRegistry & renderer_registry
        = application_->get_renderer_registry();
    const IRendererPtr browser_renderer
        = renderer_registry.select<BrowserRenderer>();

    if (0 == browser_renderer)
    {
        return;
    }

    browser_renderer->present();
    // NB: BrowserRenderer::render is a no-op.
}

void
EnlargedRenderer::on_action_view_slideshow() throw()
{
    RendererRegistry & renderer_registry
        = application_->get_renderer_registry();
    const IRendererPtr slideshow_renderer
        = renderer_registry.select<SlideshowRenderer>();

    if (0 == slideshow_renderer)
    {
        return;
    }

    const Gtk::TreeModel::iterator & iter
        = application_->get_list_store_iter();

    if (false == iter)
    {
        return;
    }

    Gtk::TreeModel::Row row = *iter;
    BrowserModelColumnRecord model_column_record;
    const PhotoPtr photo = row[model_column_record.get_column_photo()];

    slideshow_renderer->present();
    slideshow_renderer->render(photo);
}

void
EnlargedRenderer::on_action_view_best_fit() throw()
{
    if (0 == imageView_)
    {
        return;
    }

    if (false == GTK_IS_IMAGE_VIEW(imageView_))
    {
        g_warning("Not a GtkImageView");
        return;
    }

    gtk_image_view_set_fitting(GTK_IMAGE_VIEW(imageView_), TRUE);
}

void
EnlargedRenderer::on_action_view_normal_size() throw()
{
    if (0 == imageView_)
    {
        return;
    }

    if (false == GTK_IS_IMAGE_VIEW(imageView_))
    {
        g_warning("Not a GtkImageView");
        return;
    }

    gtk_image_view_set_zoom(GTK_IMAGE_VIEW(imageView_), 1.0);
}

void
EnlargedRenderer::on_action_view_zoom_in() throw()
{
    if (0 == imageView_)
    {
        return;
    }

    if (false == GTK_IS_IMAGE_VIEW(imageView_))
    {
        g_warning("Not a GtkImageView");
        return;
    }

    gtk_image_view_zoom_in(GTK_IMAGE_VIEW(imageView_));
}

void
EnlargedRenderer::on_action_view_zoom_out() throw()
{
    if (0 == imageView_)
    {
        return;
    }

    if (false == GTK_IS_IMAGE_VIEW(imageView_))
    {
        g_warning("Not a GtkImageView");
        return;
    }

    gtk_image_view_zoom_out(GTK_IMAGE_VIEW(imageView_));
}

void
EnlargedRenderer::on_dock_item_parent_changed(
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
                            &EnlargedRenderer::on_switch_page));
}

void
EnlargedRenderer::on_list_store_change_end(Application & application)
                                           throw()
{
    Gtk::TreeModel::iterator & iter
        = application_->get_list_store_iter();

    if (false == iter)
    {
        if (0 != imageScrollWin_)
        {
            gtk_container_remove(GTK_CONTAINER(dockItem_),
                                 imageScrollWin_);
        }

        imageView_ = 0;
        imageScrollWin_ = 0;
        return;
    }

    Gtk::TreeModel::Row row = *iter;
    BrowserModelColumnRecord model_column_record;
    const PhotoPtr photo = row[model_column_record.get_column_photo()];

    render(photo);

    Engine & engine = application_->get_engine();
    engine.selection_changed().emit();
}

bool
EnlargedRenderer::on_main_window_state_event(
                      GdkEventWindowState * event) throw()
{
    if (false == (event->changed_mask & Gdk::WINDOW_STATE_FULLSCREEN))
    {
        return true;
    }

    if (false == GTK_IS_IMAGE_VIEW(imageView_))
    {
        g_warning("Not a GtkImageView");
        return true;
    }

    gtk_image_view_set_black_bg(
        GTK_IMAGE_VIEW(imageView_),
        event->new_window_state & Gdk::WINDOW_STATE_FULLSCREEN);

    return true;
}

void
EnlargedRenderer::on_scroll_event(GdkScrollDirection direction) throw()
{
    switch (direction)
    {
        case GDK_SCROLL_UP:
            on_action_view_zoom_in();
            break;

        case GDK_SCROLL_DOWN:
            on_action_view_zoom_out();
            break;

        default:
            break;
    }
}

void
EnlargedRenderer::on_switch_page(GtkNotebookPage * notebook_page,
                                 guint page_num) throw()
{
    // NB: Sometimes this gets invoked more than once consecutively
    //     -- no idea why (FIXME). Better safe than sorry.

    if (pageNum_ == static_cast<gint>(page_num))
    {
        RendererRegistry & renderer_registry
            = application_->get_renderer_registry();
        const IRendererPtr enlarged_renderer
            = renderer_registry.select<EnlargedRenderer>();

        renderer_registry.set_current(enlarged_renderer);
        actionGroup_->set_visible(true);
    }
    else
    {
        actionGroup_->set_visible(false);
    }
}

void
EnlargedRenderer::prepare_for_first_use() throw()
{
    if (false == firstUse_)
    {
        return;
    }

    MainWindow & main_window = application_->get_main_window();
    const UIManagerPtr & ui_manager = main_window.get_ui_manager();

    uiID_ = ui_manager->add_ui_from_file(uiFile);
    if (0 == uiID_)
    {
        // FIXME: error condition.
    }

    create_action_group();
    if (true == actionGroup_)
    {
        ui_manager->insert_action_group(actionGroup_);
    }

    dockItem_ = gdl_dock_item_new_with_stock(
                    dockItemName_.c_str(),
                    dockItemTitle_.c_str(),
                    PACKAGE_TARNAME"-mode-image-edit",
                    dockItemBehaviour_);

    Gtk::Widget * const dock_item = Glib::wrap(dockItem_, false);
    dock_item->signal_parent_changed().connect(
        sigc::mem_fun(*this,
                      &EnlargedRenderer::on_dock_item_parent_changed));

    main_window.dock_object_center(GDL_DOCK_OBJECT(dockItem_));

    signalMainWindowStateEvent_
        = main_window.signal_window_state_event().connect(
              sigc::mem_fun(
                  *this,
                  &EnlargedRenderer::on_main_window_state_event));

    signalListStoreChangeEnd_
        = application_->list_store_change_end().connect(
              sigc::mem_fun(*this,
                            &EnlargedRenderer::on_list_store_change_end));

    firstUse_ = false;
}

} // namespace Solang
