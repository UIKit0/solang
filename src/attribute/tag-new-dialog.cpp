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

#include <glibmm.h>
#include <glibmm/i18n.h>
#include <sigc++/sigc++.h>

#include "tag-new-dialog.h"
#include "tag.h"

namespace Solang
{

TagNewDialog::TagNewDialog() throw() :
    Gtk::Dialog(),
    iconPath_(""),
    mainTable_(3, 3, false),
    iconButton_(),
    iconImage_(Gtk::Stock::MISSING_IMAGE, Gtk::ICON_SIZE_BUTTON),
    parentLabel_(_("Parent:"),
                 Gtk::ALIGN_LEFT,
                 Gtk::ALIGN_CENTER,
                 false),
    parentComboBox_(),
    nameLabel_(_("Name:"),
               Gtk::ALIGN_LEFT,
               Gtk::ALIGN_CENTER,
               false),
    nameEntry_(),
    descriptionLabel_(_("Description:"),
                      Gtk::ALIGN_LEFT,
                      Gtk::ALIGN_CENTER,
                      false),
    descriptionScrolledWindow_(),
    descriptionTextView_()
{
    set_title(_("Create New Tag"));

    setup_gui();

    show_all_children();
}

TagNewDialog::TagNewDialog( const TagPtr &tag ) throw() :
    Gtk::Dialog(),
    iconPath_( ),
    mainTable_(3, 3, false),
    iconButton_(),
    iconImage_(Gtk::Stock::MISSING_IMAGE, Gtk::ICON_SIZE_BUTTON),
    parentLabel_(_("Parent:"),
                 Gtk::ALIGN_LEFT,
                 Gtk::ALIGN_CENTER,
                 false),
    parentComboBox_(),
    nameLabel_(_("Name:"),
               Gtk::ALIGN_LEFT,
               Gtk::ALIGN_CENTER,
               false),
    nameEntry_(),
    descriptionLabel_(_("Description:"),
                      Gtk::ALIGN_LEFT,
                      Gtk::ALIGN_CENTER,
                      false),
    descriptionScrolledWindow_(),
    descriptionTextView_()
{
    set_title(_("Edit Tag"));

    setup_gui();

    //populate
    if( tag )
    {
        nameEntry_.set_text(tag->get_name());
        Glib::RefPtr<Gtk::TextBuffer> description = Gtk::TextBuffer::create();
        description->set_text( tag->get_description() );
        descriptionTextView_.set_buffer(description);
        set_icon(tag->get_icon_path());
    }

    show_all_children();
}

TagNewDialog::~TagNewDialog() throw()
{
}

bool
TagNewDialog::on_delete_event(GdkEventAny * event)
{
    bool return_value = Gtk::Dialog::on_delete_event(event);

    hide();
    return return_value;
}

void
TagNewDialog::on_response(int response_id)
{
    hide();
}

void
TagNewDialog::on_icon_button_clicked() throw()
{
    Gtk::FileChooserDialog file_chooser_dialog(
                               *this, _("Select Tag Icon"),
                               Gtk::FILE_CHOOSER_ACTION_OPEN);

//    file_chooser_dialog.add_button(Gtk::Stock::REVERT_TO_SAVED,
//                                   Gtk::RESPONSE_REJECT);
    file_chooser_dialog.add_button(Gtk::Stock::CANCEL,
                                   Gtk::RESPONSE_CANCEL);
    file_chooser_dialog.add_button(Gtk::Stock::OPEN,
                                   Gtk::RESPONSE_OK);

    if (false == iconPath_.empty())
    {
        Glib::ustring path;
        try
        {
            path = Glib::filename_to_utf8(
                       Glib::path_get_dirname(iconPath_));
            file_chooser_dialog.set_current_folder(path);
        }
        catch (const Glib::ConvertError & e)
        {
            g_warning("%s", e.what().c_str());
        }
    }

    file_chooser_dialog.set_local_only(true);
    file_chooser_dialog.set_select_multiple(false);

    const gint response = file_chooser_dialog.run();

    switch (response)
    {
        case Gtk::RESPONSE_OK:
        {
            std::string path;
            try
            {
                path = Glib::filename_from_utf8(
                           file_chooser_dialog.get_filename());
            }
            catch (const Glib::ConvertError & e)
            {
                g_warning("%s", e.what().c_str());
                break;
            }

            if (true == path.empty())
            {
                break;
            }
            set_icon( path );
            break;
        }

        case Gtk::RESPONSE_CANCEL:
        case Gtk::RESPONSE_DELETE_EVENT:
//        case Gtk::RESPONSE_REJECT:
        {
            break;
        }

        default:
        {
            break;
        }
    }
}

void
TagNewDialog::setup_gui() throw()
{
    set_border_width(12);
    set_default_size(320, 200);
    set_has_separator(false);

    Gtk::VBox * const dialog_vbox = get_vbox();
    dialog_vbox->set_spacing(18);

    mainTable_.set_col_spacings(12);
    mainTable_.set_row_spacings(6);
    dialog_vbox->pack_start(mainTable_, Gtk::PACK_EXPAND_WIDGET, 0);

    iconButton_.set_image(iconImage_);
    iconButton_.set_size_request(64, 64);
    iconButton_.signal_clicked().connect(sigc::mem_fun(*this,
        &TagNewDialog::on_icon_button_clicked));
//    mainTable_.attach(iconButton_, 0, 1, 0, 2,
//                      Gtk::FILL | Gtk::EXPAND,
//                      Gtk::FILL | Gtk::EXPAND,
//                      0, 0);

//    mainTable_.attach(parentLabel_, 1, 2, 0, 1,
//                      Gtk::FILL | Gtk::EXPAND,
//                      Gtk::FILL | Gtk::EXPAND,
//                      0, 0);

//    mainTable_.attach(parentComboBox_, 2, 3, 0, 1,
//                      Gtk::FILL | Gtk::EXPAND,
//                      Gtk::FILL | Gtk::EXPAND,
//                      0, 0);

    mainTable_.attach(nameLabel_, 1, 2, 1, 2,
                      Gtk::FILL | Gtk::EXPAND,
                      Gtk::FILL | Gtk::EXPAND,
                      0, 0);

    mainTable_.attach(nameEntry_, 2, 3, 1, 2,
                      Gtk::FILL | Gtk::EXPAND,
                      Gtk::FILL | Gtk::EXPAND,
                      0, 0);

    mainTable_.attach(descriptionLabel_, 1, 2, 2, 3,
                      Gtk::FILL | Gtk::EXPAND,
                      Gtk::FILL | Gtk::EXPAND,
                      0, 0);

    descriptionScrolledWindow_.set_policy(Gtk::POLICY_AUTOMATIC,
                                          Gtk::POLICY_AUTOMATIC);
    descriptionScrolledWindow_.set_shadow_type(Gtk::SHADOW_IN);
    mainTable_.attach(descriptionScrolledWindow_, 2, 3, 2, 3,
                      Gtk::FILL | Gtk::EXPAND,
                      Gtk::FILL | Gtk::EXPAND,
                      0, 0);

    descriptionScrolledWindow_.add(descriptionTextView_);

    add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

    return;

}

void
TagNewDialog::set_icon(const Glib::ustring &iconPath)
{
    iconPath_ = iconPath;

    if (true == iconPath_.empty())
    {
        return;
    }

    PixbufPtr pixbuf;
    try
    {
        pixbuf = Gdk::Pixbuf::create_from_file(iconPath_);
    }
    catch (const Glib::FileError & e)
    {
        g_warning("%s", e.what().c_str());
        return;
    }
    catch (const Gdk::PixbufError & e)
    {
        g_warning("%s", e.what().c_str());
        return;
    }

    iconImage_.set(pixbuf);

    return;
}

} // namespace Solang
