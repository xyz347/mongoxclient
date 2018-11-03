#include <iostream>

#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>

#include "mongoxclient.hpp"

mongocxx::instance instance{};

using namespace std;

struct User {
    int uid;
    string name;
    int age;
    vector<string> tags;
    vector<vector<int>> test;
    XTOSTRUCT(A(uid, "bson:_id"), O(name, age, tags, test));
};

int main(int argc, char *argv[]) {
    mongocxx::uri uri("mongodb://test:test@127.0.0.1:27018/test");
    mongocxx::client client(uri);
    mongocxx::collection collect = client["test"]["test"];

    mongoxc::Collection col(collect);
   
    col.RemoveAll({});

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
    col.Upsert(u1.uid, u1);   
    
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
    col.Upsert(u1.uid, u1);   

    cout<<"======count=="<<endl;
    cout<<col.Count()<<endl;
    cout<<col.Find({{"_id",123}}).Count()<<endl;
    
    cout<<"======get====="<<endl;
    User get;
    User getid;
    vector<User> all;
    col.Find({{"_id",123}}).One(get);
    cout<<x2struct::X::tojson(get)<<endl;
    col.FindId(456).One(getid);
    cout<<x2struct::X::tojson(getid)<<endl;
    col.Find({}).All(all);
    for (size_t i=0; i<all.size(); i++) {
        cout<<x2struct::X::tojson(all[i])<<endl;
    }

    cout<<"===skip==="<<endl;
    all.clear();
    col.Find({}).Skip(1).All(all);
    for (size_t i=0; i<all.size(); i++) {
        cout<<x2struct::X::tojson(all[i])<<endl;
    }

    cout<<"===sort==="<<endl;
    all.clear();
    col.Find({}).Sort({{"_id",-1}}).All(all);
    for (size_t i=0; i<all.size(); i++) {
        cout<<x2struct::X::tojson(all[i])<<endl;
    }

    cout<<"===projection==="<<endl;
    all.clear();
    col.Find({}).Projection({{"name",1}}).All(all);
    for (size_t i=0; i<all.size(); i++) {
        cout<<x2struct::X::tojson(all[i])<<endl;
    }

    cout<<"===update all==="<<endl;
    col.UpdateAll({}, {{"$set", bb::vp{{"name", "haha"}}}});
    all.clear();
    col.Find({}).All(all);
    for (size_t i=0; i<all.size(); i++) {
        cout<<x2struct::X::tojson(all[i])<<endl;
    }

    cout<<"=====remove====="<<endl;
    col.Remove({{"_id",bb::vp{{"$lt", 124}}}});
    cout<<col.Count()<<endl;

    return 0;
}
