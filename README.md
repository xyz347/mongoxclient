# mongoxclient
利用[xbson](https://github.com/xyz347/xbson)对mongo-cxx-driver的一个简单封装，可以像golang一样直接操作结构体。只有基本接口，还不是很完备。**需要C++11支持**

-----
## 范例
范例请参考[main.cpp](test/main.cpp)，里面有各个接口的使用举例

## xpack::BsonBuilder
- 用于简化一些bson数据的构造，能用json来描述bson数据
- 名字比较长，这里用using bb = xpack::BsonBuilder;举例
- 简单用法是bb::En(string, args...)
    - string是一个类似于json的字符串，里面用单引号代替双引号
    - 里面用问号表示一个变量，问号后面可以加一个变量名称方便阅读，比如?uid
    - args的数量应该和问号的数量一致，对应key的参数必须是std::string/const char[]类型
    - 范例:bb::En("{'_id':?}", uid)
    - 对于比较复杂且会复用的builder，建议定义成static变量，后面调用Encode是线程安全的

## 重要说明
- thirdparty里面的mongocxx的lib是基于**g++ (Ubuntu 4.9.4-2ubuntu1~12.04) 4.9.4**编译的，非这个环境需要自行下载代码编译
