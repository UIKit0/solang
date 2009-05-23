/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * photo-search-criteria.h
 * Copyright (C) Santanu Sinha 2009 <santanu.sinha@gmail.com>
 * 
 * photo-search-criteria.h is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * photo-search-criteria.h is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SOLANG_PHOTO_SEARCH_CRITERIA_H
#define SOLANG_PHOTO_SEARCH_CRITERIA_H

#include <tr1/memory>

#include <glibmm.h>

#include "non-copyable.h"
#include "types.h"

namespace Solang
{

class PhotoSearchCriteria :
    public NonCopyable
{
    public:
        enum ClubbingOperationType
        {
            CLUB_AND,   // All criterions of this type will be
                        //clubbed using "AND"
            CLUB_OR     // All criterions of this type will be
                        //clubbed using "OR"
        };
    protected:
        PhotoSearchCriteria();

    public:
        virtual
        ~PhotoSearchCriteria() throw();

        //Returns SQL formatted string that can be appended
        //in the where clause
        virtual Glib::ustring
        get_query_criteria() const throw() = 0;

        virtual ClubbingOperationType
        get_clubbing_type() const throw() = 0;

        virtual gint32
        get_id() const throw() = 0; //Each class should keep a static
                                    //constant and return the
                                    //value here
        virtual Glib::ustring
        get_criteria_description() const throw() = 0;

        virtual Glib::ustring
        get_criteria_icon_path() const throw() = 0;

};

} // namespace Solang

#endif // SOLANG_PHOTO_SEARCH_CRITERIA_H
