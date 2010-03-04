/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
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

#ifndef SOLANG_NON_COPYABLE_H
#define SOLANG_NON_COPYABLE_H

namespace Solang
{

class NonCopyable
{
public:
    virtual
    ~NonCopyable();

protected:
    NonCopyable();
		
private:
    // Blocked.
    NonCopyable(const NonCopyable & source);

    // Blocked.
    NonCopyable &
    operator=(const NonCopyable & source);
};

} // namespace Solang

#endif // SOLANG_NON_COPYABLE_H
