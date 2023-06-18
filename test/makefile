INC=-I../ -I../.. -I/usr/local/include/mongocxx/v_noabi -I/usr/local/include/bsoncxx/v_noabi
#-I../thirdparty -I../thirdparty/mongo-cxx-driver/include -L../thirdparty/mongo-cxx-driver/lib
test:main.cpp
	g++ -o $@ -g $^ -std=c++11 $(INC)  -lmongocxx -lbsoncxx -lmongoc-1.0  -lbson-1.0  -pthread -lrt -lssl -lcrypto

clean:
	@-rm -rf test
