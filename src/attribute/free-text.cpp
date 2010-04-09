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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "id-base.h"
#include "free-text.h"

namespace Solang
{

FreeText::FreeText(const Glib::ustring & text) throw() :
    IPhotoSearchCriteria(),
    text_(text)
{
}

FreeText::~FreeText() throw()
{
}

Glib::ustring
FreeText::get_query_criteria() const throw()
{
    // FIXME: ?photo is used to denote the nmm:Photo in the actual
    //        query in Database::search_async. Need to find a better
    //        way to do this.

    return Glib::ustring::compose(
        "nie:url ?url ."
        "  OPTIONAL {"
        "    ?photo nao:hasTag ?tag ."
        "    ?tag nao:prefLabel ?label"
        "  }"
        "FILTER("
        "  REGEX(?url, '%1', 'i')"
        "  || (BOUND(?label) && REGEX(?label, '%1', 'i'))"
        ")",
        text_);
}

const Glib::ustring &
FreeText::get_text() const throw()
{
    return text_;
}

IPhotoSearchCriteria::ClubbingOperationType
FreeText::get_clubbing_type() const throw()
{
    return IPhotoSearchCriteria::CLUB_AND;
}

gint32
FreeText::get_id() const throw()
{
    return IDBase<FreeText>::get_id();
}

Glib::ustring
FreeText::get_criteria_description() const throw()
{
    return "Text: " + get_text();
}

Glib::ustring
FreeText::get_criteria_icon_path() const throw()
{
    return Glib::ustring();
}

} // namespace Solang
