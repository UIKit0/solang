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

#ifndef SOLANG_ARCHIVE_MAKER_H
#define SOLANG_ARCHIVE_MAKER_H

#include <string>

#include <sigc++/sigc++.h>

#include "non-copyable.h"
#include "types.h"

namespace Solang
{

class Archive;
typedef std::tr1::shared_ptr<const Archive> ConstArchivePtr;
typedef std::tr1::shared_ptr<Archive> ArchivePtr;

class ArchiveEntry;
typedef std::tr1::shared_ptr<const ArchiveEntry> ConstArchiveEntryPtr;
typedef std::tr1::shared_ptr<ArchiveEntry> ArchiveEntryPtr;

class ArchiveMaker :
    private NonCopyable,
    public sigc::trackable
{
    public:
        typedef sigc::slot<void> SlotAsyncReady;

        static ArchiveMakerPtr
        create() throw();

        ~ArchiveMaker() throw();

        void
        make_async(const std::string & dest_path,
                   const PhotoListPtr & photos,
                   const SlotAsyncReady & slot,
                   const ProgressObserverPtr & observer) const
                   throw();

    protected:

    private:
        struct Triplet
        {
            std::string first;
            PhotoListPtr second;
            ArchivePtr third;
        };

        static const gsize chunk_;

        static const std::string prefix_;

        ArchiveMaker() throw();

        void
        add_file_async(const Triplet & triplet,
                       const SlotAsyncReady & slot,
                       const ProgressObserverPtr & observer) const
                       throw();

        void
        on_async_file_read_ready(
            const AsyncResultPtr & async_result,
            const Triplet & triplet,
            const FilePtr & file,
            const SlotAsyncReady & slot,
            const ProgressObserverPtr & observer) const throw();

        void
        on_async_stream_close_ready(
            const AsyncResultPtr & async_result,
            const FileInputStreamPtr & input_stream) const throw();

        void
        on_async_stream_query_ready(
            const AsyncResultPtr & async_result,
            const Triplet & triplet,
            const FileInputStreamPtr & input_stream,
            const SlotAsyncReady & slot,
            const ProgressObserverPtr & observer) const throw();

        void
        on_async_stream_read_ready(
            const AsyncResultPtr & async_result,
            const CharPtr & buffer,
            Triplet & triplet,
            const FileInputStreamPtr & input_stream,
            ArchiveEntryPtr & archive_entry,
            const SlotAsyncReady & slot,
            const ProgressObserverPtr & observer) const throw();

        void
        read_file_async(
            const Triplet & triplet,
            const FileInputStreamPtr & input_stream,
            const ArchiveEntryPtr & archive_entry,
            const SlotAsyncReady & slot,
            const ProgressObserverPtr & observer) const throw();
};

} // namespace Solang

#endif // SOLANG_ARCHIVE_MAKER_H
