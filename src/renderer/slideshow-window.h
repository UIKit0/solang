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

#ifndef SOLANG_SLIDESHOW_WINDOW_H
#define SOLANG_SLIDESHOW_WINDOW_H

#include <gdkmm.h>
#include <gtkmm.h>
#include <sigc++/sigc++.h>

#include "non-copyable.h"
#include "types.h"

namespace Solang
{

class SlideshowWindow :
    public Gtk::Window,
    private NonCopyable
{
    public:
        SlideshowWindow() throw();

        virtual
        ~SlideshowWindow() throw();

        const UIManagerPtr &
        get_ui_manager() throw();

        void
        on_image_view_scroll_event(GdkScrollDirection direction)
                                   throw();

        void
        render(const PixbufPtr & pixbuf) throw();

    protected:
        void
        on_action_view_best_fit() throw();

        void
        on_action_view_normal_size() throw();

        void
        on_action_view_zoom_in() throw();

        void
        on_action_view_zoom_out() throw();

        virtual bool
        on_motion_notify_event(GdkEventMotion * event) throw();

        virtual void
        on_show() throw();

        bool
        on_tool_bar_timeout() throw();

        ActionGroupPtr actionGroup_;

        UIManagerPtr uiManager_;

        Gtk::UIManager::ui_merge_id uiID_;

        Gtk::VBox vBox_;

        GtkWidget * imageScrollWin_;

        GtkWidget * imageView_;

        sigc::connection signalTimeout_;

    private:
};

} // namespace Solang

#endif // SOLANG_SLIDESHOW_WINDOW_H
