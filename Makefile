
SRC_DIR = ./src
TEST_DIR = ./test
BUILD_DIR = ./build
INCLUDE_DIR = ./include
ARG = --std=c++11 -Wall -g
OBJ = md5.o
CC = clang++-3.6
#CC = g++

all: $(OBJ)
	$(CC) $(ARG) $(SRC_DIR)/main.cc $(BUILD_DIR)/$(OBJ) -o $(BUILD_DIR)/httpd.out

tst: $(OBJ)
	$(CC) $(ARG) $(TEST_DIR)/test_trie_tree.cc -o $(BUILD_DIR)/trie_tree_test.out
	$(CC) $(ARG) $(TEST_DIR)/test_http_authorization.cc $(BUILD_DIR)/$(OBJ) -o $(BUILD_DIR)/http_authorization_test.out
	$(CC) $(ARG) $(TEST_DIR)/test_md5.cc $(BUILD_DIR)/$(OBJ) -o $(BUILD_DIR)/md5_test.out

tst_md5: md5.o
	$(CC) $(ARG) $(TEST_DIR)/test_md5.cc $(BUILD_DIR)/$(OBJ) -o $(BUILD_DIR)/md5_test.out

tst_url:
	$(CC) $(ARG) $(TEST_DIR)/test_urlcheck.cc $(BUILD_DIR)/$(OBJ) -o $(BUILD_DIR)/url_test.out

tst_rstr: md5.o
	$(CC) $(ARG) $(TEST_DIR)/test_random_string.cc $(BUILD_DIR)/$(OBJ) -o $(BUILD_DIR)/rstr_test.out

md5.o:
	$(CC) $(ARG) -c $(INCLUDE_DIR)/md5/md5.cc -o $(BUILD_DIR)/md5.o

clean:
	rm ./build/*

