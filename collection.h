#ifndef __MONGO_X_CLIENT_COLLECTION_H
#define __MONGO_X_CLIENT_COLLECTION_H

#include <memory>
#include <mongocxx/collection.hpp>
#include <x2struct/bson_builder.h>

#include "query.h"
#include "util.h"

namespace mongoxc {

class Collection {
public:
    typedef mongocxx::stdx::optional<mongocxx::result::update> update_result;
    typedef mongocxx::stdx::optional<mongocxx::result::delete_result> delete_result;

    Collection(mongocxx::collection &col):col_(col) {
    }
public:
    // query
    Query& Find(const bb::vp&query) {
        query_.reset(new Query(col_, query));
        return *query_;
    }
    Query& FindId(const std::string& id) {
        return Find({{"_id",id}});
    }
    Query& FindId(int64_t id) {
        return Find({{"_id",id}});
    }

    // update one
    update_result Update(const bb::vp&selector, const bb::vp&update) {
        return Update(selector, update, false);
    }
    template <typename IDTYPE>
    update_result Update(const IDTYPE& id, const bb::vp&update) {
        return Update(id, update, false);
    }
    template <typename DATA>
    update_result Update(const bb::vp&selector, const DATA&update) {
        std::string s;
        std::string u = x2struct::BsonWriter().convert("$set", update).toStr();

        auto vs = Util::VpToView(selector, s);
        auto vu = bsoncxx::document::view((const uint8_t*)u.data(), u.length());
        return col_.update_one(vs, vu);
    }
    template <typename IDTYPE, typename DATA>
    update_result Update(const IDTYPE& id, const DATA&update) {
        return Update({{"_id",id}}, update);
    }
    
    // upsert
    update_result Upsert(const bb::vp&selector, const bb::vp&update) {
        return Update(selector, update, true);
    }
    template <typename IDTYPE>
    update_result Upsert(const IDTYPE& id, const bb::vp&update) {
        return Update(id, update, true);
    }
    template <typename DATA>
    update_result Upsert(const bb::vp&selector, const DATA&update) {
        mongocxx::options::update options;
        options.upsert(true);

        std::string s;
        std::string u = x2struct::BsonWriter().convert("$set", update).toStr();

        auto vs = Util::VpToView(selector, s);
        auto vu = bsoncxx::document::view((const uint8_t*)u.data(), u.length());
        return col_.update_one(vs, vu, options);
    }
    template <typename IDTYPE, typename DATA>
    update_result Upsert(const IDTYPE& id, const DATA&update) {
        return Upsert({{"_id",id}}, update);
    }

    // update all hit
    update_result UpdateAll(const bb::vp&selector, const bb::vp&update) {
        std::string s;
        std::string u;

        auto vs = Util::VpToView(selector, s);
        auto vu = Util::VpToView(update, u);

        return col_.update_many(vs, vu);
	}

    // delete one
    delete_result Remove(const bb::vp&selector) {
        std::string s;
        auto vs = Util::VpToView(selector, s);
        return col_.delete_one(vs);
    }
    template <typename IDTYPE>
    delete_result Remove(const IDTYPE& id) {
        return Remove({{"_id", id}});
    }

    // delete many
    delete_result RemoveAll(const bb::vp&selector) {
        std::string s;
        return col_.delete_many(Util::VpToView(selector, s));
    }

    // count
    int Count() {
        return col_.count(bsoncxx::document::view());
    }
private:
    // update one
    update_result Update(const bb::vp&selector, const bb::vp&update, bool upsert) {
        mongocxx::options::update options;
        options.upsert(upsert);

        std::string s;
        std::string u;

        auto vs = Util::VpToView(selector, s);
        auto vu = Util::VpToView(update, u);

        return col_.update_one(vs, vu, options);
    }
    template <typename IDTYPE>
    update_result Update(const IDTYPE& id, const bb::vp&update, bool upsert) {
        return Update({{"_id",id}}, update, upsert);
    }
private:
    mongocxx::collection &col_;
    std::shared_ptr<Query> query_;
};



}


#endif

