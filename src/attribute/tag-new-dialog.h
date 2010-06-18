/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * Copyright (C) 2009 Debarshi Ray <rishi@gnu.org>
 * Copyright (C) 2010 Florent Thévenet <feuloren@free.fr>
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

#ifndef SOLANG_TAG_NEW_DIALOG_H
#define SOLANG_TAG_NEW_DIALOG_H

#include <string>

#include <gtkmm.h>

#include "types.h"
#include "tag-key-manager.h"
#include "tag-manager.h"

namespace Solang
{

class TagNewDialog :
    public Gtk::Dialog
{
    public:
        TagNewDialog( TagManager *tagmanager ) throw();
        TagNewDialog( TagManager *tagmanager, const TagPtr &tag ) throw();

        virtual
        ~TagNewDialog() throw();

        inline Glib::ustring
        get_name() const;

        inline Glib::ustring
        get_key() const;

        inline const std::string &
        get_icon_path() const;

        inline const Glib::ustring
        get_description();

    protected:
        virtual bool
        on_delete_event(GdkEventAny * event);

        virtual void
        on_response(int response_id);

        void
        on_icon_button_clicked() throw();

        void
        on_key_entry_changed() throw();

        void
        setup_gui() throw();

        void
        set_icon( const Glib::ustring &iconPath);

        std::string iconPath_;

        TagManager * tagManager_;

        TagKeyManager * keyManager_;

        const Glib::ustring tag_urn_;

        Gtk::Table mainTable_;

        Gtk::Button iconButton_;

        Gtk::Image iconImage_;

        Gtk::Label parentLabel_;

        Gtk::ComboBox parentComboBox_;

        Gtk::Label nameLabel_;

        Gtk::Entry nameEntry_;

        Gtk::Label descriptionLabel_;

        Gtk::ScrolledWindow descriptionScrolledWindow_;

        Gtk::TextView descriptionTextView_;

        Gtk::Label keyLabel_;

        Gtk::Entry keyEntry_;

    private:
};

inline Glib::ustring
TagNewDialog::get_name() const
{
    return nameEntry_.get_text();
}

inline Glib::ustring
TagNewDialog::get_key() const
{
    return keyEntry_.get_text();
}

inline const std::string &
TagNewDialog::get_icon_path() const
{
    return iconPath_;
}

inline const Glib::ustring
TagNewDialog::get_description()
{
    Glib::RefPtr<Gtk::TextBuffer> buf
                = descriptionTextView_.get_buffer();
    return buf->get_slice( buf->begin(), buf->end());
}

} // namespace Solang

#endif // SOLANG_TAG_NEW_DIALOG_H
