/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * Copyright (C) 2010, 2009 Debarshi Ray <rishi@gnu.org>
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

/*
 * Based on code by: Alexander Larsson
 *
 * This widget was originally written in C as a part of Nautilus:
 * libnautilus-private/nautilus-progress-info.c
 */

#include "config.h"

#include <glibmm/i18n.h>

#include "progress-dialog.h"

namespace Solang
{

ProgressDialog::ProgressDialog(Gtk::Window & parent) throw() :
    Gtk::Dialog(_("Pending Operations"), parent, false, false),
    num_(0)
{
    Gtk::VBox * const vbox = get_vbox();
    vbox->set_homogeneous(false);
    vbox->set_spacing(5);

    set_border_width(10);
    set_position(Gtk::WIN_POS_CENTER);
    set_resizable(false);

    show_all_children();
}

ProgressDialog::~ProgressDialog() throw()
{
}

void
ProgressDialog::attach(Gtk::Widget & child) throw()
{
    Gtk::VBox * const vbox = get_vbox();
    vbox->pack_start(child, false, false, 6);

    num_++;
    child.show_all();
    present();
}

void
ProgressDialog::detach(Gtk::Widget & child) throw()
{
    Gtk::VBox * const vbox = get_vbox();
    vbox->remove(child);

    num_--;
    if (0 == num_)
    {
        hide();
    }
}

bool
ProgressDialog::on_delete_event(GdkEventAny * event)
{
    const bool return_value = Gtk::Dialog::on_delete_event(event);

    hide();
    return return_value;
}

} // namespace Solang
