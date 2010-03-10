/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * Copyright (C) 2009, 2010 Debarshi Ray <rishi@gnu.org>
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

#include <utility>

#include <glibmm.h>

#include "photo.h"
#include "thumbnailer.h"

namespace Solang
{

Thumbnailer::Thumbnailer() throw() :
    map_(),
    pendingList_(),
    thumbnailerProxy_(),
    ready_()
{
    thumbnailerProxy_.error_connect(
        sigc::mem_fun(*this,
                      &Thumbnailer::on_signal_error));
    thumbnailerProxy_.ready_connect(
        sigc::mem_fun(*this,
                      &Thumbnailer::on_signal_ready));
    thumbnailerProxy_.started_connect(
        sigc::mem_fun(*this,
                      &Thumbnailer::on_signal_started));
}

Thumbnailer::~Thumbnailer() throw()
{
}

void
Thumbnailer::on_async_queue(guint handle, const PhotoList & photos)
                            throw()
{
    std::map<Glib::ustring, PhotoPtr> m;

    for (PhotoList::const_iterator iter = photos.begin();
         photos.end() != iter;
         iter++)
    {
        const PhotoPtr photo = *iter;
        m.insert(std::make_pair(photo->get_uri(), photo));
    }

    map_.insert(std::make_pair(handle, m));
}

void
Thumbnailer::on_signal_error(guint handle,
                             UStringList & uris,
                             guint code,
                             Glib::ustring & message) throw()
{
}

void
Thumbnailer::on_signal_ready(guint handle, UStringList & uris)
                             throw()
{
    if (map_.end() == map_.find(handle))
    {
        return;
    }

    std::map<Glib::ustring, PhotoPtr> & m = map_[handle];
    PhotoList photos;

    for (UStringList::const_iterator iter = uris.begin();
         uris.end() != iter;
         iter++)
    {
        const Glib::ustring & uri = *iter;

        if (m.end() == m.find(uri))
        {
            continue;
        }

        const PhotoPtr photo = m[uri];
        photo->set_thumbnail_state(Photo::THUMBNAIL_STATE_READY);
        photos.push_back(photo);
    }

    ready_.emit(photos);
}

void
Thumbnailer::on_signal_started(guint handle) throw()
{
}

void
Thumbnailer::process() throw()
{
    UStringList mime_types;
    UStringList uris;

    for (PhotoList::const_iterator iter = pendingList_.begin();
         pendingList_.end() != iter;
         iter++)
    {
        mime_types.push_back((*iter)->get_content_type());
        uris.push_back((*iter)->get_uri());
    }

    thumbnailerProxy_.queue_async(
        uris,
        mime_types,
        "normal",
        "forward",
        0,
        sigc::bind(sigc::mem_fun(*this,
                                 &Thumbnailer::on_async_queue),
                   pendingList_));
    pendingList_.clear();
}

void
Thumbnailer::push(const PhotoPtr & photo) throw()
{
    if (true == pendingList_.empty())
    {
        Glib::signal_timeout().connect_once(
            sigc::mem_fun(*this, &Thumbnailer::process),
            100,
            Glib::PRIORITY_DEFAULT);
    }

    pendingList_.push_back(photo);
}

sigc::signal<void, PhotoList &> &
Thumbnailer::signal_ready() throw()
{
    return ready_;
}

} // namespace Solang
