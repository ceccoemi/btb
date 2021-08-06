TARGET := btb
TARGET_TEST := btb-test
SRC_DIR := src
TEST_DIR := test
TARGET_MAIN := main.c
TARGET_TEST_MAIN := main_test.c
VPATH = $(SRC_DIR) $(TEST_DIR)

CC := gcc
WARNINGS := -Wall -Werror
CSTD := c17
CFLAGS_ALL := -std=$(CSTD) $(WARNINGS)
CFLAGS_TEST := -g
CFLAGS_RELEASE := -O3
LIBS := -lcrypto -lcurl -lpthread -lm
VALGRIND_SUPP_FILE := valgrind.supp
VALGRIND_CMD := valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --suppressions=$(VALGRIND_SUPP_FILE)

.PHONY: clean test

all: test build

build: $(TARGET_MAIN) src_obj
	$(CC) $(CFLAGS_ALL) $(CFLAGS_RELEASE) -I $(SRC_DIR) -o $(TARGET) *.o $< $(LIBS)

test: $(TARGET_TEST_MAIN) src_obj test_obj
	$(CC) $(CFLAGS_ALL) $(CFLAGS_TEST) -I $(SRC_DIR) -I $(TEST_DIR) -o $(TARGET_TEST) *.o $< $(LIBS)
	$(VALGRIND_CMD) ./$(TARGET_TEST)

test_objects := $(patsubst %.c,%.o,$(wildcard $(TEST_DIR)/*.c))
test_obj: $(test_objects)
	@ touch test_obj

src_objects := $(patsubst %.c,%.o,$(wildcard $(SRC_DIR)/*.c))
src_obj: $(src_objects)
	@ touch src_obj

%.o: %.c
	$(CC) $(CFLAGS_ALL) -c $<

clean:
	@ $(RM) $(TARGET) $(TARGET_TEST) *.o src_obj test_obj
