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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "database.h"
#include "exif-data.h"
#include "photo.h"
#include "i-storage.h"

namespace Solang
{

void
Photo::parse_exif_data(const UStringList & data, ExifData & exif_data)
                       throw()
{
    exif_data.set_camera(data[0]);
    exif_data.set_exposure_time(data[1]);
    exif_data.set_flash(data[2]);
    exif_data.set_fnumber(data[3]);
    exif_data.set_focal_length(data[4]);
    exif_data.set_iso_speed(data[5]);
    exif_data.set_metering_mode(data[6]);
    exif_data.set_white_balance(data[7]);
}

Photo::Photo(const Glib::ustring & uri,
             const Glib::ustring & content_type) throw() :
    DBObject(),
    uri_(uri),
    contentType_(content_type),
    state_(PHOTO_STATE_NONE),
    thumbnailPath_(),
    thumbnailState_(THUMBNAIL_STATE_NONE),
    buffer_( 0 ),
    thumbnailBuffer_( 0 ),
    hasUnsavedData_( false )
{
    const std::string md5_checksum = Glib::Checksum::compute_checksum(
                                         Glib::Checksum::CHECKSUM_MD5,
                                         uri);
    thumbnailPath_ = Glib::get_home_dir()
                     + G_DIR_SEPARATOR_S + ".thumbnails"
                     + G_DIR_SEPARATOR_S + "normal"
                     + G_DIR_SEPARATOR_S + md5_checksum + ".png";
}

Photo::~Photo() throw()
{
}

void
Photo::delete_async(Database & database, const SlotAsyncReady & slot)
                    const throw()
{
}

Glib::ustring
Photo::get_delete_query() const throw()
{
    return Glib::ustring();
}

Glib::ustring
Photo::get_save_query() const throw()
{
    return Glib::ustring();
}

bool
Photo::get_state_export_queue() const throw()
{
    return state_ & PHOTO_STATE_EXPORT_QUEUE;
}

void
Photo::set_state_export_queue(bool export_queue) throw()
{
    state_ = (true == export_queue)
             ? state_ | PHOTO_STATE_EXPORT_QUEUE
             : state_ & ~PHOTO_STATE_EXPORT_QUEUE;
}

void
Photo::save_async(Database & database, const SlotAsyncReady & slot)
                  const throw()
{
    database.save_async(*this, slot);
}

const std::string &
Photo::get_thumbnail_path() const throw()
{
    return thumbnailPath_;
}

Glib::ustring
Photo::get_exif_data_query() const throw()
{
    return Glib::ustring::compose(
        "SELECT nmm:camera(?photo)"
        "       nmm:exposureTime(?photo)"
        "       nmm:flash(?photo)"
        "       nmm:fnumber(?photo)"
        "       nmm:focalLength(?photo)"
        "       nmm:isoSpeed(?photo)"
        "       nmm:meteringMode(?photo)"
        "       nmm:whiteBalance(?photo) "
        "WHERE {"
        "  ?photo a nmm:Photo ;"
        "  nie:url '%1' ."
        "}",
        uri_);
}

void
Photo::get_exif_data_async(const Database & database,
                           const SlotAsyncExifData & slot) const
                           throw()
{
    database.get_exif_data_async(*this, slot);
}

void
Photo::set_uri(const Glib::ustring & uri)
{
    uri_ = uri;
}

void
Photo::set_content_type(const Glib::ustring & contentType) throw()
{
    contentType_ = contentType;
}

DeleteActionPtr
Photo::get_delete_action() throw()
{
    DeleteActionPtr action(
                        new DeleteAction( "Photo", this ));
    return action;
}

Photo::ThumbnailState
Photo::get_thumbnail_state() const throw()
{
    return thumbnailState_;
}

void
Photo::set_buffer( const PixbufPtr &buffer ) throw()
{
    buffer_ = buffer;
}

void
Photo::set_thumbnail_buffer( const PixbufPtr &buffer ) throw()
{
    thumbnailBuffer_ = buffer;
}

void
Photo::set_thumbnail_state(Photo::ThumbnailState thumbnail_state)
                           throw()
{
    thumbnailState_ = thumbnail_state;
}

void
Photo::set_has_unsaved_data( bool value ) throw()
{
    hasUnsavedData_ = value;
}

} // namespace Solang
