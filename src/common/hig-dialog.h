/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * Copyright (C) 2010 Debarshi Ray <rishi@gnu.org>
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

#ifndef SOLANG_HIG_DIALOG_H
#define SOLANG_HIG_DIALOG_H

#include <gtkmm.h>

namespace Solang
{

class HIGDialog :
    public Gtk::Dialog
{
    public:
        HIGDialog() throw();

        HIGDialog(const Glib::ustring & title,
                  bool modal = false,
                  bool use_separator = false) throw();

        HIGDialog(const Glib::ustring & title,
                  Gtk::Window & parent,
                  bool modal = false,
                  bool use_separator = false) throw();

        virtual
        ~HIGDialog() throw() = 0;

        Gtk::VBox &
        get_content_area() throw();

    private:
        void
        setup_gui() throw();

        Gtk::VBox vBox_;
};

} // namespace Solang

#endif // SOLANG_HIG_DIALOG_H
