/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * Copyright (C) 2009 Debarshi Ray <rishi@gnu.org>
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

#ifndef SOLANG_DIRECTORY_DESTINATION_H
#define SOLANG_DIRECTORY_DESTINATION_H

#include <glibmm.h>

#include "photo-destination.h"

namespace Solang
{

class DirectoryDestination :
    public PhotoDestination
{
    public:
        DirectoryDestination() throw();

        virtual
        ~DirectoryDestination() throw();

        virtual void
        init(Application & application) throw();

        virtual void
        final(Application & application) throw();

        virtual void
        export_photos_async(const PhotoSet & photos,
                            const ProgressObserverPtr & observer)
                            throw();

        virtual void
        final() throw();

        virtual sigc::signal<void, bool> &
        init_end() throw();

        virtual void
        read_selection() throw();

        virtual Gtk::Widget &
        get_browser() throw();

        virtual Glib::ustring
        get_label() const throw();

        virtual Glib::ustring
        get_name() const throw();

        virtual gint
        get_options() const throw();

        virtual Gtk::StockID
        get_stock_id() const throw();

        virtual void
        set_create_archive(bool value) throw();

    protected:
        void
        export_photo_async(const PhotoPtr & photo,
                           const PhotoListPtr & pending,
                           const ProgressObserverPtr & observer)
                           throw();

        void
        on_async_copy_ready(const AsyncResultPtr & async_result,
                            const FilePtr & file,
                            const PhotoListPtr & pending,
                            const ProgressObserverPtr & observer)
                            throw();

        void
        on_child_watch(GPid, int, const std::string & filename)
                       throw();

        bool createArchive_;

        std::string filename_;

        Gtk::FileChooserButton * fileChooserButton_;

        sigc::signal<void, bool> initEnd_;

    private:
};

} // namespace Solang

#endif // SOLANG_DIRECTORY_DESTINATION_H
