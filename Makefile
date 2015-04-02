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

TEST_GEN_SRC = $(wildcard test/DataGen.cc)
TEST_GEN_OBJ = $(addprefix $(BUILD_DIR)/, $(TEST_GEN_SRC:%.cc=%.o))

TEST_MIX_SRC = $(wildcard test/Mixnet.cc)
TEST_MIX_OBJ = $(addprefix $(BUILD_DIR)/, $(TEST_MIX_SRC:%.cc=%.o))

#datagen
DATA_GEN_EXE = $(BUILD_DIR)/test/datagen
MIXER_EXE = $(BUILD_DIR)/test/mixer


CXX = g++
CPPFLAGS =  -O3 -Wall -pthread -fPIC -std=c++11 -MMD $(INCLUDE_DIRS)
LDFLAGS = -lprotobuf -lglog -lgflags -lcrypto++
PROTOC = protoc
PROTOFLAGS = --cpp_out

ALLOBJS = $(PROTO_OBJS) $(UTILS_OBJS) $(ENC_OBJS) $(MIX_OBJS)

TESTOBJS = $(TEST_GEN_OBJ) $(TEST_MIX_OBJ)

.DEFAULT_GOAL = compile

test_gen: $(DATA_GEN_EXE)

test_mix: $(MIXER_EXE)

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

$(MIXER_EXE): $(ALLOBJS) $(TEST_MIX_OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)	

clean:
	rm -rf $(PROTO_CC)
	rm -rf $(PROTO_HEADERS)
	rm -rf $(BUILD_DIR)
