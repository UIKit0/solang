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

#ifndef SOLANG_I_OPERATION_H
#define SOLANG_I_OPERATION_H

#include <glibmm.h>
#include <sigc++/sigc++.h>

#include "non-copyable.h"
#include "types.h"

namespace Gtk
{

class Widget;

} // namespace Gtk

namespace Solang
{

class IOperation :
    private NonCopyable,
    public sigc::trackable
{
    public:
        typedef sigc::signal<void> SignalReady;

        virtual
        ~IOperation() throw() = 0;

        virtual BufferPtr
        apply(const BufferPtr & buffer,
              const ProgressObserverPtr & observer)
              throw(Glib::Thread::Exit) = 0;

        virtual Glib::ustring
        get_description() const throw() = 0;

        virtual Gtk::Widget *
        get_widget() throw() = 0;

        virtual SignalReady &
        signal_ready() throw() = 0;

    protected:
        IOperation() throw();

        virtual NodePtr
        get_node(const BufferPtr & buffer, const NodePtr & root) const
                 throw() = 0;

    private:
};

} // namespace Solang

#endif // SOLANG_I_OPERATION_H
