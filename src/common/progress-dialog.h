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
 * libnautilus-private/nautilus-progress-info.h
 */

#ifndef SOLANG_PROGRESS_DIALOG_H
#define SOLANG_PROGRESS_DIALOG_H

#include <gtkmm.h>

namespace Solang
{

class ProgressDialog :
    public Gtk::Dialog 
{
    public:
        ProgressDialog(Gtk::Window & parent) throw();

        virtual
        ~ProgressDialog() throw();

        void
        attach(Gtk::Widget & child) throw();

        void
        detach(Gtk::Widget & child) throw();

    protected:
        virtual bool
        on_delete_event(GdkEventAny * event);

    private:
        guint num_;
};

} // namespace Solang

#endif // SOLANG_PROGRESS_DIALOG_H
