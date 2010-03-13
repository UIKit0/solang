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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <iostream>
#include <utility>

#include "photo.h"
#include "photo-factory.h"

namespace Solang
{

PhotoFactory::PhotoFactory() throw() :
    photos_()
{
}

PhotoFactory::~PhotoFactory() throw()
{
}

PhotoPtr
PhotoFactory::create_photo(const Glib::ustring & uri,
                           const Glib::ustring & content_type) throw()
{
    const PhotoMap::iterator iter = photos_.find(uri);

    if (photos_.end() == iter)
    {
        const PhotoPtr photo(new Photo(uri, content_type),
                             &PhotoFactory::deleter);
        const WeakPhotoPtr weak_photo(photo);

        photos_.insert(std::make_pair(uri, weak_photo));
        return photo;
    }

    const WeakPhotoPtr weak_photo = iter->second;
    const PhotoPtr photo = weak_photo.lock();
    photo->set_content_type(content_type);

    return photo;
}

void
PhotoFactory::deleter(Photo * photo) throw()
{
    const Glib::ustring & uri = photo->get_uri();
    PhotoFactory & photo_factory = PhotoFactory::instance();
    PhotoMap & photos = photo_factory.get_photos();
    PhotoMap::iterator iter = photos.find(uri);

    if (photos.end() == iter)
    {
        std::cerr << G_STRLOC << ", " << G_STRFUNC << ": "
                  << uri << ": Missing WeakPhotoPtr" << std::endl;
    }
    else
    {
        photos.erase(iter);
    }

    delete photo;
}

PhotoFactory::PhotoMap &
PhotoFactory::get_photos() throw()
{
    return photos_;
}

} // namespace Solang
