#ifndef __MONGO_X_CLIENT_QUERY_H
#define __MONGO_X_CLIENT_QUERY_H

#include <mongocxx/collection.hpp>
#include <xbson/bson.h>

#include "util.h"

#define MONGXC_Q_STRING_TO_VIEW(s, v) gc_.push_back(s); auto v=bsoncxx::document::view((const uint8_t*)gc_[gc_.size()-1].data(), s.length()) 

namespace mongoxc {

class Query {
    friend class Collection;
public:
    template <typename DATA>
    int One(DATA &result) {
        auto ret_obj = col_.find_one(qv_, find_options_);
        if (!ret_obj) {
            return -1; // TODO
        }
        return Util::ViewToInterface(ret_obj->view(), result);
    }
    template <typename DATA>
    int All(std::vector<DATA> &result) {
        auto ret_cursor = col_.find(qv_, find_options_);
        return Util::CursorToVector(ret_cursor, result);
    }

    int Count() {
        return col_.count(qv_);
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
    Query& Projection(const std::string& select) {
        MONGXC_Q_STRING_TO_VIEW(select, v);
        find_options_.projection(v);
        return *this;
    }
    Query& Sort(const std::string& fields) {
        MONGXC_Q_STRING_TO_VIEW(fields, v);
        find_options_.sort(v);
        return *this;
    }
private:
    Query(mongocxx::collection col, const std::string& query):col_(col),query_(query) {
        qv_ = bsoncxx::document::view((const uint8_t*)query_.data(), query_.length());
    }
    mongocxx::collection col_;
    mongocxx::options::find find_options_;
    std::string query_;
    bsoncxx::document::view qv_;

    std::vector<std::string> gc_;
};



}


#endif

