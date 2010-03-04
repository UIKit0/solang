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

#ifndef SOLANG_FINALLY_H
#define SOLANG_FINALLY_H

#include <sigc++/sigc++.h>

#include "non-copyable.h"

namespace Solang
{

class Finally :
    public NonCopyable
{
    public:
        Finally(const sigc::slot<void> & s) throw();

        // Calling the slot might lead to an exception.
        ~Finally();

    private:
        sigc::slot<void> s_;
};

} // namespace Solang

#endif // SOLANG_FINALLY_H
