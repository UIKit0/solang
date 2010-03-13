/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * Copyright (C) 2009, 2010 Debarshi Ray <rishi@gnu.org>
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

#include <iostream>
#include <sstream>
#include <vector>

#include <glibmm/i18n.h>

#include "database.h"
#include "exif-data.h"
#include "photo.h"
#include "photo-factory.h"
#include "photo-tag.h"
#include "tag.h"
#include "date-photo-info.h"

namespace Solang
{

class Comparator
{
    public:
        bool operator()  ( const IPhotoSearchCriteriaPtr &lhs,
                            const IPhotoSearchCriteriaPtr &rhs )
        {
            return lhs->get_id() < rhs->get_id();
        }
};

Database::Database() :
    trackerClient_()
{
}

//Database::Database( const Database &rhs )
//    : path_(rhs.path_)
//{
//}

//Database &
//Database::operator =( const Database &rhs )
//{
//    if( this != &rhs )
//    {
//        path_ = rhs.path_;
//    }
//
//    return *this;
//}

Database::~Database()
{
}

void
Database::get_exif_data_async(const Photo & photo,
                              const SlotAsyncExifData & slot) const
                              throw()
{
    trackerClient_.sparql_query_async(
        photo.get_exif_data_query(),
        sigc::bind(sigc::mem_fun(*this,
                                 &Database::on_async_exif_data),
                   slot));
}

void
Database::delete_async(const PhotoTag & photo_tag,
                       const SlotAsyncReady & slot) throw()
{
    trackerClient_.sparql_update_async(
        photo_tag.get_delete_query(),
        sigc::bind(sigc::mem_fun(*this, &Database::on_async_ready),
                   slot));
}

void
Database::delete_async(const Tag & tag, const SlotAsyncReady & slot)
                       throw()
{
    trackerClient_.sparql_update_async(
        tag.get_delete_query(),
        sigc::bind(sigc::mem_fun(*this, &Database::on_async_ready),
                   slot));
}

void
Database::edit_async(Tag & tag,
                     const Glib::ustring & name,
                     const Glib::ustring & description,
                     const SlotAsyncReady & slot) throw()
{
    trackerClient_.sparql_update_async(
        tag.get_edit_query(name, description),
        sigc::bind(sigc::mem_fun(*this, &Database::on_async_ready),
                   slot));
    tag.set_name(name);
    tag.set_description(description);
}

void
Database::save_async(const Photo & photo, const SlotAsyncReady & slot)
                     throw()
{
}

void
Database::save_async(const PhotoTag & photo_tag,
                     const SlotAsyncReady & slot) throw()
{
    trackerClient_.sparql_update_async(
        photo_tag.get_save_query(),
        sigc::bind(sigc::mem_fun(*this, &Database::on_async_ready),
                   slot));
}

void
Database::save_async(const Tag & tag, const SlotAsyncReady & slot)
                     throw()
{
    trackerClient_.sparql_update_async(
        tag.get_save_query(),
        sigc::bind(sigc::mem_fun(*this, &Database::on_async_ready),
                   slot));
}

void
Database::search_async(const IPhotoSearchCriteriaList & criteria,
                       const SlotAsyncPhotos & slot) const throw()
{
    Glib::ustring clauses = "";

    for (IPhotoSearchCriteriaList::const_iterator it
             = criteria.begin();
         criteria.end() != it;
         it++)
    {
        clauses += Glib::ustring::compose(
                                      "{?photo %1}",
                                      (*it)->get_query_criteria());
    }

    const Glib::ustring query
        = Glib::ustring::compose(
              "SELECT nie:url(?photo) nie:mimeType(?photo) "
              "WHERE {"
              "  ?photo a nmm:Photo ."
              "  %1"
              "}",
              clauses);

    trackerClient_.sparql_query_async(
        query.c_str(),
        sigc::bind(sigc::mem_fun(*this,
                                 &Database::on_async_photos),
                   slot));
}

//Group by year
DatePhotoInfoList
Database::get_dates_with_picture_count()
{
    Glib::ustring sql
        = "select 0, 0, mod_year, count(*) from photos ";
    sql += "group by mod_year";
    return get_dates_with_picture_count( sql );
}

//Group by year, month
DatePhotoInfoList
Database::get_dates_with_picture_count(gint year)
{
    std::ostringstream sout;
    sout<<"select 0, mod_month, mod_year, count(*) from photos ";
    sout<<"where mod_year="<<year<<" ";
    sout<<"group by mod_year,mod_month ";
//    sout<<"order by mod_year desc, mod_month desc";
    return get_dates_with_picture_count( sout.str() );
}

//Group by year, month, day
DatePhotoInfoList
Database::get_dates_with_picture_count(gint year, gint month)
{
    std::ostringstream sout;
    sout<<"select mod_day, mod_month, mod_year, count(*) from photos ";
    sout<<"where mod_year="<<year<<" ";
    sout<<"and mod_month="<<month<<" ";
    sout<<"group by mod_year,mod_month,mod_day ";
//    sout<<"order by mod_year desc, mod_month desc, mod_day desc";
    return get_dates_with_picture_count( sout.str() );
}

DatePhotoInfoList
Database::get_dates_with_picture_count(const Glib::ustring & sql)
{
    DatePhotoInfoList infos;

/*    try
    {
        const DataModelPtr model
            = gdaConnection_->statement_execute_select(
                  sql, Gnome::Gda::STATEMENT_MODEL_RANDOM_ACCESS);

        const gint32 numRows
                         = static_cast<gint32>(model->get_n_rows());

        for( gint32 row = 0; row < numRows; row++ )
        {
            infos.push_back(
                DatePhotoInfo(
                    ModificationDate(
                        model->get_value_at( 0, row ).get_int(),
                        model->get_value_at( 1, row ).get_int(),
                        model->get_value_at( 2, row ).get_int()),
                    model->get_value_at( 3, row ).get_int()));
        }
    }
    catch (Glib::Error &e)
    {
        std::cerr << __FILE__ << ":" << __LINE__ << ", "
                  << __FUNCTION__ << ": " << e.what()
                  << std::endl;
    }
*/
    return infos;
}

void
Database::get_tags_async(const SlotAsyncTags & slot) const throw()
{
    trackerClient_.sparql_query_async(
        "SELECT DISTINCT ?t ?d ?u "
        "WHERE {"
        "  ?u a nao:Tag ;"
        "  nao:prefLabel ?t ."
        "  OPTIONAL {"
        "      ?u a nie:InformationElement ;"
        "      nie:comment ?d ."
        "  }"
        "} "
        "ORDER BY ASC(?t)",
        sigc::bind(sigc::mem_fun(*this, &Database::on_async_tags),
                   slot));
}

void
Database::on_async_exif_data(std::vector<UStringList> & result,
                             const SlotAsyncExifData & slot) const
                             throw()
{
    if (1 != result.size())
    {
        std::cerr << G_STRLOC << ", " << G_STRFUNC << ": "
                  << "Unexpected result" << std::endl;
    }

    ExifData exif_data;

    if (false == result.empty())
    {
        const UStringList & data = result.front();
        Photo::parse_exif_data(data, exif_data);
    }

    if (0 != slot)
    {
        slot(exif_data);
    }
}

void
Database::on_async_photos(std::vector<UStringList> & result,
                          const SlotAsyncPhotos & slot) const throw()
{
    PhotoFactory & photo_factory = PhotoFactory::instance();
    PhotoList photos;

    for (std::vector<UStringList>::const_iterator it = result.begin();
         result.end() != it;
         it++)
    {
        PhotoPtr photo = photo_factory.create_photo((*it)[0],
                                                    (*it)[1]);
        photos.push_back(photo);
    }

    if (0 != slot)
    {
        slot(photos);
    }
}

void
Database::on_async_ready(const SlotAsyncReady & slot) const throw()
{
    if (0 != slot)
    {
        slot();
    }
}

void
Database::on_async_tags(std::vector<UStringList> & result,
                        const SlotAsyncTags & slot) const throw()
{
    TagList tags;
    for (std::vector<UStringList>::const_iterator it = result.begin();
         result.end() != it;
         it++)
    {
        TagPtr tag(new Tag((*it)[0], (*it)[1], (*it)[2]));
        tags.push_back(tag);
    }

    if (0 != slot)
    {
        slot(tags);
    }
}

} // namespace Solang
