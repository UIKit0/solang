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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "hig-dialog.h"

namespace Solang
{

HIGDialog::HIGDialog() throw() :
    Gtk::Dialog(),
    vBox_(false, 0)
{
    setup_gui();
}

HIGDialog::HIGDialog(const Glib::ustring & title,
                     bool modal,
                     bool use_separator) throw() :
    Gtk::Dialog(title, modal, use_separator),
    vBox_(false, 0)
{
    setup_gui();
}

HIGDialog::HIGDialog(const Glib::ustring & title,
                     Gtk::Window & parent,
                     bool modal,
                     bool use_separator) throw() :
    Gtk::Dialog(title, parent, modal, use_separator),
    vBox_(false, 0)
{
    setup_gui();
}

HIGDialog::~HIGDialog() throw()
{
}

Gtk::VBox &
HIGDialog::get_content_area() throw()
{
    return vBox_;
}

void
HIGDialog::setup_gui() throw()
{
    // Dialog
    // +------------------------------------------------+
    // |                                                |
    // |     VBox                                       |
    // |     +------------------------------------+     |
    // |     |                                    |     |
    // |     |     Content Area                   |     |
    // |     |     +------------------------+     |     |
    // |     |     |                        |     |     |
    // |     |     |     Content            |     |     |
    // |     |     |     +------------+     |     |     |
    // |     |     |     |            |     |     |     |
    // |     |     |     |            |     |     |     |
    // |     |     |     |            |<-5->|<-2->|<-5->|
    // |     |     |     |            |     |     |     |
    // |     |     |     |            |     |     |     |
    // |     |     |     +------------+     |     |     |
    // |     |     |                        |     |     |
    // |     |     |                        |     |     |
    // |     |     +------------------------+     |     |
    // |     |                                    |     |
    // |     |     Action Area                    |     |
    // |     |     +------------------------+     |     |
    // |     |     |                        |     |     |
    // |     |     |         Button         |     |     |
    // |     |     |         +--------+     |     |     |
    // |     |     |         |        |<-5->|<-2->|<-5->|
    // |     |     |         +--------+     |     |     |
    // |     |     |                        |     |     |
    // |     |     |                        |     |     |
    // |     |     +------------------------+     |     |
    // |     |                                    |     |
    // |     |                                    |     |
    // |     +------------------------------------+     |
    // |                                                |
    // |                                                |
    // +------------------------------------------------+

    set_border_width(5);

    Gtk::VBox * const vbox = get_vbox();
    vbox->pack_start(vBox_, Gtk::PACK_SHRINK, 0);

    vBox_.set_border_width(5);

    show_all_children();
}

} // namespace Solang
