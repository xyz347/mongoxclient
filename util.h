#ifndef __MONGO_X_CLIENT_UTIL_H
#define __MONGO_X_CLIENT_UTIL_H

#include <bsoncxx/document/view.hpp>
#include <xbson/bson.h>


namespace mongoxc {

class Util {
public:
    template<typename T>
    static std::string kv(const char *key, const T&val) {
        xpack::BsonEncoder en;
        en.ob(NULL);
        en.add(key, val);
        en.oe(NULL);
        return en.String();
    }
    
    template <typename DATA>
    static int ViewToInterface(const bsoncxx::document::view_or_value &v, DATA &result) {
        try {
            xpack::bson::decode(v.view().data(), 0, result);
            return 0;
        } catch (...) {
            return -1;
        }
    }

    template <typename DATA>
    static int CursorToVector(mongocxx::cursor& cursor, std::vector<DATA> &result) {
       for (auto iter=cursor.begin(); iter!=cursor.end(); ++iter) {
            DATA data;
            try {
                xpack::bson::decode((*iter).data(), 0, data);
                result.push_back(data);
            } catch (...) {
                continue;
            }
        }
        return 0; 
    }
};



}


#endif

