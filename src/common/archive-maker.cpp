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

#include <archive.h>
#include <archive_entry.h>
#include <giomm.h>

#include "archive-maker.h"
#include "i-progress-observer.h"
#include "photo.h"

namespace Solang
{

class Archive
{
    public:
        static ArchivePtr
        create(gint fd) throw();

        static ArchivePtr
        create(const std::string & filename) throw();

        ~Archive() throw();

        archive *
        obj() throw();

    private:
        Archive(gint fd) throw();

        Archive(const std::string & filename) throw();

        archive * a_;
};

Archive::Archive(gint fd) throw() :
    a_(archive_write_new())
{
    archive_write_set_compression_none(a_);
    archive_write_set_format(a_, ARCHIVE_FORMAT_ZIP);
    archive_write_open_fd(a_, fd);
}

Archive::Archive(const std::string & filename) throw() :
    a_(archive_write_new())
{
    archive_write_set_compression_none(a_);
    archive_write_set_format(a_, ARCHIVE_FORMAT_ZIP);
    archive_write_open_filename(a_, filename.c_str());
}

Archive::~Archive() throw()
{
    archive_write_finish(a_);
}

ArchivePtr
Archive::create(gint fd) throw()
{
    return ArchivePtr(new Archive(fd));
}

ArchivePtr
Archive::create(const std::string & filename) throw()
{
    return ArchivePtr(new Archive(filename));
}

archive *
Archive::obj() throw()
{
    return a_;
}

class ArchiveEntry
{
    public:
        static ArchiveEntryPtr
        create() throw();

        ~ArchiveEntry() throw();

        void
        copy_stat(const FileInfoPtr & file_info) throw();

        archive_entry *
        obj() throw();

    private:
        ArchiveEntry() throw();

