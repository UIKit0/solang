/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * Copyright (C) 2009, 2010 Debarshi Ray <rishi@gnu.org>
 * Copyright (C) 2009 Santanu Sinha <santanu.sinha@gmail.com>
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

#include <sstream>

#include "id-base.h"
#include "modification-date.h"


namespace Solang
{

Glib::ustring
ModificationDate::get_query_criteria() const throw()
{
    return Glib::ustring();
}

IPhotoSearchCriteria::ClubbingOperationType
ModificationDate::get_clubbing_type() const throw()
{
    return IPhotoSearchCriteria::CLUB_OR;
}

gint32
ModificationDate::get_id() const throw()
{
    return IDBase<ModificationDate>::get_id();
}

Glib::ustring
ModificationDate::get_criteria_description() const throw()
{
    std::ostringstream sout;
    if( get_day() == 0 )
    {
        if( get_month() != 0 )
        {
            sout<<"Month: "<<get_month()<<" ";
        }
        if( get_year() != 0 )
        {
            sout<<"Year: "<<get_year();
        }
    }
    else
    {
        sout<<"Date: "<<get_day()
            <<"/"<<get_month()
            <<"/"<<get_year();
    }

    return sout.str();
}

Glib::ustring
ModificationDate::get_criteria_icon_path() const throw()
{
    //TBD::CORRECT
    return PACKAGE_DATA_DIR G_DIR_SEPARATOR_S
           "pixmaps" G_DIR_SEPARATOR_S
           "tag-16.png";
}

} // namespace Solang
