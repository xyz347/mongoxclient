#ifndef __MONGO_X_CLIENT_QUERY_H
#define __MONGO_X_CLIENT_QUERY_H

#include <mongocxx/collection.hpp>
#include <x2struct/bson_builder.h>


#include "util.h"

namespace mongoxc {

class Query {
    friend class Collection;
public:
    template <typename DATA>
    int One(DATA &result) {
        auto ret_obj = col_.find_one(Util::VpToView(query_, gc_), find_options_);
        if (!ret_obj) {
            return -1; // TODO
        }
        return Util::ViewToInterface(ret_obj->view(), result);
    }
    template <typename DATA>
    int All(std::vector<DATA> &result) {
        auto ret_cursor = col_.find(Util::VpToView(query_, gc_), find_options_);
        return Util::CursorToVector(ret_cursor, result);
    }

    int Count() {
        return col_.count(Util::VpToView(query_, gc_));
    }
public:
    Query& Skip(int s) {
        find_options_.skip(s);
        return *this;
    }
    Query& Limit(int l) {
        find_options_.limit(l);
        return *this;
    }
    Query& Slave() {
        mongocxx::read_preference read_preference;
        read_preference.mode(mongocxx::read_preference::read_mode::k_secondary_preferred);
        find_options_.read_preference(read_preference);
        return *this;
    }
    Query& Projection(const bb::vp& select) {
        find_options_.projection(Util::VpToView(select, gc_));
        return *this;
    }
    Query& Sort(const bb::vp& fields) {
        find_options_.sort(Util::VpToView(fields, gc_));
        return *this;
    }
private:
    Query(mongocxx::collection col, const bb::vp&query):col_(col),query_(query) {
    }
    mongocxx::collection col_;
    mongocxx::options::find find_options_;
    bb::vp query_;

    std::vector<std::string> gc_;
};



}


#endif

