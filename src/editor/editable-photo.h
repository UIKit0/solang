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

#ifndef SOLANG_EDITABLE_PHOTO_H
#define SOLANG_EDITABLE_PHOTO_H

#include <queue>
#include <utility>
#include <tr1/memory>

#include <glibmm.h>
#include <sigc++/sigc++.h>

#include "types.h"

namespace Solang
{

class EditablePhoto
{
    public:
        typedef sigc::slot<void> SlotAsyncReady;

        typedef std::tr1::shared_ptr<SlotAsyncReady>
            SlotAsyncReadyPtr;

        EditablePhoto(const PhotoPtr & photo) throw();

        ~EditablePhoto() throw();

        void
        apply_async(const IOperationPtr & operation,
                    const SlotAsyncReady & slot,
                    const ProgressObserverPtr & observer) throw();

        PhotoPtr &
        get_photo() throw();

    private:
        struct Triplet
        {
            IOperationPtr first;
            SlotAsyncReadyPtr second;
            ProgressObserverPtr third;
        };

        typedef std::queue<Triplet> PendingOperationQueue;

        void
        apply_begin() throw();

        void
        apply_worker(const IOperationPtr & operation,
                     const ProgressObserverPtr & observer)
                     throw(Glib::Thread::Exit);

        void
        on_apply_end() throw();

        BufferPtr buffer_;

        PhotoPtr photo_;

        PixbufPtr pixbuf_;

        PendingOperationQueue pending_;

        Glib::Dispatcher applyEnd_;

        Glib::ThreadPool threadPool_;
};

} // namespace Solang

#endif // SOLANG_EDITABLE_PHOTO_H
