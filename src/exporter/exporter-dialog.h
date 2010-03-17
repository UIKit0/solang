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

#ifndef SOLANG_EXPORTER_DIALOG_H
#define SOLANG_EXPORTER_DIALOG_H

#include <gtkmm.h>

#include "hig-dialog.h"
#include "types.h"

namespace Solang
{

class ExporterDialog :
    public HIGDialog
{
    public:
        ExporterDialog(Gtk::Widget & browser,
                       gint source_options) throw();

        virtual
        ~ExporterDialog() throw();

        bool
        get_create_archive() const throw();

    protected:
        virtual bool
        on_delete_event(GdkEventAny * event);

        virtual void
        on_response(int response_id);

        Gtk::VBox generalVBox_;

        Gtk::VBox optionsVBox_;

        Gtk::Label generalLabel_;

        Gtk::Alignment generalAlignment_;

        Gtk::Label optionsLabel_;

        Gtk::Alignment optionsAlignment_;

        Gtk::HBox generalHBox_;

        Gtk::VBox optionsVBox2_;

        Gtk::Label destinationLabel_;

        Gtk::CheckButton duplicatesCheckButton_;

        Gtk::CheckButton * archiveCheckButton_;

        Gtk::Button exportButton_;

        Gtk::Image exportImage_;

        gint sourceOptions_;

    private:
};

} // namespace Solang

#endif // SOLANG_EXPORTER_DIALOG_H
