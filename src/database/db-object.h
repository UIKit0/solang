/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
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

#include <libgdamm.h>

#include "delete-action.h"
#include "error.h"
#include "non-copyable.h"
#include "types.h"

namespace Solang
{

class Database;

//Represents an object that is serialized to a database

class DBObject :
    public NonCopyable
{
    private:
        gint32 row_;
        DBTablePtr table_;
        bool isDeleted_;

    protected:
        DBObject() throw();

    public:
        virtual
        ~DBObject() throw();

        void
        save(Database & db) throw(Error);

        virtual void
        insert( DataModelPtr &model, gint32 lastIndex ) throw(Error) = 0;

        virtual void
        update( DataModelPtr &model ) throw(Error);

        virtual void
        update( DataModelPtr &model, gint32 row ) throw(Error) = 0;

        virtual void
        remove( DataModelPtr &model ) throw(Error);

        virtual void
        remove( DataModelPtr &model, gint32 row ) throw(Error);

        virtual void
        remove_physical_existence() throw();
    
        //create insert into ... SQL and save this object to DB    
        //virtual Glib::ustring getCreateSQL() throw(Error) = 0;
        
        virtual void
        create( Database & db, gint32 row ) throw(Error);

        //Construct object from a row in the 
        //given datamodel
        virtual void
        create(DataModelPtr & dataModel, gint32 row) throw(Error) = 0;
        
        //Will be used by DB engine to construct the name
        //of tables in indices
        virtual Glib::ustring
        get_db_object_type_name() const throw() = 0;

        inline gint32
        get_row() const throw();

        void
        set_row( gint32 row ) throw();
    
        inline const DBTablePtr &
        get_table() const throw();

        void
        set_table( const DBTablePtr &table ) throw();
        
        inline bool
        get_is_deleted() const throw();

        void
        set_is_deleted( bool value ) throw();
};

inline gint32
DBObject::get_row() const throw()
{
    return row_;
}

inline const DBTablePtr &
DBObject::get_table() const throw()
{
    return table_;
}    

inline bool
DBObject::get_is_deleted() const throw()
{
    return isDeleted_;
}

} // namespace Solang

#endif // SOLANG_DB_OBJECT_H
