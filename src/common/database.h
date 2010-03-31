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

#ifndef SOLANG_DATABASE_H
#define SOLANG_DATABASE_H

#include <set>
#include <tr1/memory>

#include <sigc++/sigc++.h>

#include "i-photo-search-criteria.h"
#include "tracker-client.h"
#include "types.h"

namespace Solang
{

class Photo;
class PhotoTag;
class Tag;

class Database
{
    private:
        TrackerClient trackerClient_;

    public:
        typedef sigc::slot<void, ExifData &> SlotAsyncExifData;

        typedef sigc::slot<void, PhotoList &> SlotAsyncPhotos;

        typedef sigc::slot<void> SlotAsyncReady;

        typedef sigc::slot<void, TagList &> SlotAsyncTags;

        Database();

        ~Database();

        void
        delete_async(const PhotoTag & photo_tag,
                     const SlotAsyncReady & slot) throw();

        void
        delete_async(const Tag & tag, const SlotAsyncReady & slot)
                     throw();

        void
        edit_async(Tag & tag,
                   const Glib::ustring & name,
                   const Glib::ustring & description,
                   const SlotAsyncReady & slot) throw();

        void
        get_exif_data_async(const Photo & photo,
                            const SlotAsyncExifData & slot) const
                            throw();

        void
        save_async(const Photo & photo, const SlotAsyncReady & slot)
                   throw();

        void
        save_async(const PhotoTag & photo_tag,
                   const SlotAsyncReady & slot) throw();

        void
        save_async(const Tag & tag, const SlotAsyncReady & slot)
                   throw();

        void
        search_async(const IPhotoSearchCriteriaList & criteria,
                     const SlotAsyncPhotos & slot) const throw();

        //Group by year
        DatePhotoInfoList
        get_dates_with_picture_count( );

        //Group by year, month
        DatePhotoInfoList
        get_dates_with_picture_count( gint year );

        //Group by year, month, day
        DatePhotoInfoList
        get_dates_with_picture_count(gint year, gint month);

        void
        get_tags_async(bool all, const SlotAsyncTags & slot) const
                       throw();

    private:
        DatePhotoInfoList
        get_dates_with_picture_count(const Glib::ustring & sql);

        void
        on_async_exif_data(std::vector<UStringList> & result,
                           const SlotAsyncExifData & slot) const
                           throw();

        void
        on_async_photos(std::vector<UStringList> & result,
                        const SlotAsyncPhotos & slot) const throw();

        void
        on_async_ready(const SlotAsyncReady & slot) const throw();

        void
        on_async_tags(std::vector<UStringList> & result,
                      const SlotAsyncTags & slot) const throw();
};

} // namespace Solang

#endif // SOLANG_DATABASE_H
