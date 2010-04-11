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

#ifndef SOLANG_DB_OBJECT_H
#define SOLANG_DB_OBJECT_H

#include "delete-action.h"
#include "error.h"
#include "non-copyable.h"
#include "types.h"

namespace Solang
{

class Database;

//Represents an object that is serialized to a database

class DBObject :
    private NonCopyable
{
    public:
        typedef sigc::slot<void> SlotAsyncReady;

        virtual
        ~DBObject() throw();

        virtual void
        delete_async(Database & database, const SlotAsyncReady & slot)
                     const throw() = 0;

        virtual Glib::ustring
        get_delete_query() const throw() = 0;

        virtual Glib::ustring
        get_save_query() const throw() = 0;

        virtual void
        save_async(Database & database, const SlotAsyncReady & slot)
                   const throw() = 0;

        inline bool
        get_is_deleted() const throw();

        void
        set_is_deleted( bool value ) throw();

    protected:
        DBObject() throw();

    private:
        bool isDeleted_;
};

inline bool
DBObject::get_is_deleted() const throw()
{
    return isDeleted_;
}

} // namespace Solang

#endif // SOLANG_DB_OBJECT_H
