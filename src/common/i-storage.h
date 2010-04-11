/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * i-storage.h
 * Copyright (C) Debarshi Ray 2010 <rishi@gnu.org>
 * Copyright (C) Santanu Sinha 2009 <santanu.sinha@gmail.com>
 * 
 * i-storage.h is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * i-storage.h is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SOLANG_I_STORAGE_H
#define SOLANG_I_STORAGE_H

#include <glibmm.h>

#include "error.h"
#include "non-copyable.h"
#include "photo.h"
#include "types.h"

namespace Solang
{

class IStorage :
    private NonCopyable
{
    public:
        virtual
        ~IStorage() throw() = 0;

        virtual
        void init(Application & application) throw(Error) = 0;

        virtual void
        save(const PhotoPtr & photo) = 0;
        virtual void
        save(const PhotoPtr & photo, bool move) = 0;


        // The following should return the path of the file in the local
        // disk. If required, it will download from the storage.
        // For raw files, the thumbnail will be extracted and path for
        // the same will be returned.
        virtual Glib::ustring
        retrieve(const Photo & photo) throw(Error) = 0;

        virtual void
        downloadToDisk(const PhotoList & photos,
                       ProgressObserverPtr & observer) throw(Error) = 0;

        virtual void
        remove(const PhotoPtr & photo) throw(Error) = 0;

        virtual void
        remove(const PhotoList & photos) throw(Error) = 0;

        virtual std::string
        get_storage_uri_prefix() throw() = 0;

        virtual void
        final(Application & application) throw(Error) = 0;

    protected:
        IStorage() throw();

    private:
};

} // namespace Solang

#endif // SOLANG_STORAGE_H
