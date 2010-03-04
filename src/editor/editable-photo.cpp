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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <algorithm>

#include <geglmm.h>
#include <geglmm/buffer.h>

#include "buffer-maker.h"
#include "buffer-pixbuf-converter.h"
#include "editable-photo.h"
#include "finally.h"
#include "i-operation.h"
#include "i-progress-observer.h"
#include "photo.h"

namespace Solang
{

EditablePhoto::EditablePhoto(const PhotoPtr & photo) throw() :
    buffer_(0),
    photo_(photo),
    pixbuf_(0),
    pending_(),
    applyEnd_(),
    threadPool_(1, false)
{
    applyEnd_.connect(sigc::mem_fun(*this,
                                    &EditablePhoto::on_apply_end));
}

EditablePhoto::~EditablePhoto() throw()
{
    if (true == pending_.empty())
    {
        return;
    }

    const Triplet & current = pending_.front();
    const ProgressObserverPtr & observer = current.third;

    if (0 != observer)
    {
        observer->cancel();
    }
}

void
EditablePhoto::apply_async(const IOperationPtr & operation,
                           const SlotAsyncReady & slot,
                           const ProgressObserverPtr & observer)
                           throw()
{
    const SlotAsyncReadyPtr slot_copy(new SlotAsyncReady(slot));

    Triplet triplet;
    triplet.first = operation;
    triplet.second = slot_copy;
    triplet.third = observer;

    pending_.push(triplet);
    if (1 < pending_.size())
    {
        return;
    }

    apply_begin();
}


void
EditablePhoto::apply_begin() throw()
{
    const Triplet & current = pending_.front();

    threadPool_.push(sigc::bind(
                         sigc::mem_fun(*this,
                                       &EditablePhoto::apply_worker),
                         current.first,
                         current.third));
}

void
EditablePhoto::apply_worker(const IOperationPtr & operation,
                            const ProgressObserverPtr & observer)
                            throw(Glib::Thread::Exit)
{
    const Finally finally(sigc::mem_fun(applyEnd_,
                                        &Glib::Dispatcher::emit));

    if (0 == buffer_)
    {
        std::string path;
        try
        {
            path = Glib::filename_from_uri(photo_->get_uri());
        }
        catch (const Glib::ConvertError & e)
        {
            g_warning("%s", e.what().c_str());
            throw Glib::Thread::Exit();
        }

        BufferMaker buffer_maker;
        buffer_ = buffer_maker(path);
    }

    buffer_ = operation->apply(buffer_, observer);
g_warning("done op");
    BufferPixbufConverter buffer_pixbuf_converter;
    pixbuf_ = buffer_pixbuf_converter(buffer_);
g_warning("done pixbuf");
}

void
EditablePhoto::on_apply_end() throw()
{
    const Triplet current = pending_.front();

    pending_.pop();
    photo_->set_buffer(pixbuf_);

    if (0 != current.second && 0 != *current.second)
    {
        (*current.second)();
    }

    const ProgressObserverPtr & observer = current.third;

    // Clear the pending queue if:
    // + an operation was cancelled
    // + the buffer could not be created
    if (true == observer->is_cancelled() || 0 == buffer_)
    {
        PendingOperationQueue tmp;
        std::swap(pending_, tmp);
    }

    if (true == pending_.empty())
    {
        return;
    }

    apply_begin();
}

PhotoPtr &
EditablePhoto::get_photo() throw()
{
    return photo_;
}

} // namespace Solang
