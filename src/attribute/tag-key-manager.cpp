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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <glibmm.h>
#include <glibmm/i18n.h>
#include <sigc++/sigc++.h>

#include "tag-key-manager.h"
#include "tag.h"

#define NBR_KEYS 36

namespace Solang
{

const Glib::ustring valid_keys[NBR_KEYS] = {"0", "1", "2", "3", "4", "5", "6", "7", "8",
      "9", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
      "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"};

TagKeyManager::TagKeyManager() throw()
{
    gsettings_ = g_settings_new("org.gnome.solang.keys");
}

TagKeyManager::~TagKeyManager() throw()
{
}

bool
TagKeyManager::is_key_valid(const Glib::ustring key)
{
    for(int i = 0; i<NBR_KEYS-1; i++)
    {
        if (valid_keys[i] == key)
        {
            return true;
        }
    }
    return false;
}

bool
TagKeyManager::is_key_used(const Glib::ustring key)
{
    return get_tag_for_key(key) != "";
}

Glib::ustring
TagKeyManager::get_tag_for_key(const Glib::ustring key)
{
    if (is_key_valid(key))
    {
        gchar * tag_urn = g_settings_get_string(gsettings_,
                                Glib::ustring::compose("key-%1", key).c_str());
        return Glib::ustring(tag_urn);
    }
    return "";
}

Glib::ustring
TagKeyManager::get_key_for_tag(const Glib::ustring tag_urn)
{
    for(int i = 0; i<NBR_KEYS-1; i++)
    {
        if (Glib::ustring(g_settings_get_string(gsettings_,
            Glib::ustring::compose("key-%1", valid_keys[i]).c_str()))
            == tag_urn)
        {
            return valid_keys[i];
        }
    }
    return "";
}

void
TagKeyManager::set_tag_for_key(const Glib::ustring key,
                               const Glib::ustring tag_urn)
{
    if (is_key_valid(key))
    {
        g_settings_set_string(gsettings_,
                              Glib::ustring::compose("key-%1", key).c_str(),
                              tag_urn.c_str());
    }
}

void
TagKeyManager::save_tag(const Glib::ustring new_key,
                        const Glib::ustring tag_urn)
{
    if (new_key != "")
    {
        set_tag_for_key(new_key, tag_urn);
    }
    else
    {
        if (get_key_for_tag(tag_urn) != "")
        {
            set_tag_for_key(new_key, "");
        }
    }
}

} // namespace Solang
