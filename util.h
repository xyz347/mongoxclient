#ifndef __MONGO_X_CLIENT_UTIL_H
#define __MONGO_X_CLIENT_UTIL_H

#include <bsoncxx/document/view.hpp>
#include <x2struct/bson_builder.h>
#include <x2struct/x2struct.hpp>


namespace mongoxc {

class Util {
public:
    static bsoncxx::document::view VpToView(const bb::vp& v, std::string&container) {
        container = bb::build(v, 0);
        return bsoncxx::document::view((const uint8_t*)container.data(), container.length());
    }
    static bsoncxx::document::view VpToView(const bb::vp& v, std::vector<std::string>& gc) {
        gc.resize(gc.size()+1);
        std::string& container = gc[gc.size()-1];
        container = bb::build(v, 0);
        return bsoncxx::document::view((const uint8_t*)container.data(), container.length());
    }

    template <typename DATA>
    static int ViewToInterface(const bsoncxx::document::view_or_value &v, DATA &result) {
        try {
            x2struct::X::loadbson(v.view().data(), 0, result);
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
                x2struct::X::loadbson((*iter).data(), 0, data);
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

