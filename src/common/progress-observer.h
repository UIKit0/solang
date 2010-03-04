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

#ifndef SOLANG_PROGRESS_OBSERVER_H
#define SOLANG_PROGRESS_OBSERVER_H

#include <glibmm/i18n.h>
#include <sigc++/sigc++.h>

#include "i-progress-observer.h"
#include "progress-dialog.h"
#include "progress-widget.h"
#include "types.h"
#include "utils.h"

namespace Solang
{

template <typename T>
class ProgressObserver :
    public IProgressObserver
{
    public:
        static ProgressObserverPtr
        create(ProgressDialog & progress_dialog);

        virtual
        ~ProgressObserver() throw();

        virtual bool
        is_finished() const throw();

        virtual void
        progress() throw();

        virtual void
        set_description(const Glib::ustring & description) throw();

        virtual void
        set_fraction(gdouble fraction) throw();

        virtual void
        set_total(guint64 total) throw();

    protected:
        ProgressObserver(ProgressDialog & progress_dialog) throw();

        virtual void
        on_cancelled();

    private:
        void
        clean_up() throw();

        void
        on_fraction_changed() throw();

        T fractionChanged_;

        mutable Glib::Mutex mutex_;

        gdouble fraction_;

        guint64 current_;

        guint64 total_;

        ProgressDialog & progressDialog_;

        ProgressWidget progressWidget_;

        sigc::connection connectionFractionChanged_;
};

template <typename T>
ProgressObserver<T>::ProgressObserver(ProgressDialog & progress_dialog)
                                      throw() :
    IProgressObserver(),
    fractionChanged_(),
    mutex_(),
    fraction_(0.0),
    current_(0),
    total_(0),
    progressDialog_(progress_dialog),
    progressWidget_(),
    connectionFractionChanged_()
{
    fractionChanged_.connect(
        sigc::mem_fun(*this,
                      &ProgressObserver::on_fraction_changed));
    progressWidget_.signal_cancelled().connect(
        sigc::mem_fun(*this,
                      &ProgressObserver::cancel));
}

template <typename T>
ProgressObserver<T>::~ProgressObserver() throw()
{
    clean_up();
}

template <typename T>
void
ProgressObserver<T>::clean_up() throw()
{
    if (0 != progressWidget_.get_parent())
    {
        progressDialog_.detach(progressWidget_);
    }
}

template <typename T>
ProgressObserverPtr
ProgressObserver<T>::create(ProgressDialog & progress_dialog)
{
    return ProgressObserverPtr(new ProgressObserver<T>(
                                       progress_dialog));
}

template <typename T>
bool
ProgressObserver<T>::is_finished() const throw()
{
    Glib::Mutex::Lock lock(mutex_);
    return (0 < total_) ? (current_ == total_)
                        : is_equal(1.0, fraction_);
}

template <typename T>
void
ProgressObserver<T>::on_cancelled()
{
    clean_up();
    connectionFractionChanged_.block();
}

template <typename T>
void
ProgressObserver<T>::on_fraction_changed() throw()
{
    {
        Glib::Mutex::Lock lock(mutex_);

        progressWidget_.set_fraction(fraction_);

        if (0 < total_)
        {
            // Translators: Progress bar text. eg., 13 of 42 completed.
            const Glib::ustring details = Glib::ustring::compose(
                                              _("%1 of %2 completed"),
                                              current_,
                                              total_);
            progressWidget_.set_details(details);
        }
        else
        {
            // Translators: Progress bar text. eg., 13% completed.
            const Glib::ustring details = Glib::ustring::compose(
                                              _("%1%% complete"),
                                              fraction_ * 100.0);
            progressWidget_.set_details(details);
        }
    }

    if (0 == progressWidget_.get_parent())
    {
        progressDialog_.attach(progressWidget_);
    }
}

template <typename T>
void
ProgressObserver<T>::progress() throw()
{
    if (true == is_cancelled())
    {
        return;
    }

    {
        Glib::Mutex::Lock lock(mutex_);
        current_++;
        fraction_ = static_cast<gdouble>(current_)
                    / static_cast<gdouble>(total_);
    }
    fractionChanged_.emit();
}

template <typename T>
void
ProgressObserver<T>::set_description(const Glib::ustring & description)
                                     throw()
{
    progressWidget_.set_status(description);
}

template <typename T>
void
ProgressObserver<T>::set_fraction(gdouble fraction) throw()
{
    if (true == is_cancelled())
    {
        return;
    }

    {
        Glib::Mutex::Lock lock(mutex_);
        fraction_ = fraction;
    }
    fractionChanged_.emit();
}

template <typename T>
void
ProgressObserver<T>::set_total(guint64 total) throw()
{
    Glib::Mutex::Lock lock(mutex_);
    total_ = total;
}

} // namespace Solang

#endif // SOLANG_PROGRESS_OBSERVER_H