        archive_entry * ae_;
};

ArchiveEntry::ArchiveEntry() throw() :
    ae_(archive_entry_new())
{
}

ArchiveEntry::~ArchiveEntry() throw()
{
    archive_entry_free(ae_);
}

ArchiveEntryPtr
ArchiveEntry::create() throw()
{
    return ArchiveEntryPtr(new ArchiveEntry());
}

void
ArchiveEntry::copy_stat(const FileInfoPtr & file_info) throw()
{
    {
        const guint64 time = file_info->get_attribute_uint64(
                                 G_FILE_ATTRIBUTE_TIME_ACCESS);
        const guint32 time_m = file_info->get_attribute_uint32(
                                   G_FILE_ATTRIBUTE_TIME_ACCESS_USEC)
                               * 1000;
        archive_entry_set_atime(ae_, time, time_m);
    }

    {
        const guint64 time = file_info->get_attribute_uint64(
                                 G_FILE_ATTRIBUTE_TIME_CHANGED);
        const guint32 time_m = file_info->get_attribute_uint32(
                                   G_FILE_ATTRIBUTE_TIME_CHANGED_USEC)
                               * 1000;
        archive_entry_set_ctime(ae_, time, time_m);
    }

    {
        const guint64 time = file_info->get_attribute_uint64(
                                 G_FILE_ATTRIBUTE_TIME_MODIFIED);
        const guint32 time_m = file_info->get_attribute_uint32(
                                   G_FILE_ATTRIBUTE_TIME_MODIFIED_USEC)
                               * 1000;
        archive_entry_set_mtime(ae_, time, time_m);
    }

    {
        const guint64 time = file_info->get_attribute_uint64(
                                 G_FILE_ATTRIBUTE_TIME_MODIFIED);
        const guint32 time_m = file_info->get_attribute_uint32(
                                   G_FILE_ATTRIBUTE_TIME_MODIFIED_USEC)
                               * 1000;
        archive_entry_set_mtime(ae_, time, time_m);
    }

    archive_entry_unset_birthtime(ae_);

    {
        const guint32 dev = file_info->get_attribute_uint32(
                                G_FILE_ATTRIBUTE_UNIX_DEVICE);
        archive_entry_set_dev(ae_, dev);
    }

    {
        const guint32 gid = file_info->get_attribute_uint32(
                                G_FILE_ATTRIBUTE_UNIX_GID);
        archive_entry_set_gid(ae_, gid);
    }

    {
        const guint32 uid = file_info->get_attribute_uint32(
                                G_FILE_ATTRIBUTE_UNIX_UID);
        archive_entry_set_uid(ae_, uid);
    }

    {
        const guint64 ino = file_info->get_attribute_uint64(
                                G_FILE_ATTRIBUTE_UNIX_INODE);
        archive_entry_set_ino(ae_, ino);
    }

    {
        const guint32 nlink = file_info->get_attribute_uint32(
                                  G_FILE_ATTRIBUTE_UNIX_NLINK);
        archive_entry_set_nlink(ae_, nlink);
    }

    {
        const guint32 rdev = file_info->get_attribute_uint32(
                                 G_FILE_ATTRIBUTE_UNIX_RDEV);
        archive_entry_set_rdev(ae_, rdev);
    }

    {
        const guint64 size = file_info->get_attribute_uint64(
                                 G_FILE_ATTRIBUTE_STANDARD_SIZE);
        archive_entry_set_size(ae_, size);
    }

    {
        const guint32 mode = file_info->get_attribute_uint32(
                                 G_FILE_ATTRIBUTE_UNIX_MODE);
        archive_entry_set_mode(ae_, mode);
    }
}

archive_entry *
ArchiveEntry::obj() throw()
{
    return ae_;
}

const gsize ArchiveMaker::chunk_ = 4096;

const std::string ArchiveMaker::prefix_ = "Photos/";

ArchiveMaker::ArchiveMaker() throw() :
    NonCopyable(),
    sigc::trackable()
{
}

ArchiveMaker::~ArchiveMaker() throw()
{
}

ArchiveMakerPtr
ArchiveMaker::create() throw()
{
    return ArchiveMakerPtr(new ArchiveMaker());
}

void
ArchiveMaker::add_file_async(const Triplet & triplet,
                             const SlotAsyncReady & slot,
                             const ProgressObserverPtr & observer)
                             const throw()
{
    const PhotoPtr & photo = triplet.second->back();
    const FilePtr file = Gio::File::create_for_uri(photo->get_uri());

    file->read_async(sigc::bind(
                         sigc::mem_fun(
                             *this,
                             &ArchiveMaker::on_async_file_read_ready),
                         triplet,
                         file,
                         slot,
                         observer),
                     observer,
                     Glib::PRIORITY_DEFAULT);
}

void
ArchiveMaker::make_async(const std::string & dest_path,
                         const PhotoListPtr & photos,
                         const SlotAsyncReady & slot,
                         const ProgressObserverPtr & observer) const
                         throw()
{
    if (true == photos->empty())
    {
        return;
    }

    const ArchivePtr archive = Archive::create(dest_path);

    Triplet triplet;
    triplet.first = dest_path;
    triplet.second = photos;
    triplet.third = archive;

    add_file_async(triplet, slot, observer);
}

void
ArchiveMaker::on_async_file_read_ready(
                  const AsyncResultPtr & async_result,
                  const Triplet & triplet,
                  const FilePtr & file,
                  const SlotAsyncReady & slot,
                  const ProgressObserverPtr & observer) const throw()
{
    FileInputStreamPtr input_stream;

    try
    {
        input_stream = file->read_finish(async_result);
    }
    catch (const Gio::Error & e)
    {
        switch (e.code())
        {
        case Gio::Error::CANCELLED:
            break;

        default:
            g_warning("%s", e.what().c_str());
            break;
        }
        return;
    }

    input_stream->query_info_async(
        sigc::bind(sigc::mem_fun(
                       *this,
                       &ArchiveMaker::on_async_stream_query_ready),
                   triplet,
                   input_stream,
                   slot,
                   observer),
        observer,
        "*",
        Glib::PRIORITY_DEFAULT);
}

void
ArchiveMaker::on_async_stream_close_ready(
                  const AsyncResultPtr & async_result,
                  const FileInputStreamPtr & input_stream) const
                  throw()
{
    try
    {
        input_stream->close_finish(async_result);
    }
    catch (const Gio::Error & e)
    {
        g_warning("%s", e.what().c_str());
    }
}

void
ArchiveMaker::on_async_stream_query_ready(
                  const AsyncResultPtr & async_result,
                  const Triplet & triplet,
                  const FileInputStreamPtr & input_stream,
                  const SlotAsyncReady & slot,
                  const ProgressObserverPtr & observer) const throw()
{
    FileInfoPtr file_info;

    try
    {
        file_info = input_stream->query_info_finish(async_result);
    }
    catch (const Gio::Error & e)
    {
        switch (e.code())
        {
        case Gio::Error::CANCELLED:
            break;

        default:
            g_warning("%s", e.what().c_str());
            break;
        }
        return;
    }

    const ArchiveEntryPtr & archive_entry = ArchiveEntry::create();
    archive_entry->copy_stat(file_info);

    const PhotoPtr & photo = triplet.second->back();
    const FilePtr file = Gio::File::create_for_uri(photo->get_uri());
    const std::string pathname = prefix_ + file->get_basename();

    archive_entry_set_pathname(archive_entry->obj(),
                               pathname.c_str());

    archive_write_header(triplet.third->obj(), archive_entry->obj());

    read_file_async(triplet,
                    input_stream,
                    archive_entry,
                    slot,
                    observer);
}

void
ArchiveMaker::on_async_stream_read_ready(
                  const AsyncResultPtr & async_result,
                  const CharPtr & buffer,
                  Triplet & triplet,
                  const FileInputStreamPtr & input_stream,
                  ArchiveEntryPtr & archive_entry,
                  const SlotAsyncReady & slot,
                  const ProgressObserverPtr & observer) const throw()
{
    gssize nread;

    try
    {
        nread = input_stream->read_finish(async_result);
    }
    catch (const Gio::Error & e)
    {
        switch (e.code())
        {
        case Gio::Error::CANCELLED:
            break;

        default:
            g_warning("%s", e.what().c_str());
            break;
        }
        return;
    }

    if (0 >= nread)
    {
        archive_entry.reset();
        input_stream->close_async(
            sigc::bind(
                sigc::mem_fun(
                    *this,
                    &ArchiveMaker::on_async_stream_close_ready),
                input_stream),
            Glib::PRIORITY_DEFAULT);

        if (0 != observer)
        {
            observer->progress();
        }

        triplet.second->pop_back();

        if (true == triplet.second->empty())
        {
            triplet.third.reset();
            if (true == slot)
            {
                slot();
            }
        }
        else
        {
            add_file_async(triplet, slot, observer);
        }
    }
    else
    {
        archive_write_data(triplet.third->obj(), &*buffer, nread);
        read_file_async(triplet,
                        input_stream,
                        archive_entry,
                        slot,
                        observer);
    }
}

void
ArchiveMaker::read_file_async(
                  const Triplet & triplet,
                  const FileInputStreamPtr & input_stream,
                  const ArchiveEntryPtr & archive_entry,
                  const SlotAsyncReady & slot,
                  const ProgressObserverPtr & observer) const throw()
{
    const CharPtr buffer(new char[chunk_]);

    input_stream->read_async(
        &*buffer,
        chunk_,
        sigc::bind(sigc::mem_fun(
                       *this,
                       &ArchiveMaker::on_async_stream_read_ready),
                   buffer,
                   triplet,
                   input_stream,
                   archive_entry,
                   slot,
                   observer),
        observer,
        Glib::PRIORITY_DEFAULT);
}

} // namespace Solang
