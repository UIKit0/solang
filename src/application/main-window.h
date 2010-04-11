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

#ifndef SOLANG_MAIN_WINDOW_H
#define SOLANG_MAIN_WINDOW_H

#include <string>
#include <vector>

#include <gdl/gdl.h>
#include <glibmm.h>
#include <gtkmm.h>

#include "non-copyable.h"
#include "spinner-tool-item.h"
#include "types.h"

namespace Solang
{

class Application;

class MainWindow :
    public Gtk::Window,
    private NonCopyable
{
    public:
        MainWindow() throw();

        virtual
        ~MainWindow() throw();

        void
        init(Application & application) throw();

        void
        final(Application & application) throw();

        void
        add_dock_object_left_top(DockObjectPtr dock_object) throw();

        void
        add_dock_object_left_bottom(DockObjectPtr dock_object) throw();

        void
        add_dock_object_center(DockObjectPtr dock_object) throw();

        void
        dock_object_center(DockObjectPtr dock_object) throw();

        void
        undock_object_center(DockObjectPtr dock_object) throw();

        void
        present_dock_object(DockObjectPtr dock_object) throw();

        Gtk::Notebook *
        get_notebook_center() throw();

        Gtk::Statusbar &
        get_status_bar() throw();

        const UIManagerPtr &
        get_ui_manager() throw();

        void
        set_busy(bool busy) throw();

    protected:
        std::string
        get_user_layout_file() throw();

        void
        load_layout() throw();

        void
        on_about_activate_link_url(Gtk::AboutDialog & about_dialog,
                                   const Glib::ustring & link)
                                   throw();

        void
        on_action_edit_add_to_export_queue() throw();

        void
        on_action_edit_clear_export_queue() throw();

        void
        on_action_help_about() throw();

        void
        on_action_help_contents() throw();

        void
        on_action_photo_quit() throw();

        void
        on_action_view_full_screen(
            const ConstToggleActionPtr & toggle_action) throw();

        void
        on_action_view_status_bar() throw();

        void
        on_action_view_tool_bar() throw();

        virtual bool
        on_delete_event(GdkEventAny * event);

        void
        save_layout() throw();

        ApplicationPtr application_;

        ActionGroupPtr actionGroup_;

        UIManagerPtr uiManager_;

        Gtk::UIManager::ui_merge_id uiID_;

        Gtk::VBox vBox_;

        SpinnerToolItem spinnerToolItem_;

        Gtk::HBox hBox_;

        Gtk::Statusbar statusBar_;

        GtkWidget * const dock_;

        GtkWidget * const dockBar_;

        GdlDockLayout * const layout_;

        bool showToolBar_;

        bool showStatusBar_;

        std::vector<DockObjectPtr> dockObjectsLeftTop_;

        std::vector<DockObjectPtr> dockObjectsLeftBottom_;

        std::vector<DockObjectPtr> dockObjectsCenter_;

    private:
        static const std::string artistsFile_;

        static const std::string authorsFile_;

        static const std::string copyingFile_;

        static const std::string layoutFile_;

        static const std::string uiFile_;

        static const Glib::ustring websiteURL_;
};

} // namespace Solang

#endif // SOLANG_MAIN_WINDOW_H
