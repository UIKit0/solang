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

#ifndef SOLANG_I_PROGRESS_OBSERVER_H
#define SOLANG_I_PROGRESS_OBSERVER_H

#include <giomm.h>
#include <glibmm.h>

namespace Solang
{

class IProgressObserver :
    public Gio::Cancellable
{
    public:
        virtual
        ~IProgressObserver() throw() = 0;

        virtual bool
        is_finished() const throw() = 0;

        virtual void
        progress() throw() = 0;

        virtual void
        set_description(const Glib::ustring & description)
                        throw() = 0;

        virtual void
        set_fraction(gdouble fraction) throw() = 0;

        virtual void
        set_total(guint64 total) throw() = 0;

    protected:
        IProgressObserver() throw();

    private:
};

} // namespace Solang

#endif // SOLANG_I_PROGRESS_OBSERVER_H
