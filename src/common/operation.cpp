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

extern "C"
{
#include <babl/babl.h>
}

#include <geglmm.h>
#include <geglmm/buffer.h>
#include <geglmm/processor.h>

#include "finally.h"
#include "i-progress-observer.h"
#include "operation.h"

namespace Solang
{

Operation::Operation() throw() :
    IOperation()
{
}

Operation::~Operation() throw()
{
}

BufferPtr
Operation::apply(const BufferPtr & buffer,
                 const ProgressObserverPtr & observer)
                 throw(Glib::Thread::Exit)
{
    if (0 != observer)
    {
        observer->set_description(get_description());
    }

    const NodePtr root = Gegl::Node::create();
    root->set("format", babl_format("RGB u8"));

    const NodePtr buffer_source = root->new_child(
                                      "operation",
                                      "gegl:buffer-source");
    gegl_node_set(buffer_source->gobj(),
                  "buffer", buffer->gobj(),
                  NULL);

    const NodePtr operation = get_node(buffer, root);

    const NodePtr buffer_sink = root->new_child("operation",
                                                "gegl:buffer-sink");
    GeglBuffer * output_buffer = 0;
    gegl_node_set(buffer_sink->gobj(),
                  "buffer", &output_buffer,
                  NULL);

    buffer_source->link(operation)->link(buffer_sink);

    gdouble progress;
    GeglProcessor * const processor = gegl_node_new_processor(
                                          buffer_sink->gobj(), 0);
    const Finally finally(sigc::bind(sigc::ptr_fun(&g_object_unref),
                                     processor));

    while (true == gegl_processor_work(processor, &progress))
    {
        if (0 != observer)
        {
            observer->set_fraction(progress);
            if (true == observer->is_cancelled())
            {
                // FIXME: Find a better way to do this.
                if (0 != output_buffer)
                {
                    g_object_unref(output_buffer);
                }
                throw Glib::Thread::Exit();
            }
        }
    }

    return Glib::wrap(output_buffer, false);
}

} // namespace Solang
