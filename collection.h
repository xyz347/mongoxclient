#ifndef __MONGO_X_CLIENT_COLLECTION_H
#define __MONGO_X_CLIENT_COLLECTION_H

#include <memory>
#include <mongocxx/collection.hpp>
#include <xbson/bson.h>

#include "query.h"
#include "util.h"


#define MONGXC_STRING_TO_VIEW(s)  bsoncxx::document::view((const uint8_t*)s.data(), s.length())

namespace mongoxc {

class Collection {
public:
    typedef mongocxx::stdx::optional<mongocxx::result::update> update_result;
    typedef mongocxx::stdx::optional<mongocxx::result::delete_result> delete_result;
    typedef mongocxx::stdx::optional<mongocxx::result::insert_one> insert_result;

    Collection(mongocxx::collection &col):col_(col) {
    }
public:
    // query
    Query& Find(const std::string&query) {
        query_.reset(new Query(col_, query));
        return *query_;
    }
    
    template<typename T>
    Query& FindId(const T& id) {
        return Find(Util::kv("_id", id));
    }

    // update one
    update_result Update(const std::string& selector, const std::string& update) {
        return Update(selector, update, false);
    }

    template <typename T>
    update_result Update(const std::string& selector, const T& data) {
        return Update(selector, Util::kv("$set", data), false);
    }

    template <typename T>
    update_result UpdateId(const T& id, const std::string& data) {
        return Update(Util::kv("_id", id), data, false);
    }

    template <typename ID, typename DATA>
    update_result UpdateId(const ID& id, const DATA& data) {
        return Update(Util::kv("_id", id), Util::kv("$set", data), false);
    }

    // upsert
    update_result Upsert(const std::string& selector, const std::string& data) {
        return Update(selector, data, true);
    }
    template <typename T>
    update_result Upsert(const std::string& selector, const T& data) {
        return Update(selector, Util::kv("$set", data), true);
    }

    template <typename T>
    update_result UpsertId(const T& id, const std::string& data) {
        return Update(Util::kv("_id",id), data, true);
    }
    template <typename ID, typename DATA>
    update_result UpsertId(const ID& id, const DATA& data) {
        return Update(Util::kv("_id", id), Util::kv("$set", data), true);
    }

    // update all hit
    update_result UpdateAll(const std::string& selector, const std::string& data) {
        auto vs = MONGXC_STRING_TO_VIEW(selector);
        auto vu = MONGXC_STRING_TO_VIEW(data);

        return col_.update_many(vs, vu);
	}

    // insert
    insert_result Insert(const std::string &data) {
        auto vs = MONGXC_STRING_TO_VIEW(data);

        return col_.insert_one(vs);
    }
    template <typename DATA>
    insert_result Insert(const DATA &data) {
        std::string s = xpack::bson::encode(data);
        auto vs = MONGXC_STRING_TO_VIEW(s);

        return col_.insert_one(vs);
    }

    // delete one
    delete_result Remove(const std::string& selector) {
        auto vs = MONGXC_STRING_TO_VIEW(selector);
        return col_.delete_one(vs);
    }
    template <typename T>
    delete_result RemoveId(const T& id) {
        return Remove(Util::kv("_id", id));
    }

    // delete many
    delete_result RemoveAll(const std::string& selector) {
        return col_.delete_many(MONGXC_STRING_TO_VIEW(selector));
    }

    // count
    int Count() {
        return col_.count(bsoncxx::document::view());
    }
private:
    // update one
    update_result Update(const std::string& selector, const std::string& data, bool upsert) {
        mongocxx::options::update options;
        options.upsert(upsert);

        auto vs = MONGXC_STRING_TO_VIEW(selector);
        auto vu = MONGXC_STRING_TO_VIEW(data);

        return col_.update_one(vs, vu, options);
    }
private:
    mongocxx::collection &col_;
    std::shared_ptr<Query> query_;
};



}


#endif

