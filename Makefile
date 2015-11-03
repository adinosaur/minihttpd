
SRC_DIR = ./src
TEST_DIR = ./test
BUILD_DIR = ./build
INCLUDE_DIR = ./include
ARG = --std=c++11 -Wall -g
OBJ = logger.o md5.o helper.o base64.o strTime.o http.o httpBase.o httpAuthorization.o httpRequest.o httpResponse.o iomultiplexing.o channel.o randomString.o
CC = clang++-3.6
#CC = g++

all: $(OBJ)
	$(CC) $(ARG) $(SRC_DIR)/main.cc $(BUILD_DIR)/*.o -o $(BUILD_DIR)/httpd.out -lpthread

# io复用模块
iomultiplexing.o:
	$(CC) $(ARG) -c $(INCLUDE_DIR)/EventLoop/IOMultiplexing.cc -o $(BUILD_DIR)/iomultiplexing.o

# Channel模块
channel.o:
	$(CC) $(ARG) -c $(INCLUDE_DIR)/EventLoop/Channel.cc -o $(BUILD_DIR)/channel.o

# Http模块
http.o: helper.o
	$(CC) $(ARG) -c $(INCLUDE_DIR)/Http/Http.cc -o $(BUILD_DIR)/http.o

# HttpBase模块
httpBase.o:
	$(CC) $(ARG) -c $(INCLUDE_DIR)/Http/HttpBase.cc -o $(BUILD_DIR)/httpBase.o

# Http认证模块
httpAuthorization.o: httpBase.o
	$(CC) $(ARG) -c $(INCLUDE_DIR)/Http/HttpAuthorization.cc -o $(BUILD_DIR)/httpAuthorization.o

# HttpRequest模块
httpRequest.o:
	$(CC) $(ARG) -c $(INCLUDE_DIR)/Http/HttpRequest.cc -o $(BUILD_DIR)/httpRequest.o

# HttpResponse模块
httpResponse.o:
	$(CC) $(ARG) -c $(INCLUDE_DIR)/Http/HttpResponse.cc -o $(BUILD_DIR)/httpResponse.o

# 日志模块
logger.o:
	$(CC) $(ARG) -c $(INCLUDE_DIR)/Base/Logger.cc -o $(BUILD_DIR)/logger.o

logger.test: logger.o
	$(CC) $(ARG) $(TEST_DIR)/Logger_test.cc $(BUILD_DIR)/logger.o -o $(BUILD_DIR)/logger.test -lpthread

# Helper模块
helper.o:
	$(CC) $(ARG) -c $(INCLUDE_DIR)/Base/Helper.cc -o $(BUILD_DIR)/helper.o

helper.test: helper.o
	$(CC) $(ARG) $(TEST_DIR)/Helper_test.cc $(BUILD_DIR)/helper.o -o $(BUILD_DIR)/helper.test

# RandomString模块
randomString.o:
	$(CC) $(ARG) -c $(INCLUDE_DIR)/Base/RandomString.cc -o $(BUILD_DIR)/randomString.o

randomString.test: randomString.o md5.o
	$(CC) $(ARG) $(TEST_DIR)/RandomString_test.cc $(BUILD_DIR)/randomString.o $(BUILD_DIR)/md5.o -o $(BUILD_DIR)/randomString.test

# Base64模块
base64.o:
	$(CC) $(ARG) -c $(INCLUDE_DIR)/Base/Base64.cc -o $(BUILD_DIR)/base64.o

base64.test: base64.o
	$(CC) $(ARG) $(TEST_DIR)/Base64_test.cc $(BUILD_DIR)/base64.o -o $(BUILD_DIR)/base64.test

# StrTime模块
strTime.o:
	$(CC) $(ARG) -c $(INCLUDE_DIR)/Base/StrTime.cc -o $(BUILD_DIR)/strTime.o

strTime.test: strTime.o
	$(CC) $(ARG) $(TEST_DIR)/StrTime_test.cc $(BUILD_DIR)/strTime.o -o $(BUILD_DIR)/strTime.test

blockingQueue.test:
	$(CC) $(ARG) $(TEST_DIR)/BlockingQueue_test.cc -o $(BUILD_DIR)/blockingQueue.test -lpthread

md5.o:
	$(CC) $(ARG) -c $(INCLUDE_DIR)/md5/md5.cc -o $(BUILD_DIR)/md5.o

clean:
	rm ./build/*

