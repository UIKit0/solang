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

#ifndef SOLANG_I_PHOTO_DESTINATION_H
#define SOLANG_I_PHOTO_DESTINATION_H

#include <glibmm.h>
#include <gtkmm.h>
#include <sigc++/sigc++.h>

#include "non-copyable.h"
#include "types.h"

namespace Solang
{

class Application;

class IPhotoDestination :
    public NonCopyable
{
    public:
        virtual
        ~IPhotoDestination() throw() = 0;

        virtual void
        init(Application & application) throw() = 0;

        virtual void
        final(Application & application) throw() = 0;

        virtual void
        export_photos_async(const PhotoListPtr & photos,
                            const ProgressObserverPtr & observer)
                            throw() = 0;

        virtual void
        final() throw() = 0;

        virtual sigc::signal<void, bool> &
        init_end() throw() = 0;

        virtual void
        read_selection() throw() = 0;

        virtual Gtk::Widget &
        get_browser() throw() = 0;

        virtual Glib::ustring
        get_label() const throw() = 0;

        virtual Glib::ustring
        get_name() const throw() = 0;

        virtual gint
        get_options() const throw() = 0;

        virtual Gtk::StockID
        get_stock_id() const throw() = 0;

        virtual void
        set_create_archive(bool value) throw() = 0;

    protected:
        IPhotoDestination() throw();

    private:
};

} // namespace Solang

#endif // SOLANG_I_PHOTO_DESTINATION_H
