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

#include <algorithm>
#include <functional>

#include <giomm.h>
#include <glibmm/i18n.h>
#include <sigc++/sigc++.h>

#include "application.h"
#include "export-queue-operations.h"
#include "main-window.h"

namespace Solang
{

class Docker :
    public std::unary_function<DockObjectPtr const &, void>
{
    public:
        Docker(DockObjectPtr const & reference,
               const GdlDockPlacement placement) throw();

        Docker(const Docker & source) throw();

        ~Docker() throw();

        Docker &
        operator=(const Docker & source) throw();

        void
        operator()(DockObjectPtr const & requester) throw();

    protected:
        DockObjectPtr reference_;

        GdlDockPlacement placement_;
};

Docker::Docker(DockObjectPtr const & reference,
               const GdlDockPlacement placement) throw() :
    std::unary_function<DockObjectPtr const &, void>(),
    reference_(reference),
    placement_(placement)
{
}

Docker::Docker(const Docker & source) throw() :
    std::unary_function<DockObjectPtr const &, void>(source),
    reference_(source.reference_),
    placement_(source.placement_)
{
}

Docker::~Docker() throw()
{
}

Docker &
Docker::operator=(const Docker & source) throw()
{
    if (this != &source)
    {
        std::unary_function<DockObjectPtr const &, void>
                ::operator=(source);
        reference_ = source.reference_;
        placement_ = source.placement_;
    }

    return *this;
}

void
Docker::operator()(DockObjectPtr const & requester) throw()
{
    gdl_dock_object_dock(reference_, requester, placement_, NULL);
    gtk_widget_show_all(GTK_WIDGET(requester));
}

class DockHider :
    public std::unary_function<DockObjectPtr const &, void>
{
    public:
        DockHider() throw();

        DockHider(ConstDockObjectPtr const & skip) throw();

        DockHider(const DockHider & source) throw();

        ~DockHider() throw();

        DockHider &
        operator=(const DockHider & source) throw();

        void
        operator()(DockObjectPtr const & dock_object) throw();

    protected:
        ConstDockObjectPtr skip_;

    private:
};

DockHider::DockHider() throw() :
    std::unary_function<DockObjectPtr const &, void>(),
    skip_(0)
{
}

DockHider::DockHider(ConstDockObjectPtr const & skip) throw() :
    std::unary_function<DockObjectPtr const &, void>(),
    skip_(skip)
{
}

DockHider::DockHider(const DockHider & source) throw() :
    std::unary_function<DockObjectPtr const &, void>(source),
    skip_(source.skip_)
{
}

DockHider::~DockHider() throw()
{
}

DockHider &
DockHider::operator=(const DockHider & source) throw()
{
    if (this != &source)
    {
        std::unary_function<DockObjectPtr const &, void>
                ::operator=(source);
        skip_ = source.skip_;
    }

    return *this;
}

void
DockHider::operator()(DockObjectPtr const & dock_object) throw()
{
    if (skip_ == dock_object)
    {
        return;
    }

    if (false == GDL_IS_DOCK_ITEM(dock_object))
    {
        g_warning("Not a GdlDockItem");
        return;
    }

    gtk_widget_hide(GTK_WIDGET(dock_object));
    gdl_dock_item_hide_item(GDL_DOCK_ITEM(dock_object));
}

const std::string MainWindow::artistsFile_
    = PACKAGE_DOC_DIR G_DIR_SEPARATOR_S "ARTISTS";

const std::string MainWindow::authorsFile_
    = PACKAGE_DOC_DIR G_DIR_SEPARATOR_S "AUTHORS";

const std::string MainWindow::copyingFile_
    = PACKAGE_DOC_DIR G_DIR_SEPARATOR_S "COPYING";

const std::string MainWindow::layoutFile_
                      = PACKAGE_DATA_DIR G_DIR_SEPARATOR_S
                        PACKAGE_TARNAME"-layout.xml";

const std::string MainWindow::uiFile_
                      = PACKAGE_DATA_DIR G_DIR_SEPARATOR_S
                        "ui" G_DIR_SEPARATOR_S
                        PACKAGE_TARNAME".ui";

const Glib::ustring MainWindow::websiteURL_
                        = "http://projects.gnome.org/solang";

MainWindow::MainWindow() throw() :
    Gtk::Window(Gtk::WINDOW_TOPLEVEL),
    application_(),
    actionGroup_(Gtk::ActionGroup::create(
                     Glib::ustring::compose("%1:%2",
                                            __FILE__,
                                            __LINE__))),
    uiManager_(Gtk::UIManager::create()),
    uiID_(uiManager_->add_ui_from_file(uiFile_)),
    vBox_(false, 0),
    spinnerToolItem_(),
    hBox_(false, 6),
    statusBar_(),
    dock_(gdl_dock_new()),
    dockBar_(gdl_dock_bar_new(GDL_DOCK(dock_))),
    layout_(gdl_dock_layout_new(GDL_DOCK(dock_))),
    showToolBar_(true),
    showStatusBar_(true),
    dockObjectsLeftTop_(),
    dockObjectsLeftBottom_(),
    dockObjectsCenter_()
{
    set_icon_name(PACKAGE_TARNAME);
    set_title("Solang");
    add(vBox_);

    actionGroup_->add(
        Gtk::Action::create(
            "ActionPhotoMenu", _("_Photo")));

    actionGroup_->add(
        Gtk::Action::create(
            "ActionPhotoQuit", Gtk::Stock::QUIT),
        sigc::mem_fun(*this, &MainWindow::on_action_photo_quit));

    actionGroup_->add(
        Gtk::Action::create(
            "ActionEditMenu", _("_Edit")));

    actionGroup_->add(
        Gtk::Action::create(
            "ActionEditAddToExportQueue", Gtk::Stock::ADD,
            _("_Add to Export Queue")),
        sigc::mem_fun(*this,
                      &MainWindow::on_action_edit_add_to_export_queue));

    actionGroup_->add(
        Gtk::Action::create(
            "ActionEditClearExportQueue", Gtk::Stock::CLEAR,
            _("_Clear Export Queue")),
        sigc::mem_fun(*this,
                      &MainWindow::on_action_edit_clear_export_queue));

    actionGroup_->add(Gtk::Action::create("ActionViewMenu",
                                          _("_View")));

    actionGroup_->add(
        Gtk::ToggleAction::create(
            "ActionViewToolBar", _("_Toolbar"),
            _("Show or hide the toolbar in the current window"),
            showToolBar_),
        sigc::mem_fun(*this, &MainWindow::on_action_view_tool_bar));

    actionGroup_->add(
        Gtk::ToggleAction::create(
            "ActionViewStatusBar", _("_Statusbar"),
            _("Show or hide the statusbar in the current window"),
            showStatusBar_),
        sigc::mem_fun(*this, &MainWindow::on_action_view_status_bar));

    {
        ToggleActionPtr toggle_action = Gtk::ToggleAction::create(
            "ActionViewFullScreen", Gtk::Stock::FULLSCREEN,
            _("_Full Screen"),
            _("Show the current photo in full screen mode"),
            false);

        // FIXME: some problem with the reference counting.
        toggle_action->reference();

        actionGroup_->add(
            toggle_action, Gtk::AccelKey("F11"),
            sigc::bind(
                sigc::mem_fun(
                    *this,
                    &MainWindow::on_action_view_full_screen),
                toggle_action));
    }

    actionGroup_->add(
        Gtk::Action::create(
            "ActionHelpMenu", _("_Help")));

    actionGroup_->add(
        Gtk::Action::create(
            "ActionHelpContents", Gtk::Stock::HELP,
            _("_Contents")),
        Gtk::AccelKey("F1"),
        sigc::mem_fun(*this, &MainWindow::on_action_help_contents));

    actionGroup_->add(
        Gtk::Action::create(
            "ActionHelpAbout", Gtk::Stock::ABOUT),
        sigc::mem_fun(*this, &MainWindow::on_action_help_about));

    uiManager_->insert_action_group(actionGroup_);
    add_accel_group(uiManager_->get_accel_group());

    Gtk::Widget * const menu_bar = uiManager_->get_widget("/MenuBar");
    if (NULL != menu_bar)
    {
        vBox_.pack_start(*menu_bar, Gtk::PACK_SHRINK, 0);
    }

    Gtk::Toolbar * const tool_bar = dynamic_cast<Gtk::Toolbar *>(
        uiManager_->get_widget("/ToolBar"));

    if (NULL != tool_bar)
    {
        tool_bar->set_toolbar_style( Gtk::TOOLBAR_ICONS );
        //We don't need no stinkin' text :-P
        tool_bar->append(spinnerToolItem_);

        vBox_.pack_start(*tool_bar, Gtk::PACK_SHRINK, 0);
    }

    GtkBox * const hbox = GTK_BOX(hBox_.gobj());
    gtk_box_pack_start(hbox, dockBar_, FALSE, FALSE, 0);
    gtk_box_pack_end(hbox, dock_, TRUE, TRUE, 0);
    vBox_.pack_start(hBox_, Gtk::PACK_EXPAND_WIDGET, 0);

    statusBar_.set_has_resize_grip(true);
    vBox_.pack_start(statusBar_, Gtk::PACK_SHRINK, 0);

    show_all_children();
}

MainWindow::~MainWindow() throw()
{
    uiManager_->remove_action_group(actionGroup_);
    uiManager_->remove_ui(uiID_);
    g_object_unref(layout_);
}

void
MainWindow::init(Application & application) throw()
{
    application_ = &application;
    
    GSettings * settings = application.get_settings();
    if (g_settings_get_boolean(settings, "window-maximized"))
    {
        maximize();
    }
    else
    {
        int width = g_settings_get_int(settings, "window-width");
        int height = g_settings_get_int(settings, "window-height");
        set_default_size(width, height);
    }

    if (false == dockObjectsCenter_.empty())
    {
        gdl_dock_add_item(GDL_DOCK(dock_),
                          GDL_DOCK_ITEM(dockObjectsCenter_.front()),
                          GDL_DOCK_RIGHT);
        gtk_widget_show_all(GTK_WIDGET(dockObjectsCenter_.front()));

        if (1 < dockObjectsCenter_.size())
        {
            std::for_each(++dockObjectsCenter_.begin(),
                          dockObjectsCenter_.end(),
                          Docker(dockObjectsCenter_.front(),
                                 GDL_DOCK_CENTER));
            present_dock_object(dockObjectsCenter_.front());
        }
    }

    if (false == dockObjectsLeftTop_.empty())
    {
        gdl_dock_add_item(GDL_DOCK(dock_),
                          GDL_DOCK_ITEM(dockObjectsLeftTop_.front()),
                          GDL_DOCK_LEFT);
        gtk_widget_show_all(GTK_WIDGET(dockObjectsLeftTop_.front()));

        if (1 < dockObjectsLeftTop_.size())
        {
            std::for_each(++dockObjectsLeftTop_.begin(),
                          dockObjectsLeftTop_.end(),
                          Docker(dockObjectsLeftTop_.front(),
                                 GDL_DOCK_CENTER));
        }
    }

    if (false == dockObjectsLeftBottom_.empty())
    {
        gdl_dock_add_item(GDL_DOCK(dock_),
            GDL_DOCK_ITEM(dockObjectsLeftBottom_.front()),
            GDL_DOCK_BOTTOM);
        gtk_widget_show_all(GTK_WIDGET(dockObjectsLeftBottom_.front()));

        if (1 < dockObjectsLeftBottom_.size())
        {
            std::for_each(++dockObjectsLeftBottom_.begin(),
                          dockObjectsLeftBottom_.end(),
                          Docker(dockObjectsLeftBottom_.front(),
                                 GDL_DOCK_CENTER));
        }
    }

    load_layout();

    present_dock_object(dockObjectsCenter_.front());
}

void
MainWindow::final(Application & application) throw()
{
    // NB: + Hide all renderers except the browser before saving the
    //       layout. Otherwise, all hell breaks loose when loading
    //       the layout because GDL tries to place the renderers in a
    //       Gtk::Notebook, which leads to spurious switch-page
    //       signals being emitted.
    //     + Do not save the layout if something other than a renderer
    //       has been docked in the centre. In other words, when all
    //       the other renderers have been hidden, the browser should
    //       not be placed inside a Gtk::Notebook. The reason for this
    //       is the same as above.

    if (1 < dockObjectsCenter_.size())
    {
        const std::vector<DockObjectPtr>::iterator second
            = dockObjectsCenter_.begin() + 1;

        std::for_each(second, dockObjectsCenter_.end(), DockHider());
        dockObjectsCenter_.erase(second, dockObjectsCenter_.end());

        Gtk::Container * const dock_object
            = Glib::wrap(GTK_CONTAINER(dockObjectsCenter_.front()),
                         false);

        if (0 != dynamic_cast<Gtk::Notebook *>(
                     dock_object->get_parent()))
        {
            return;
        }
    }

    save_layout();
    
    //I don't know if it's the right place to save the window size
    GSettings * settings = application_->get_settings();
    int width, height;
    get_size(height, width);
    g_settings_set_int(settings, "window-width", width);
    g_settings_set_int(settings, "window-height", height);
    //we get the state of the underlying Gdk::Window
    WindowPtr window = get_window();
    Gdk::WindowState state = window->get_state();
    gboolean maximized = state | Gdk::WINDOW_STATE_MAXIMIZED;
    g_settings_set_boolean(settings, "window-maximized", maximized);
}

void
MainWindow::add_dock_object_left_top(DockObjectPtr dock_object)
                                     throw()
{
    dockObjectsLeftTop_.push_back(dock_object);
}

void
MainWindow::add_dock_object_left_bottom(DockObjectPtr dock_object)
                                        throw()
{
    dockObjectsLeftBottom_.push_back(dock_object);
}

void
MainWindow::add_dock_object_center(DockObjectPtr dock_object) throw()
{
    dockObjectsCenter_.push_back(dock_object);
}

void
MainWindow::dock_object_center(DockObjectPtr dock_object) throw()
{
    gdl_dock_object_dock(dockObjectsCenter_.front(),
                         dock_object, GDL_DOCK_CENTER, NULL);
    dockObjectsCenter_.push_back(dock_object);
}

void
MainWindow::undock_object_center(DockObjectPtr dock_object) throw()
{
    if (false == GDL_IS_DOCK_ITEM(dock_object))
    {
        g_warning("Not a GdlDockItem");
        return;
    }

    const std::vector<DockObjectPtr>::iterator iter
        = std::find(dockObjectsCenter_.begin(),
                    dockObjectsCenter_.end(),
                    dock_object);

    if (dockObjectsCenter_.end() == iter)
    {
        return;
    }

    gdl_dock_item_hide_item(GDL_DOCK_ITEM(dock_object));
    dockObjectsCenter_.erase(iter);
}

void
MainWindow::present_dock_object(DockObjectPtr dock_object) throw()
{
    if (0 == dock_object)
    {
        g_warning("0 == dock_object");
        return;
    }

    Gtk::Container * dock_container
        = Glib::wrap(GTK_CONTAINER(dock_object), false);
    Gtk::Notebook * parent
        = dynamic_cast<Gtk::Notebook *>(dock_container->get_parent());

    if (0 != parent)
    {
        const gint page_num = parent->page_num(*dock_container);
        parent->set_current_page(page_num);
    }
}

Gtk::Notebook *
MainWindow::get_notebook_center() throw()
{
    Gtk::Container * dock_container
        = Glib::wrap(GTK_CONTAINER(dockObjectsCenter_.front()), false);
    Gtk::Notebook * parent
        = dynamic_cast<Gtk::Notebook *>(dock_container->get_parent());

    return parent;
}

Gtk::Statusbar &
MainWindow::get_status_bar() throw()
{
    return statusBar_;
}

const UIManagerPtr &
MainWindow::get_ui_manager() throw()
{
    return uiManager_;
}

std::string
MainWindow::get_user_layout_file() throw()
{
    return Glib::get_user_data_dir() + "/"
               + Glib::get_prgname() + "/"
               + Glib::get_prgname() + "-layout.xml";
}

void
MainWindow::load_layout() throw()
{
    if (false == gdl_dock_layout_load_from_file(
                    GDL_DOCK_LAYOUT(layout_),
                    get_user_layout_file().c_str()))
    {
        if (false == gdl_dock_layout_load_from_file(
                         GDL_DOCK_LAYOUT(layout_),
                         layoutFile_.c_str()))
        {
            g_warning("gdl_dock_layout_load_from_file: Failed");
            return;
        }
    }

    if (false == gdl_dock_layout_load_layout(GDL_DOCK_LAYOUT(layout_),
                                             0))
    {
        g_warning("gdl_dock_layout_load_layout: Failed");
    }
}

void
MainWindow::on_about_activate_link_url(
                Gtk::AboutDialog & about_dialog,
                const Glib::ustring & link) throw()
{
    const ScreenPtr screen = about_dialog.get_screen();
    gtk_show_uri(screen->gobj(), link.c_str(), GDK_CURRENT_TIME,
                 NULL);
}

void
MainWindow::on_action_edit_add_to_export_queue() throw()
{
    ExportQueueInserter export_queue_inserter(*application_);
    export_queue_inserter();
}

void
MainWindow::on_action_edit_clear_export_queue() throw()
{
    ExportQueueCleaner export_queue_cleaner(*application_);
    export_queue_cleaner();
}

void
MainWindow::on_action_help_about() throw()
{
    Gtk::AboutDialog about_dialog;

    about_dialog.set_transient_for(*this);

    DataInputStreamPtr fin;
    FilePtr file = Gio::File::create_for_path(artistsFile_);

    try
    {
        fin = Gio::DataInputStream::create(file->read());
    }
    catch(const Gio::Error & e)
    {
        g_warning("%s", e.what().c_str());
    }

    std::string line;
    std::vector<Glib::ustring> artists;

    while (0 != fin && true == fin->read_line(line))
    {
        if (true == line.empty())
        {
            continue;
        }

        if ('#' == line[0])
        {
            continue;
        }

        artists.push_back(Glib::ustring(line));
    }

    if (false == artists.empty())
    {
        about_dialog.set_artists(artists);
    }

    file = Gio::File::create_for_path(authorsFile_);

    try
    {
        fin = Gio::DataInputStream::create(file->read());
    }
    catch(const Gio::Error & e)
    {
        g_warning("%s", e.what().c_str());
    }

    std::vector<Glib::ustring> authors;

    while (0 != fin && true == fin->read_line(line))
    {
        if (true == line.empty())
        {
            continue;
        }

        if ('#' == line[0])
        {
            continue;
        }

        authors.push_back(Glib::ustring(line));
    }

    if (false == authors.empty())
    {
        about_dialog.set_authors(authors);
    }

    about_dialog.set_comments(_("A photo manager for GNOME"));
    about_dialog.set_copyright(
        "Copyright \xc2\xa9 2009 - 2010 The " PACKAGE_NAME " authors");

    file = Gio::File::create_for_path(copyingFile_);

    try
    {
        fin = Gio::DataInputStream::create(file->read());
    }
    catch(const Gio::Error & e)
    {
        g_warning("%s", e.what().c_str());
    }

    Glib::ustring license;

    while (0 != fin && true == fin->read_line(line))
    {
        license += Glib::ustring(line);
        license += "\n";
    }

    if (false == license.empty())
    {
        about_dialog.set_license(license);
    }

    about_dialog.set_logo_icon_name(PACKAGE_TARNAME);

    // Translators: Put your names here.
    about_dialog.set_translator_credits(_("translator-credits"));

    about_dialog.set_version(_(PACKAGE_VERSION));
    about_dialog.set_website(websiteURL_);
    about_dialog.set_website_label(
        Glib::ustring::compose(C_("Project website", "%1 Website"),
                               PACKAGE_NAME));
    about_dialog.set_wrap_license(false);
    about_dialog.set_url_hook(
        sigc::mem_fun(*this,
                      &MainWindow::on_about_activate_link_url));

    about_dialog.run();
}

void
MainWindow::on_action_help_contents() throw()
{
    const ScreenPtr screen = get_screen();
    gtk_show_uri(screen->gobj(), "ghelp:" PACKAGE_TARNAME,
                 GDK_CURRENT_TIME, 0);
}

void
MainWindow::on_action_photo_quit() throw()
{
    hide();
}

void
MainWindow::on_action_view_full_screen(
                const ConstToggleActionPtr & toggle_action) throw()
{
    const bool fullscreen_active = toggle_action->get_active();
    static gint page_num = -1;

    Gtk::Widget * const menu_bar = uiManager_->get_widget("/MenuBar");
    Gtk::Widget * const tool_bar = uiManager_->get_widget("/ToolBar");

    std::string layout_file = get_user_layout_file();

    if (true == fullscreen_active)
    {
        fullscreen();

        if (0 != menu_bar)
        {
            menu_bar->hide();
        }

        if (0 != tool_bar && true == showToolBar_)
        {
            tool_bar->hide();
        }

        if (true == showStatusBar_)
        {
            statusBar_.hide();
        }

        gdl_dock_layout_save_layout(GDL_DOCK_LAYOUT(layout_), 0);

        const bool result = gdl_dock_layout_save_to_file(
                                GDL_DOCK_LAYOUT(layout_),
                                layout_file.c_str());

        if (false == result)
        {
            g_warning("%s: Failed to save the layout",
                      layout_file.c_str());
            return;
        }

        std::for_each(dockObjectsLeftTop_.begin(),
                      dockObjectsLeftTop_.end(),
                      DockHider());

        std::for_each(dockObjectsLeftBottom_.begin(),
                      dockObjectsLeftBottom_.end(),
                      DockHider());

        const Gtk::Notebook * const notebook = get_notebook_center();

        if (0 == notebook)
        {
            g_warning("No notebook found");
            page_num = -1;
        }
        else
        {
            page_num = notebook->get_current_page();

            const Gtk::Widget * const current_page
                = notebook->get_nth_page(page_num);

            std::for_each(dockObjectsCenter_.begin(),
                          dockObjectsCenter_.end(),
                          DockHider(GDL_DOCK_OBJECT(
                                        current_page->gobj())));
        }
    }
    else
    {
        if (0 != menu_bar)
        {
            menu_bar->show_all();
        }

        if (0 != tool_bar && true == showToolBar_)
        {
            tool_bar->show_all();
        }

        if (true == showStatusBar_)
        {
            statusBar_.show_all();
        }

        unfullscreen();

        if (false == Glib::file_test(layout_file,
                                     Glib::FILE_TEST_EXISTS))
        {
            g_warning("%s: File not found", layout_file.c_str());

            if (false == Glib::file_test(layoutFile_,
                                         Glib::FILE_TEST_EXISTS))
            {
                g_warning("%s: File not found", layoutFile_.c_str());
                return;
            }

            layout_file = layoutFile_;
        }

        const bool result = gdl_dock_layout_load_from_file(
                                GDL_DOCK_LAYOUT(layout_),
                                layout_file.c_str());

        if (true == result)
        {
            gdl_dock_layout_load_layout(GDL_DOCK_LAYOUT(layout_), 0);
        }
        else
        {
            g_warning("%s: Failed to load the layout",
                      layout_file.c_str());
            return;
        }

        Gtk::Notebook * const notebook = get_notebook_center();

        if (0 == notebook)
        {
            g_warning("No notebook found");
        }
        else if (-1 == page_num)
        {
            g_warning("No page number set");
        }
        else
        {
            notebook->set_current_page(page_num);
        }
    }
}

void
MainWindow::on_action_view_status_bar() throw()
{
    showStatusBar_ = (true == showStatusBar_) ? false : true;
    (true == showStatusBar_) ? statusBar_.show_all()
                             : statusBar_.hide();
}

void
MainWindow::on_action_view_tool_bar() throw()
{
    showToolBar_ = (true == showToolBar_) ? false : true;

    Gtk::Widget * const tool_bar = uiManager_->get_widget("/ToolBar");
    if (NULL != tool_bar)
    {
        (true == showToolBar_) ? tool_bar->show_all()
                               : tool_bar->hide();
    }
}

bool
MainWindow::on_delete_event(GdkEventAny * event)
{
    bool return_value = Gtk::Window::on_delete_event(event);

    hide();
    return return_value;
}

void
MainWindow::save_layout() throw()
{
    gdl_dock_layout_save_layout(GDL_DOCK_LAYOUT(layout_), 0);

    if (false == gdl_dock_layout_save_to_file(
                     GDL_DOCK_LAYOUT(layout_),
                     get_user_layout_file().c_str()))
    {
        g_warning("gdl_dock_layout_save_to_file: Failed");
    }
}

void
MainWindow::set_busy(bool busy) throw()
{
    WindowPtr window = get_window();

    switch (busy)
    {
        case true:
        {
            Gdk::Cursor cursor(Gdk::WATCH);
            window->set_cursor(cursor);
            break;
        }

        case false:
            window->set_cursor();
            break;
    }

    spinnerToolItem_.set_spinning(busy);
}

} // namespace Solang
