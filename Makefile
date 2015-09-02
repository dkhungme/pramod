BUILD_DIR = build
INCLUDE_DIRS = -I ./include -I /usr/local/include

#proto objecst
PROTO_SRCS = $(wildcard src/proto/*.proto)
PROTO_CC = $(PROTO_SRCS:%.proto=%.pb.cc)
PROTO_OBJS = $(addprefix $(BUILD_DIR)/, $(PROTO_CC:%.cc=%.o))
-include $(PROTO_OBJS:%.o=%.d)

PROTO_HEADERS= $(PROTO_SRC:%.proto=%pb.h)

#utils objects
UTILS_SRCS = $(wildcard src/utils/*.cc)
UTILS_OBJS = $(addprefix $(BUILD_DIR)/, $(UTILS_SRCS:%.cc=%.o))
-include $(UTILS_OBJS:%.o=%.d)

ENC_SRCS = $(wildcard src/encrypt/*.cc)
ENC_OBJS = $(addprefix $(BUILD_DIR)/, $(ENC_SRCS:%.cc=%.o))
-include $(ENC_OBJS:%.o=%.d)

MIX_SRCS = $(wildcard src/mixer/*.cc)
MIX_OBJS = $(addprefix $(BUILD_DIR)/, $(MIX_SRCS:%.cc=%.o))
-include $(MIX_OBJS:%o=%.d)

SCRAMBLER_SRCS = $(wildcard src/scrambler/*.cc)
SCRAMBLER_OBJS = $(addprefix $(BUILD_DIR)/, $(SCRAMBLER_SRCS:%.cc=%.o))
-include $(MIX_OBJS:%o=%.d)

JOIN_SRCS = $(wildcard src/join/*.cc)
JOIN_OBJS = $(addprefix $(BUILD_DIR)/, $(JOIN_SRCS:%.cc=%.o))
-include $(MIX_OBJS:%o=%.d)

SORT_SRCS = $(wildcard src/sort/*.cc)
SORT_OBJS = $(addprefix $(BUILD_DIR)/, $(SORT_SRCS:%.cc=%.o))
-include $(SORT_OBJS:%.o=%.d)

TEST_GEN_SRC = $(wildcard test/DataGen.cc)
TEST_GEN_OBJ = $(addprefix $(BUILD_DIR)/, $(TEST_GEN_SRC:%.cc=%.o))

TEST_GC_GEN_SRC = $(wildcard test/Goodrich_Compact_DataGen.cc)
TEST_GC_GEN_OBJ = $(addprefix $(BUILD_DIR)/, $(TEST_GC_GEN_SRC:%.cc=%.o))

TEST_MIX_SRC = $(wildcard test/Mixnet.cc)
TEST_MIX_OBJ = $(addprefix $(BUILD_DIR)/, $(TEST_MIX_SRC:%.cc=%.o))

TEST_SCRAMBLER_SRC = $(wildcard test/Melbourne.cc)
TEST_SCRAMBLER_OBJ = $(addprefix $(BUILD_DIR)/, $(TEST_SCRAMBLER_SRC:%.cc=%.o))

TEST_JOIN_SRC = $(wildcard test/Ojoin.cc)
TEST_JOIN_OBJ = $(addprefix $(BUILD_DIR)/, $(TEST_JOIN_SRC:%.cc=%.o))


TEST_SORT_SRC = test/Sorter.cc
TEST_SORT_OBJ = build/test/Sorter.o

TEST_COMPACT_SRC = test/Compact.cc
TEST_COMPACT_OBJ = build/test/Compact.o

TEST_GROUP_SRC = test/Group.cc
TEST_GROUP_OBJ = build/test/Group.o

TEST_ENCRYPT_TIME_SRC = test/encrypt_time.cc
TEST_ENCRYPT_TIME_OBJ = build/test/encrypt_time.o

TEST_GOODRICH_SRC = test/Goodrich.cc
TEST_GOODRICH_OBJ = build/test/Goodrich.o

TEST_GOODRICH_COMPACT_SRC = test/Goodrich_Compact.cc
TEST_GOODRICH_COMPACT_OBJ = build/test/Goodrich_Compact.o

#datagen
DATA_GEN_EXE = $(BUILD_DIR)/test/datagen
GC_DATA_GEN_EXE = $(BUILD_DIR)/test/Goodrich_Compact_datagen
MIXER_EXE = $(BUILD_DIR)/test/mixer
SCRAMBLER_EXE = $(BUILD_DIR)/test/melbourne
JOIN_EXE = $(BUILD_DIR)/test/join
SORT_EXE = $(BUILD_DIR)/test/sorter
COMPACT_EXE = $(BUILD_DIR)/test/compact
GROUP_EXE = $(BUILD_DIR)/test/group
ENCRYPT_TIME_EXE = $(BUILD_DIR)/test/encrypt_time
GOODRICH_EXE = $(BUILD_DIR)/test/goodrich
GOODRICH_COMPACT_EXE = $(BUILD_DIR)/test/goodrich_compact

CXX = g++
CPPFLAGS =  -O3 -Wall -pthread -fPIC -std=c++11 -MMD $(INCLUDE_DIRS)
LDFLAGS = -lprotobuf -lglog -lgflags -lcrypto++ -lrt
PROTOC = protoc
PROTOFLAGS = --cpp_out

ALLOBJS = $(PROTO_OBJS) $(UTILS_OBJS) $(ENC_OBJS) $(MIX_OBJS) $(SORT_OBJS) $(SCRAMBLER_OBJS) $(JOIN_OBJS)

TESTOBJS = $(TEST_GEN_OBJ) $(TEST_MIX_OBJ) $(TEST_SCRAMBLER_OBJ) $(TEST_JOIN_OBJ) $(TEST_SORT_OBJ) $(TEST_GOODRICH_OBJ) $(TEST_GOODRICH_COMPACT_OBJ) $(TEST_GC_GEN_OBJ) $(TEST_COMPACT_OBJ) $(TEST_GROUP_OBJ) $(TEST_ENCRYPT_TIME_OBJ) 

.DEFAULT_GOAL = compile

test_gen: $(DATA_GEN_EXE)

test_gc_gen: $(GC_DATA_GEN_EXE)

test_mix: $(MIXER_EXE)

test_scrambler: $(SCRAMBLER_EXE)

test_join: $(JOIN_EXE)

test_sort: $(SORT_EXE)

test_compact: $(COMPACT_EXE)

test_group: $(GROUP_EXE)

test_encrypt_time: $(ENCRYPT_TIME_EXE)

test_goodrich: $(GOODRICH_EXE)

test_goodrich_compact: $(GOODRICH_COMPACT_EXE)

compile: $(ALLOBJS) 

$(ALLOBJS):$(BUILD_DIR)/%.o : %.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) -c $< -o $@
	
$(TESTOBJS):$(BUILD_DIR)/%.o : %.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) -c $< -o $@

$(PROTO_CC):%.pb.cc : %.proto
	$(PROTOC) --proto_path=src/proto --cpp_out=src/proto $<
	@mkdir -p include/proto 
	@cp $(@:%.cc=%.h) include/proto/

$(DATA_GEN_EXE): $(ALLOBJS) $(TEST_GEN_OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)	

$(GC_DATA_GEN_EXE): $(ALLOBJS) $(TEST_GC_GEN_OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)	

$(MIXER_EXE): $(ALLOBJS) $(TEST_MIX_OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)	


$(SCRAMBLER_EXE): $(ALLOBJS) $(TEST_SCRAMBLER_OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)	


$(JOIN_EXE): $(ALLOBJS) $(TEST_JOIN_OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)	


$(SORT_EXE): $(ALLOBJS) $(TEST_SORT_OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)	

$(COMPACT_EXE): $(ALLOBJS) $(TEST_COMPACT_OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)	

$(GROUP_EXE): $(ALLOBJS) $(TEST_GROUP_OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)	

$(ENCRYPT_TIME_EXE): $(ALLOBJS) $(TEST_ENCRYPT_TIME_OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)	


$(GOODRICH_EXE): $(ALLOBJS) $(TEST_GOODRICH_OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(GOODRICH_COMPACT_EXE): $(ALLOBJS) $(TEST_GOODRICH_COMPACT_OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

clean:
	rm -rf $(PROTO_CC)
	rm -rf $(PROTO_HEADERS)
	rm -rf $(BUILD_DIR)
