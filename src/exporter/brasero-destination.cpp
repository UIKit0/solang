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

#include <brasero-burn-dialog.h>
#include <brasero-burn-lib.h>
#include <glibmm/i18n.h>

#include "brasero-destination.h"
#include "i-progress-observer.h"
#include "photo.h"
#include "photo-destination-enums.h"

namespace Solang
{

BraseroDestination::BraseroDestination() throw() :
    PhotoDestination(),
    braseroDrive_(0),
    braseroSessionCfg_(0),
    braseroTrackDataCfg_(0),
    braseroDriveSelection_(0),
    initEnd_()
{
    brasero_burn_library_start(0, 0);
}

BraseroDestination::~BraseroDestination() throw()
{
    brasero_burn_library_stop();
}

void
BraseroDestination::init(Application & application) throw()
{
    initEnd_.emit(true);
}

void
BraseroDestination::final(Application & application) throw()
{
}

void
BraseroDestination::export_photo_async(
                          const PhotoPtr & photo,
                          const ProgressObserverPtr &) throw()
{
    brasero_track_data_cfg_add(braseroTrackDataCfg_,
                               photo->get_uri().c_str(),
                               0);
}

void
BraseroDestination::export_photos_async(
                          const PhotoList & photos,
                          const ProgressObserverPtr & observer)
                          throw()
{
    if (0 == braseroDrive_)
    {
        return;
    }

    braseroSessionCfg_ = brasero_session_cfg_new();
    brasero_burn_session_set_burner(
        BRASERO_BURN_SESSION(braseroSessionCfg_), braseroDrive_);
    g_object_unref(braseroDrive_);
    braseroDrive_ = 0;

    braseroTrackDataCfg_ = brasero_track_data_cfg_new();
    brasero_burn_session_add_track(
        BRASERO_BURN_SESSION(braseroSessionCfg_),
        BRASERO_TRACK(braseroTrackDataCfg_),
        NULL);

    PhotoList::const_iterator it;

    for (it = photos.begin(); photos.end() != it; it++)
    {
        export_photo_async(*it, observer);
    }

    g_object_unref(braseroTrackDataCfg_);
    braseroTrackDataCfg_ = 0;

    brasero_burn_begin();
}

void BraseroDestination::final() throw()
{
}

sigc::signal<void, bool> &
BraseroDestination::init_end() throw()
{
    return initEnd_;
}

void
BraseroDestination::read_selection() throw()
{
    braseroDrive_
        = brasero_drive_selection_get_active(
              BRASERO_DRIVE_SELECTION(braseroDriveSelection_));
}

Gtk::Widget &
BraseroDestination::get_browser() throw()
{
    braseroDriveSelection_ = brasero_drive_selection_new();
    brasero_drive_selection_show_type(
        BRASERO_DRIVE_SELECTION(braseroDriveSelection_),
        static_cast<BraseroDriveType>(BRASERO_DRIVE_TYPE_FILE
                                      | BRASERO_DRIVE_TYPE_WRITER));

    return *Gtk::manage(Glib::wrap(
                            GTK_COMBO_BOX(braseroDriveSelection_),
                            false));
}

Glib::ustring
BraseroDestination::get_label() const throw()
{
    return Glib::ustring(_("_CD/DVD..."));
}

Glib::ustring
BraseroDestination::get_name() const throw()
{
    return Glib::ustring("Brasero");
}

gint
BraseroDestination::get_options() const throw()
{
    return PHOTO_DESTINATION_CREATE_ARCHIVE_FALSE;
}

Gtk::StockID
BraseroDestination::get_stock_id() const throw()
{
    return Gtk::Stock::CDROM;
}

void
BraseroDestination::set_create_archive(bool value) throw()
{
    // NB: Creation of archives is not supported.
}

void
BraseroDestination::brasero_burn_begin() throw()
{
    Glib::signal_idle().connect_once(
        sigc::mem_fun(*this,
                      &BraseroDestination::brasero_burn_begin_idle),
        Glib::PRIORITY_LOW);
}

void
BraseroDestination::brasero_burn_begin_idle() throw()
{
    GtkWidget * const burn_dialog = brasero_burn_dialog_new();

    gtk_window_set_title(GTK_WINDOW(burn_dialog),
                         _("Export to CD/DVD"));
    brasero_burn_dialog_run(BRASERO_BURN_DIALOG(burn_dialog),
                            BRASERO_BURN_SESSION(braseroSessionCfg_));
    gtk_widget_destroy(burn_dialog);

    g_object_unref(braseroSessionCfg_);
    braseroSessionCfg_ = 0;
}

} // namespace Solang
