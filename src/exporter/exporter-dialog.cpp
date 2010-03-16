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

#include <glibmm/i18n.h>

#include "exporter-dialog.h"
#include "photo-destination-enums.h"

namespace Solang
{

ExporterDialog::ExporterDialog(Gtk::Widget & browser,
                               gint source_options) throw() :
    Gtk::Dialog(_("Export"), true, false),
    generalVBox_(false, 6),
    optionsVBox_(false, 6),
    generalLabel_("", Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER, false),
    generalAlignment_(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER, 1.0, 1.0),
    optionsLabel_("", Gtk::ALIGN_LEFT, Gtk::ALIGN_CENTER, false),
    optionsAlignment_(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER, 1.0, 1.0),
    generalHBox_(false, 12),
    optionsVBox2_(false, 6),
    destinationLabel_(_("_Destination:"), Gtk::ALIGN_LEFT,
                      Gtk::ALIGN_CENTER, true),
    duplicatesCheckButton_(_("_Detect duplicates"), true),
    archiveCheckButton_((source_options
                         & PHOTO_DESTINATION_CREATE_ARCHIVE_OPTIONAL)
                         ? Gtk::manage(new Gtk::CheckButton(
                               _("_Create an archive"),
                               true))
                         : 0),
    exportButton_(),
    exportImage_(Gtk::Stock::APPLY, Gtk::ICON_SIZE_BUTTON),
    sourceOptions_(source_options)
{
    set_border_width(12);
    set_resizable(false);

    Gtk::VBox * const dialog_vbox = get_vbox();
    dialog_vbox->set_spacing(18);

    dialog_vbox->pack_start(generalVBox_, Gtk::PACK_SHRINK, 0);
    dialog_vbox->pack_start(optionsVBox_, Gtk::PACK_SHRINK, 0);

    generalLabel_.set_use_markup(true);
    {
        Glib::ustring markup = "<b>";
        markup += C_("A group of UI controls", "General");
        markup += "</b>";
        generalLabel_.set_markup(markup);
    }
    generalVBox_.pack_start(generalLabel_, Gtk::PACK_SHRINK, 0);

    generalAlignment_.set_padding(0, 0, 12, 0);
    generalVBox_.pack_start(generalAlignment_, Gtk::PACK_SHRINK, 0);

    optionsLabel_.set_use_markup(true);
    {
        Glib::ustring markup = "<b>";
        markup += C_("A group of UI controls", "Options");
        markup += "</b>";
        optionsLabel_.set_markup(markup);
    }
    optionsVBox_.pack_start(optionsLabel_, Gtk::PACK_SHRINK, 0);

    optionsAlignment_.set_padding(0, 0, 12, 0);
    optionsVBox_.pack_start(optionsAlignment_, Gtk::PACK_SHRINK, 0);

    generalAlignment_.add(generalHBox_);
    optionsAlignment_.add(optionsVBox2_);

    destinationLabel_.set_use_underline(true);
    generalHBox_.pack_start(destinationLabel_, Gtk::PACK_SHRINK, 0);

    // Insert a Gtk::Widget specific to the particular source.
    generalHBox_.pack_start(browser, Gtk::PACK_EXPAND_WIDGET, 0);

    duplicatesCheckButton_.set_use_underline(true);
    optionsVBox2_.pack_start(duplicatesCheckButton_,
                             Gtk::PACK_SHRINK, 0);

    if (0 != archiveCheckButton_)
    {
        archiveCheckButton_->set_use_underline(true);
        optionsVBox2_.pack_start(*archiveCheckButton_,
                                 Gtk::PACK_SHRINK, 0);
    }

    add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);

    exportButton_.set_image(exportImage_);
    exportButton_.set_use_underline(true);
    exportButton_.set_label(_("_Export"));
    add_action_widget(exportButton_, Gtk::RESPONSE_APPLY);

    show_all_children();
}

ExporterDialog::~ExporterDialog() throw()
{
}

bool
ExporterDialog::get_create_archive() const throw()
{
    return (sourceOptions_ & PHOTO_DESTINATION_CREATE_ARCHIVE_OPTIONAL)
           ? archiveCheckButton_->get_active()
           : sourceOptions_ & PHOTO_DESTINATION_CREATE_ARCHIVE_TRUE;
}

bool
ExporterDialog::on_delete_event(GdkEventAny * event)
{
    hide();
    return false;
}

void
ExporterDialog::on_response(int response_id)
{
    hide();
}

} // namespace Solang
