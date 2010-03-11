/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * Copyright (C) 2009 Debarshi Ray <rishi@gnu.org>
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
#include <iterator>

#include "application.h"
#include "export-queue-operations.h"

namespace Solang
{

ExportQueueCleaner::ExportQueueCleaner(Application & application)
                                       throw() :
    std::unary_function<void, void>(),
    application_(&application)
{
}

ExportQueueCleaner::ExportQueueCleaner(
                        const ExportQueueCleaner & source) throw() :
    std::unary_function<void, void>(source),
    application_(source.application_)
{
}

ExportQueueCleaner::~ExportQueueCleaner() throw()
{
}

ExportQueueCleaner &
ExportQueueCleaner::operator=(const ExportQueueCleaner & source) throw()
{
    if (this != &source)
    {
        std::unary_function<void, void>::operator=(source);
        application_ = source.application_;
    }

    return *this;
}

void
ExportQueueCleaner::operator()(void) throw()
{
    Engine & engine = application_->get_engine();
    PhotoSet & queue = engine.get_export_queue();
    queue.clear();
}

ExportQueueInserter::ExportQueueInserter(Application & application)
                                         throw() :
    std::unary_function<void, void>(),
    application_(&application)
{
}

ExportQueueInserter::ExportQueueInserter(
                         const ExportQueueInserter & source) throw() :
    std::unary_function<void, void>(source),
    application_(source.application_)
{
}

ExportQueueInserter::~ExportQueueInserter() throw()
{
}

ExportQueueInserter &
ExportQueueInserter::operator=(const ExportQueueInserter & source)
                               throw()
{
    if (this != &source)
    {
        std::unary_function<void, void>::operator=(source);
        application_ = source.application_;
    }

    return *this;
}

void
ExportQueueInserter::operator()(void) throw()
{
    Engine & engine = application_->get_engine();
    PhotoSet & queue = engine.get_export_queue();
    RendererRegistry & renderer_registry
                           = application_->get_renderer_registry();
    const IRendererPtr renderer = renderer_registry.get_current();
    const PhotoList & photos = renderer->get_current_selection();

    std::copy(photos.begin(), photos.end(),
              std::inserter(queue, queue.begin()));
}

} // namespace Solang
