#include <iostream>

#ifdef BLADE_TEST
#include <gtest/gtest.h>
#endif

#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>

#include "mongoxclient.hpp"
#include "xpack/json.h"

mongocxx::instance instance{};

using namespace std;
using bb = xpack::BsonBuilder;

struct User {
    int uid;
    string name;
    int age;
    vector<string> tags;
    vector<vector<int>> test;
    XPACK(A(uid, "bson:_id"), O(name, age, tags, test));
};


#ifndef BLADE_TEST
int main(int argc, char *argv[])
#else
TEST(mongoxclient, test)
#endif
{
    mongocxx::uri uri("mongodb://test:test@127.0.0.1:27018/test");
    mongocxx::client client(uri);
    mongocxx::collection collect = client["test"]["test"];

    mongoxc::Collection col(collect);

    std::string empty = bb::En("{}");
   
    col.RemoveAll(empty);

    User u1;
    u1.uid = 123;
    u1.name = "Hello";
    u1.age = 20;
    u1.tags.push_back("study");
    u1.tags.push_back("play game");
    u1.test.resize(2);
    u1.test[0].push_back(1);
    u1.test[0].push_back(2);
    u1.test[0].push_back(3);
    u1.test[1].push_back(4);
    u1.test[1].push_back(5);
    col.UpsertId(u1.uid, u1);   
    
    u1.uid = 456;
    u1.name = "Good";
    u1.age = 22;
    u1.tags[0] = "windows";
    u1.tags[1] = "linux";
    u1.test.clear();
    u1.test.resize(2);
    u1.test[0].push_back(10);
    u1.test[0].push_back(20);
    u1.test[0].push_back(30);
    u1.test[1].push_back(40);
    u1.test[1].push_back(50);
    col.UpsertId(u1.uid, u1);   

    u1.uid = 789;
    u1.name = "insert";
    col.Insert(u1);

    col.Insert(bb::En("{'_id':1111, 'name':'insert_bb'}"));

    cout<<"======count=="<<endl;
    cout<<col.Count()<<endl;
    cout<<col.FindId(123).Count()<<endl;
    
    cout<<"======get====="<<endl;
    User get;
    User getid;
    vector<User> all;
    col.FindId(123).One(get);
    cout<<xpack::json::encode(get)<<endl;
    col.FindId(456).One(getid);
    cout<<xpack::json::encode(getid)<<endl;
    col.Find(empty).All(all);
    for (size_t i=0; i<all.size(); i++) {
        cout<<xpack::json::encode(all[i])<<endl;
    }

    cout<<"===skip==="<<endl;
    all.clear();
    col.Find(empty).Skip(1).All(all);
    for (size_t i=0; i<all.size(); i++) {
        cout<<xpack::json::encode(all[i])<<endl;
    }

    cout<<"===sort==="<<endl;
    all.clear();
    col.Find(empty).Sort(bb::En("{'_id':-1}")).All(all);
    for (size_t i=0; i<all.size(); i++) {
        cout<<xpack::json::encode(all[i])<<endl;
    }

    cout<<"===projection==="<<endl;
    all.clear();
    col.Find(empty).Projection(bb::En("{'name':1}")).All(all);
    for (size_t i=0; i<all.size(); i++) {
        cout<<xpack::json::encode(all[i])<<endl;
    }

    cout<<"===update all==="<<endl;
    col.UpdateAll(empty, bb::En("{'$set':{'name':'haha'}}"));
    all.clear();
    col.Find(empty).All(all);
    for (size_t i=0; i<all.size(); i++) {
        cout<<xpack::json::encode(all[i])<<endl;
    }

    cout<<"=====remove====="<<endl;
    col.Remove(bb::En("{'_id':{'$lt':124}}"));
    cout<<col.Count()<<endl;

    return 0;
}
