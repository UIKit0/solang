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
#include <gtkimageview/gtkimagescrollwin.h>
#include <gtkimageview/gtkimageview.h>

#include "slideshow-window.h"

namespace Solang
{

static const guint toolBarInterval = 2; // s

static const std::string uiFile
    = PACKAGE_DATA_DIR G_DIR_SEPARATOR_S "ui" G_DIR_SEPARATOR_S
           PACKAGE_TARNAME"-slideshow.ui";

static void
image_view_on_scroll_event(GtkImageView * view,
                           GdkScrollDirection direction,
                           gpointer user_data) throw()
{
    if (0 == user_data)
    {
        g_warning("Not an instance of SlideshowWindow");
        return;
    }

    SlideshowWindow * const slideshow_window
        = static_cast<SlideshowWindow *>(user_data);

    slideshow_window->on_image_view_scroll_event(direction);
}

SlideshowWindow::SlideshowWindow() throw() :
    Gtk::Window(Gtk::WINDOW_TOPLEVEL),
    actionGroup_(Gtk::ActionGroup::create(
                     Glib::ustring::compose("%1:%2",
                                            __FILE__,
                                            __LINE__))),
    uiManager_(Gtk::UIManager::create()),
    uiID_(uiManager_->add_ui_from_file(uiFile)),
    vBox_(false, 0),
    imageScrollWin_(0),
    imageView_(0)
{
    set_decorated(false);
    set_default_size(800, 600);
    add(vBox_);

    actionGroup_->add(
        Gtk::Action::create(
            "ActionViewSlideshowZoomIn", Gtk::Stock::ZOOM_IN,
            C_("Zoom", "In"),
            _("Enlarge the photo")),
        Gtk::AccelKey("<control>plus"),
        sigc::mem_fun(*this,
                      &SlideshowWindow::on_action_view_zoom_in));

    actionGroup_->add(
        Gtk::Action::create(
            "ActionViewSlideshowZoomOut", Gtk::Stock::ZOOM_OUT,
            C_("Zoom", "Out"),
            _("Shrink the photo")),
        Gtk::AccelKey("<control>minus"),
        sigc::mem_fun(*this,
                      &SlideshowWindow::on_action_view_zoom_out));

    actionGroup_->add(
        Gtk::Action::create(
            "ActionViewSlideshowNormalSize", Gtk::Stock::ZOOM_100,
            C_("Zoom", "Normal"),
            _("Show the photo at its normal size")),
        Gtk::AccelKey("<control>0"),
        sigc::mem_fun(*this,
                      &SlideshowWindow::on_action_view_normal_size));

    actionGroup_->add(
        Gtk::Action::create(
            "ActionViewSlideshowBestFit", Gtk::Stock::ZOOM_FIT,
            C_("Zoom", "Fit"),
            _("Fit the photo to the window")),
        sigc::mem_fun(*this,
                      &SlideshowWindow::on_action_view_best_fit));

    actionGroup_->add(
        Gtk::Action::create(
            "AccelViewSlideshowZoomIn", Gtk::StockID(),
            C_("Zoom", "In"),
            _("Enlarge the photo")),
        Gtk::AccelKey("plus"),
        sigc::mem_fun(*this,
                      &SlideshowWindow::on_action_view_zoom_in));

    actionGroup_->add(
        Gtk::Action::create(
            "AccelViewSlideshowZoomIn1", Gtk::StockID(),
            C_("Zoom", "In"),
            _("Enlarge the photo")),
        Gtk::AccelKey("KP_Add"),
        sigc::mem_fun(*this,
                      &SlideshowWindow::on_action_view_zoom_in));

    actionGroup_->add(
        Gtk::Action::create(
            "AccelViewSlideshowZoomIn2", Gtk::StockID(),
            C_("Zoom", "In"),
            _("Enlarge the photo")),
        Gtk::AccelKey("equal"),
        sigc::mem_fun(*this,
                      &SlideshowWindow::on_action_view_zoom_in));

    actionGroup_->add(
        Gtk::Action::create(
            "AccelViewSlideshowZoomOut", Gtk::StockID(),
            C_("Zoom", "Out"),
            _("Shrink the photo")),
        Gtk::AccelKey("minus"),
        sigc::mem_fun(*this,
                      &SlideshowWindow::on_action_view_zoom_out));

    actionGroup_->add(
        Gtk::Action::create(
            "AccelViewSlideshowZoomOut1", Gtk::StockID(),
            C_("Zoom", "Out"),
            _("Shrink the photo")),
        Gtk::AccelKey("KP_Subtract"),
        sigc::mem_fun(*this,
                      &SlideshowWindow::on_action_view_zoom_out));

    uiManager_->insert_action_group(actionGroup_);
    add_accel_group(uiManager_->get_accel_group());

    Gtk::Toolbar * const tool_bar = dynamic_cast<Gtk::Toolbar *>(
        uiManager_->get_widget("/ToolBar"));

    if (0 != tool_bar)
    {
        vBox_.pack_start(*tool_bar, Gtk::PACK_SHRINK, 0);
    }

    show_all_children();
}

SlideshowWindow::~SlideshowWindow() throw()
{
}

const UIManagerPtr &
SlideshowWindow::get_ui_manager() throw()
{
    return uiManager_;
}

void
SlideshowWindow::on_action_view_best_fit() throw()
{
    if (0 == imageView_ || false == GTK_IS_IMAGE_VIEW(imageView_))
    {
        g_warning("Not a GtkImageView");
        return;
    }

    gtk_image_view_set_fitting(GTK_IMAGE_VIEW(imageView_), TRUE);
}

void
SlideshowWindow::on_action_view_normal_size() throw()
{
    if (0 == imageView_ || false == GTK_IS_IMAGE_VIEW(imageView_))
    {
        g_warning("Not a GtkImageView");
        return;
    }

    gtk_image_view_set_zoom(GTK_IMAGE_VIEW(imageView_), 1.0);
}

void
SlideshowWindow::on_action_view_zoom_in() throw()
{
    if (0 == imageView_ || false == GTK_IS_IMAGE_VIEW(imageView_))
    {
        g_warning("Not a GtkImageView");
        return;
    }

    gtk_image_view_zoom_in(GTK_IMAGE_VIEW(imageView_));
}

void
SlideshowWindow::on_action_view_zoom_out() throw()
{
    if (0 == imageView_ || false == GTK_IS_IMAGE_VIEW(imageView_))
    {
        g_warning("Not a GtkImageView");
        return;
    }

    gtk_image_view_zoom_out(GTK_IMAGE_VIEW(imageView_));
}

void
SlideshowWindow::on_image_view_scroll_event(
                     GdkScrollDirection direction) throw()
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

bool
SlideshowWindow::on_motion_notify_event(GdkEventMotion * event)
                                        throw()
{
    bool return_value = Gtk::Window::on_motion_notify_event(event);

    signalTimeout_.disconnect();

    Gtk::Toolbar * const tool_bar = dynamic_cast<Gtk::Toolbar *>(
        uiManager_->get_widget("/ToolBar"));

    if (0 != tool_bar)
    {
        tool_bar->show_all();

        signalTimeout_
            = Glib::signal_timeout().connect_seconds(
                  sigc::mem_fun(*this,
                                &SlideshowWindow::on_tool_bar_timeout),
                  toolBarInterval, Glib::PRIORITY_DEFAULT);
    }

    return return_value;
}

void
SlideshowWindow::on_show() throw()
{
    Gtk::Window::on_show();

    Gtk::Toolbar * const tool_bar = dynamic_cast<Gtk::Toolbar *>(
        uiManager_->get_widget("/ToolBar"));

    if (0 != tool_bar)
    {
        tool_bar->hide();
    }
}

bool
SlideshowWindow::on_tool_bar_timeout() throw()
{
    Gtk::Toolbar * const tool_bar = dynamic_cast<Gtk::Toolbar *>(
        uiManager_->get_widget("/ToolBar"));

    if (0 != tool_bar)
    {
        tool_bar->hide();
    }

    return false;
}

void
SlideshowWindow::render(const PixbufPtr & pixbuf) throw()
{
    if (0 == imageView_)
    {
        imageView_ = gtk_image_view_new();
        if (0 == imageView_)
        {
            return;
        }

        gtk_image_view_set_black_bg(GTK_IMAGE_VIEW(imageView_),
                                    TRUE);
        gtk_image_view_set_show_frame(GTK_IMAGE_VIEW(imageView_),
                                      FALSE);

        GtkBindingSet * const binding_set = gtk_binding_set_by_class(
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
    }

    if (0 == imageScrollWin_)
    {
        imageScrollWin_ = gtk_image_scroll_win_new(
                              GTK_IMAGE_VIEW(imageView_));
        if (0 == imageScrollWin_)
        {
            return;
        }

        gtk_widget_show_all(imageScrollWin_);

        Gtk::Widget * const image_scroll_win
                                = Glib::wrap(imageScrollWin_, false);
        vBox_.pack_start(*image_scroll_win,
                         Gtk::PACK_EXPAND_WIDGET, 0);
    }

    gtk_image_view_set_pixbuf(GTK_IMAGE_VIEW(imageView_),
                              pixbuf->gobj(), TRUE);
}

} // namespace Solang
