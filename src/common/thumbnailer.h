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

#ifndef SOLANG_THUMBNAILER_H
#define SOLANG_THUMBNAILER_H

#include <map>

#include <glibmm.h>
#include <sigc++/sigc++.h>

#include "singleton.h"
#include "thumbnailer-proxy.h"
#include "types.h"

namespace Solang
{

class Thumbnailer :
    public Singleton<Thumbnailer>,
    public sigc::trackable
{
    public:
        ~Thumbnailer() throw();

        void
        push(const PhotoPtr & photo) throw();

        sigc::signal<void, PhotoList &> &
        signal_ready() throw();

    private:
        friend class Singleton<Thumbnailer>;

        Thumbnailer() throw();

        void
        on_async_queue(guint handle, const PhotoList & photos)
                       throw();

        void
        on_signal_error(guint handle,
                        UStringList & uris,
                        guint code,
                        Glib::ustring & message) throw();

        void
        on_signal_ready(guint handle, UStringList & uris)
                        throw();

        void
        on_signal_started(guint handle) throw();

        void
        process(void) throw();

        std::map<guint, std::map<Glib::ustring, PhotoPtr> > map_;

        PhotoList pendingList_;

        ThumbnailerProxy thumbnailerProxy_;

        sigc::signal<void, PhotoList &> ready_;
};

} // namespace Solang

#endif // SOLANG_THUMBNAILER_H
