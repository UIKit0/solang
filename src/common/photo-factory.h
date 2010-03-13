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

#ifndef SOLANG_PHOTO_FACTORY_H
#define SOLANG_PHOTO_FACTORY_H

#include <map>
#include <tr1/memory>

#include <glibmm.h>

#include "singleton.h"
#include "types.h"

namespace Solang
{

class Photo;

class PhotoFactory :
    public Singleton<PhotoFactory>
{
    public:
        ~PhotoFactory() throw();

        PhotoPtr
        create_photo(const Glib::ustring & uri,
                     const Glib::ustring & content_type) throw();

    private:
        typedef std::tr1::weak_ptr<Photo> WeakPhotoPtr;

        typedef std::map<Glib::ustring, WeakPhotoPtr> PhotoMap;

        friend class Singleton<PhotoFactory>;

        static void
        deleter(Photo * photo) throw();

        PhotoMap &
        get_photos() throw();

        PhotoFactory() throw();

        PhotoMap photos_;
};

} // namespace Solang

#endif // SOLANG_PHOTO_FACTORY_H
