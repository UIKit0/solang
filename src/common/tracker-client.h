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

#ifndef SOLANG_TRACKER_CLIENT_H
#define SOLANG_TRACKER_CLIENT_H

#include <string>
#include <vector>

#include <glibmm.h>
#include <libtracker-client/tracker-client.h>
#include <sigc++/sigc++.h>

#include "types.h"

namespace Solang
{

class TrackerClient
{
    public:
        typedef sigc::slot<void, std::vector<UStringList> &>
            SlotAsyncReply;

        typedef sigc::slot<void> SlotAsyncReady;

        TrackerClient();

        ~TrackerClient();

        std::vector<UStringList>
        sparql_query(const std::string & query) const
                     throw(Glib::Error);

        guint
        sparql_query_async(const std::string & query,
                           const SlotAsyncReply & slot) const throw();

        void
        sparql_update(const std::string & query) const
                      throw(Glib::Error);

        guint
        sparql_update_async(const std::string & query,
                            const SlotAsyncReady & slot) const
                            throw();

    private:
        ::TrackerClient * trackerClient_;
};

} // namespace Solang

#endif // SOLANG_TRACKER_CLIENT_H
