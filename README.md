# mongoxclient
利用x2struct对mongo-cxx-driver的一个简单封装，可以像golang一样直接操作结构体。只有基本接口，还不是很完备。

-----
## 范例
范例请参考[main.cpp](test/main.cpp)，里面有各个接口的使用举例
```C++
#include <iostream>

#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>

#include "mongoxclient.hpp"       // 包含mongoxclient.hpp

mongocxx::instance instance{};

using namespace std;

// 结构体定义例子
struct User {
    int uid;
    string name;
    int age;
    vector<string> tags;
    vector<vector<int>> test;
    XTOSTRUCT(A(uid, "bson:_id"), O(name, age, tags, test));  // XTOSTRUCT定义，uid作为bson的_id
};

int main(int argc, char *argv[]) {
    mongocxx::uri uri("mongodb://test:test@127.0.0.1:27018/test");
    mongocxx::client client(uri);
    mongocxx::collection collect = client["test"]["test"];

    mongoxc::Collection col(collect);   // 利用mongocxx::collection构造一个mongoxc::Collection
   
    col.RemoveAll({});                  // 删除所有的document

    User u1;
    // ... 初始化u1结构体
    col.Upsert(u1.uid, u1);             // Upsert一个结构体
    
    u1.uid = 456;
    // ... 重新初始化u1结构体
    col.Upsert(u1.uid, u1);             // Upsert另一个结构体   

    u1.uid = 789;
    u1.name = "insert";
    col.Insert(u1);                     // Insert一个结构体

    col.Insert(bb::vp{{"_id",1111}, {"name", "insert_bb"}});  // Insert一个bb::vp结构

    cout<<"======count=="<<endl;
    cout<<col.Count()<<endl;                      // Count整个collection
    cout<<col.Find({{"_id",123}}).Count()<<endl;  // 基于某种查找的Count
    
    cout<<"======get====="<<endl;
    User get;
    User getid;
    vector<User> all;
    
    col.Find({{"_id",123}}).One(get);             // 根据bb::vp查找一个document
    cout<<x2struct::X::tojson(get)<<endl;
    
    col.FindId(456).One(getid);                   // 根据_id查找一个document
    cout<<x2struct::X::tojson(getid)<<endl;
    col.Find({}).All(all);                        // 根据bb::vp查找多个document
    for (size_t i=0; i<all.size(); i++) {
        cout<<x2struct::X::tojson(all[i])<<endl;
    }

    cout<<"===skip==="<<endl;
    all.clear();
    col.Find({}).Skip(1).All(all);                // Skip若干个然后查找所有document
    for (size_t i=0; i<all.size(); i++) {
        cout<<x2struct::X::tojson(all[i])<<endl;
    }

    cout<<"===sort==="<<endl;
    all.clear();
    col.Find({}).Sort({{"_id",-1}}).All(all);     // 根据bb::vp sort
    for (size_t i=0; i<all.size(); i++) {
        cout<<x2struct::X::tojson(all[i])<<endl;
    }

    cout<<"===projection==="<<endl;
    all.clear();
    col.Find({}).Projection({{"name",1}}).All(all); // 过滤输出
    for (size_t i=0; i<all.size(); i++) {
        cout<<x2struct::X::tojson(all[i])<<endl;
    }

    cout<<"===update all==="<<endl;
    col.UpdateAll({}, {{"$set", bb::vp{{"name", "haha"}}}});  // UpdateAll
    all.clear();
    col.Find({}).All(all);
    for (size_t i=0; i<all.size(); i++) {
        cout<<x2struct::X::tojson(all[i])<<endl;
    }

    cout<<"=====remove====="<<endl;
    col.Remove({{"_id",bb::vp{{"$lt", 124}}}});       // Remove
    cout<<col.Count()<<endl;

    return 0;
}
```

## bb::vp
- bb是namespace，bson builder的意思，起一个短名方便使用
- vp是vector pair的意思，本质是std::vector<std::pair>，pair是<std::string, bb::intf>，intf是interfere的意思，具体支持的类型可以看[bson_builder.h](https://github.com/xyz347/x2struct/blob/master/bson_builder.h)
- vp对应的是mgo/bson的M，由于C++不支持A:B这种初始化map的语法，所以用pair来表征一个key value对，然后多个kv对需要用一个vector存储。构造起来比Golang的bson.M麻烦（只要是两层大括号），但是比bsoncxx简单不少。
- 比如构造一个bson.M{"_id":10} 这种数据只需要写{{"_id",10}}即可
- 构造bson.M{"_id":bson.M{"$lt":10}}则是 {{"_id",bb::vp{{"$lt",10}}}} 最别扭的就是多了一倍的括号
- 用pair可以省一层括号，但是表述能力很受限，为了统一，还是全部用vector pair
