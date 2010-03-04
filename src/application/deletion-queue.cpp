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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "deletion-queue.h"
#include "i-progress-observer.h"

namespace Solang
{

DeletionQueue::DeletionQueue(Database &db)
    :db_(db),
    actions_(),
    mutex_()
{
}

DeletionQueue::DeletionQueue( const DeletionQueue &rhs )
    :db_(rhs.db_),
    actions_( rhs.actions_ ),
    mutex_()
{
}

DeletionQueue::~DeletionQueue() throw()
{
    actions_.clear();
}

DeletionQueue &
DeletionQueue::operator = ( const DeletionQueue &rhs )
{
    if( this != &rhs )
    {
        actions_ = rhs.actions_;
    }

    return *this;
}

void
DeletionQueue::schedule_delete_action(
                const DeleteActionPtr &action ) throw()
{
    Glib::Mutex::Lock lock( mutex_ );
    actions_.push_back( action );
    return;
}

void
DeletionQueue::execute_actions() throw(Error)
{
    for( DeleteActionList::iterator action = actions_.begin();
            action != actions_.end(); action++ )
    {
        execute_action( *action );
    }
}

void
DeletionQueue::execute_actions(
                const ProgressObserverPtr &observer) throw(Error)
{
    if( ! observer )
    {
        try
        {
            execute_actions();
        }
        catch( Error &e )
        {
            e.add_call_info( __FUNCTION__, __FILE__, __LINE__ );
            throw;
        }
    }
    observer->set_description( "Executing delete actions" );
    observer->set_total( actions_.size() );
    for( DeleteActionList::iterator action = actions_.begin();
                                action != actions_.end(); action++ )
    {
        try
        {
            execute_action( *action );
        }
        catch( Error &e )
        {
            e.add_call_info( __FUNCTION__, __FILE__, __LINE__ );
            observer->reset();
            throw;
        }
        observer->progress();
    }
}

void
DeletionQueue::execute_action(
                    const DeleteActionPtr &action ) throw(Error)
{
    action->execute( db_ );
}

} //namespace Solang
