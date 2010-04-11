/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * i-photo-source.h
 * Copyright (C) Debarshi Ray 2009, 2010 <rishi@gnu.org>
 * 
 * photo-source.h is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * photo-source.h is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SOLANG_I_PHOTO_SOURCE_H
#define SOLANG_I_PHOTO_SOURCE_H

#include <glibmm.h>
#include <gtkmm.h>
#include <sigc++/sigc++.h>

#include "non-copyable.h"
#include "types.h"

namespace Solang
{

class Database;
class Tag;

class IPhotoSource :
    private NonCopyable
{
    public:
        virtual
        ~IPhotoSource() throw() = 0;

        virtual void
        init(Application & application) throw() = 0;

        virtual void
        final(Application & application) throw() = 0;

        virtual PhotoPtr
        import(const PhotoPtr & photo, const IStoragePtr & storage,
               const TagList & tag, Database & db,
               const ProgressObserverPtr & observer) throw() = 0;

        virtual PhotoList
        import(const PhotoList & photos, const IStoragePtr & storage,
               const TagList & tag, Database & db,
               const ProgressObserverPtr & observer) throw() = 0;

        virtual PhotoList
        import(const IStoragePtr & storage, const TagList & tag,
               Database & db, const ProgressObserverPtr & observer)
               throw() = 0;

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

    protected:
        IPhotoSource() throw();

    private:
};

} // namespace Solang

#endif // SOLANG_I_PHOTO_SOURCE_H
