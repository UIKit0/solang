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

#ifndef SOLANG_MAIN_WINDOW_H
#define SOLANG_MAIN_WINDOW_H

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
    public NonCopyable
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
        present_dock_object(DockObjectPtr dock_object) throw();

        Gtk::Notebook *
        get_notebook_center() throw();

        const UIManagerPtr &
        get_ui_manager() throw();

        void
        set_busy(bool busy) throw();

        void
        connect_progress(
                const ProgressObserverPtr &observer ) throw();


    protected:
        std::string
        get_user_layout_file() throw();

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

        ApplicationPtr application_;

        void
        handle_progress_event() throw();

        ActionGroupPtr actionGroup_;

        UIManagerPtr uiManager_;

        Gtk::UIManager::ui_merge_id uiID_;

        Gtk::VBox vBox_;

        Gtk::SeparatorToolItem separatorToolItem_;

        SpinnerToolItem spinnerToolItem_;

        Gtk::HBox hBox_;

        Gtk::Statusbar statusBar_;

        Gtk::ProgressBar progress_;

        ProgressObserverPtr observer_;

        GtkWidget * const dock_;

        GtkWidget * const dockBar_;

        GdlDockLayout * const layout_;

        bool showToolBar_;

        bool showStatusBar_;

        std::vector<DockObjectPtr> dockObjectsLeftTop_;

        std::vector<DockObjectPtr> dockObjectsLeftBottom_;

        std::vector<DockObjectPtr> dockObjectsCenter_;

    private:
};

} // namespace Solang

#endif // SOLANG_MAIN_WINDOW_H
