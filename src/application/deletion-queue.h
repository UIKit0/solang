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

#ifndef SOLANG_DELETION_QUEUE_H
#define SOLANG_DELETION_QUEUE_H

#include <glibmm.h>

#include "delete-action.h"
#include "error.h"
#include "types.h"

namespace Solang
{

class DeletionQueue
{
    public:
        DeletionQueue( Database &db );

        DeletionQueue( const DeletionQueue &);

        ~DeletionQueue() throw();

        DeletionQueue &
        operator = (const DeletionQueue &);

        void
        schedule_delete_action( const DeleteActionPtr &) throw();

        inline Glib::ustring
        get_last_action_name() throw();

        inline size_t
        get_queue_size() const throw();

        void
        execute_actions() throw(Error);

        void
        execute_actions(
                    const ProgressObserverPtr &observer ) throw(Error);

    private:

        void
        execute_action(const DeleteActionPtr &action) throw(Error);

        Database &db_;

        DeleteActionList actions_;

        Glib::Mutex mutex_;
};

inline Glib::ustring
DeletionQueue::get_last_action_name() throw()
{
    DeleteActionList::iterator it = actions_.begin();

    if( it != actions_.end() )
    {
        return (*it)->get_command_name();
    }
}

inline size_t
DeletionQueue::get_queue_size() const throw()
{
    return actions_.size();
}

} //namespace Solang

#endif
