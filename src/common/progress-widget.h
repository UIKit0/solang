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

/*
 * Based on code by: Alexander Larsson
 *
 * This widget was originally written in C as a part of Nautilus:
 * libnautilus-private/nautilus-progress-info.h
 */

#ifndef SOLANG_WIDGET_H
#define SOLANG_WIDGET_H

#include <glibmm.h>
#include <gtkmm.h>

namespace Solang
{

class ProgressWidget :
    public Gtk::VBox
{
    public:
        ProgressWidget() throw();

        virtual
        ~ProgressWidget() throw();

        void
        set_details(const Glib::ustring & details) throw();

        void
        set_fraction(gdouble fraction) throw();

        void
        set_status(const Glib::ustring & status) throw();

        Glib::SignalProxy0<void>
        signal_cancelled() throw();

    private:
        Gtk::Button button_;

        Gtk::HBox hBox_;

        Gtk::Image image_;

        Gtk::Label detailsLabel_;

        Gtk::Label statusLabel_;

        Gtk::ProgressBar progressBar_;

        Gtk::VBox vBox_;
};

} // namespace Solang

#endif // SOLANG_WIDGET_H
