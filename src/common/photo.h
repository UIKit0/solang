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

#ifndef SOLANG_PHOTO_H
#define SOLANG_PHOTO_H

#include <string>
#include <tr1/memory>

#include <gdkmm.h>
#include <glibmm.h>

#include "db-object.h"
#include "types.h"

namespace Solang
{

class ExifData;
class Storage;

class Photo :
    public DBObject
{
    public:
        enum ThumbnailState
        {
            THUMBNAIL_STATE_MASK    = 0x03,
            THUMBNAIL_STATE_UNKNOWN = 0x00,
            THUMBNAIL_STATE_NONE    = 0x01,
            THUMBNAIL_STATE_READY   = 0x02,
            THUMBNAIL_STATE_LOADING = 0x03,
        };

        typedef sigc::slot<void, ExifData &> SlotAsyncExifData;

        static void
        parse_exif_data(const UStringList & data,
                        ExifData & exif_data) throw();

        virtual
        ~Photo() throw();

        virtual void
        delete_async(Database & database, const SlotAsyncReady & slot)
                     const throw();

        virtual Glib::ustring
        get_delete_query() const throw();

        virtual Glib::ustring
        get_save_query() const throw();

        virtual void
        save_async(Database & database, const SlotAsyncReady & slot)
                   const throw();

        void
        get_exif_data_async(const Database & database,
                            const SlotAsyncExifData & slot) const
                            throw();

        Glib::ustring
        get_exif_data_query() const throw();

        inline const Glib::ustring &
        get_uri() const throw();

        void
        set_uri( const Glib::ustring &uri );

        inline const Glib::ustring &
        get_content_type() const throw();

        void
        set_content_type( const Glib::ustring &contentType ) throw();

        const std::string &
        get_thumbnail_path() const throw();

        inline const PixbufPtr &
        get_buffer() const throw();

        void
        set_buffer( const PixbufPtr &buffer ) throw();

        bool
        get_state_export_queue() const throw();

        void
        set_state_export_queue(bool export_queue) throw();

        inline const PixbufPtr &
        get_thumbnail_buffer( ) const throw();

        void
        set_thumbnail_buffer( const PixbufPtr &buffer ) throw();

        ThumbnailState
        get_thumbnail_state() const throw();

        void
        set_thumbnail_state(ThumbnailState thumbnail_state) throw();

        inline bool
        get_has_unsaved_data() const throw();

        void
        set_has_unsaved_data( bool value ) throw();

        virtual DeleteActionPtr
        get_delete_action() throw();

    private:
        friend class PhotoFactory;

        // States may not be mutually exclusive.
        enum State
        {
            PHOTO_STATE_NONE            = 0,
            PHOTO_STATE_EXPORT_QUEUE    = 1 << 0
        };

        Photo(const Glib::ustring & uri,
              const Glib::ustring & content_type) throw();

        Glib::ustring uri_;

        Glib::ustring contentType_; //content type

        guint8 state_;

        std::string thumbnailPath_;

        ThumbnailState thumbnailState_;

        //Thumbnail
        PixbufPtr buffer_;
        PixbufPtr thumbnailBuffer_;
        bool hasUnsavedData_;
};

inline const Glib::ustring &
Photo::get_uri() const throw()
{
    return uri_;
}

inline const Glib::ustring &
Photo::get_content_type() const throw()
{
    return contentType_;
}

inline const PixbufPtr &
Photo::get_buffer() const throw()
{
    return buffer_;
}

inline const PixbufPtr &
Photo::get_thumbnail_buffer() const throw()
{
    return thumbnailBuffer_;
}

inline bool
Photo::get_has_unsaved_data() const throw()
{
    return hasUnsavedData_;
}

} // namespace Solang

#endif // SOLANG_PHOTO_H
