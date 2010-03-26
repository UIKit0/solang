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

#include <algorithm>
#include <iterator>

#include <gtkmm.h>

#include "application.h"
#include "export-queue-operations.h"

namespace Solang
{

class ExportMarker :
    public std::unary_function<const PhotoPtr &, PhotoPtr>
{
    public:
        ExportMarker(bool mark) throw();

        ExportMarker(const ExportMarker & source) throw();

        ~ExportMarker() throw();

        ExportMarker &
        operator=(const ExportMarker & source) throw();

        PhotoPtr
        operator()(const PhotoPtr & photo) throw();

    protected:
        bool mark_;
};

ExportMarker::ExportMarker(bool mark) throw() :
    std::unary_function<const PhotoPtr &, PhotoPtr>(),
    mark_(mark)
{
}

ExportMarker::ExportMarker(const ExportMarker & source) throw() :
    std::unary_function<const PhotoPtr &, PhotoPtr>(source),
    mark_(source.mark_)
{
}

ExportMarker::~ExportMarker() throw()
{
}

ExportMarker &
ExportMarker::operator=(const ExportMarker & source) throw()
{
    if (this != &source)
    {
        std::unary_function<const PhotoPtr &, PhotoPtr>::operator=(
                                                             source);
        mark_ = source.mark_;
    }

    return *this;
}

PhotoPtr
ExportMarker::operator()(const PhotoPtr & photo) throw()
{
    photo->set_state_export_queue(mark_);
    return photo;
}

class TreeModelRowChangedEmitter :
    public std::unary_function<const Gtk::TreeModel::Path &, void>
{
    public:
        TreeModelRowChangedEmitter(const TreeModelPtr & tree_model)
                                   throw();

        TreeModelRowChangedEmitter(
            const TreeModelRowChangedEmitter & source) throw();

        ~TreeModelRowChangedEmitter() throw();

        TreeModelRowChangedEmitter &
        operator=(const TreeModelRowChangedEmitter & source) throw();

        void
        operator()(const Gtk::TreeModel::Path & path) throw();

    protected:
        TreeModelPtr treeModel_;
};

TreeModelRowChangedEmitter::TreeModelRowChangedEmitter(
                                const TreeModelPtr & tree_model)
                                throw() :
    std::unary_function<const Gtk::TreeModel::Path &, void>(),
    treeModel_(tree_model)
{
}

TreeModelRowChangedEmitter::TreeModelRowChangedEmitter(
    const TreeModelRowChangedEmitter & source) throw() :
    std::unary_function<const Gtk::TreeModel::Path &, void>(source),
    treeModel_(source.treeModel_)
{
}

TreeModelRowChangedEmitter::~TreeModelRowChangedEmitter() throw()
{
}

TreeModelRowChangedEmitter &
TreeModelRowChangedEmitter::operator=(
    const TreeModelRowChangedEmitter & source) throw()
{
    if (this != &source)
    {
        std::unary_function<const Gtk::TreePath &, void>::operator=(
                                                              source);
        treeModel_ = source.treeModel_;
    }

    return *this;
}

void
TreeModelRowChangedEmitter::operator()(const Gtk::TreePath & path)
                                       throw()
{
    treeModel_->row_changed(path, treeModel_->get_iter(path));
}

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

    std::for_each(queue.begin(), queue.end(), ExportMarker(false));
    if (false == queue.empty())
    {
        const TreeModelPtr & tree_model
                                 = application_->get_list_store();
        TreeModelRowChangedEmitter emitter(tree_model);

        tree_model->foreach_path(
            sigc::bind_return(
                sigc::mem_fun(
                    emitter,
                    &TreeModelRowChangedEmitter::operator()),
                false));
    }
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
    std::transform(photos.begin(), photos.end(),
                   std::inserter(queue, queue.begin()),
                   ExportMarker(true));

    const TreeModelPtr & tree_model = application_->get_list_store();
    const TreePathList & paths = renderer->get_selected_paths();
    std::for_each(paths.begin(), paths.end(),
                  TreeModelRowChangedEmitter(tree_model));
}

} // namespace Solang
