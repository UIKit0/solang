/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * Copyright (C) 2010 Debarshi Ray <rishi@gnu.org>
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

#ifndef SOLANG_CONTENT_TYPE_REPO_H
#define SOLANG_CONTENT_TYPE_REPO_H

#include <vector>

#include <gdkmm.h>

#include "non-copyable.h"
#include "types.h"

namespace Solang
{

class ContentTypeRepo
        : private NonCopyable
{
    private:
        typedef Gdk::Pixbuf::SListHandle_PixbufFormat
                                    PixbufFormatList;
        typedef std::vector<Glib::ustring>
                                SupportedContentTypeList;

        SupportedContentTypeList supportedTypes_;

        //This is a singleton
        ContentTypeRepo() throw();

    public:

        ~ContentTypeRepo() throw();

        void
        init() throw();

        Glib::ustring
        get_content_type(const std::string & filename) const throw();

        bool
        is_gdk_supported(const Glib::ustring & content_type) const
                         throw();

        //Static
        static const ContentTypeRepoPtr
        instance() throw();
};

} // namespace Solang

#endif // SOLANG_CONTENT_TYPE_REPO_H
