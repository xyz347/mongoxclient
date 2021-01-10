test:main.cpp
	g++ -I../ -I ../.. -I../thirdparty -I ../thirdparty/mongo-cxx-driver/include -o $@ $^ -std=c++11 -DXTOSTRUCT_BSON \
	-L../thirdparty/mongo-cxx-driver/lib -l mongocxx -lbsoncxx -lmongoc-1.0  -lbson-1.0 -lssl -pthread -lrt

clean:
	@-rm -rf test
