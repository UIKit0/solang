/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
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

#ifndef SOLANG_TAG_KEY_MANAGER_H
#define SOLANG_TAG_KEY_MANAGER_H

#include <string>
#include <map>

#include <glibmm.h>
#include <gtkmm.h>

#include "types.h"

namespace Solang
{

class TagKeyManager
{
    public:
        TagKeyManager() throw();

        virtual
        ~TagKeyManager() throw();

        bool
        is_key_valid(const Glib::ustring key);

        bool
        is_key_used(const Glib::ustring key);

        Glib::ustring
        get_tag_for_key(const Glib::ustring key);

        Glib::ustring
        get_key_for_tag(const Glib::ustring tag_urn);

        void
        set_tag_for_key(const Glib::ustring key, const Glib::ustring tag_urn);

        void
        save_tag(const Glib::ustring new_key, const Glib::ustring tag_urn);

    protected:
        GSettings* gsettings_;

    private:
};

} // namespace Solang

#endif // SOLANG_TAG_KEY_MANAGER_H
