# makefile taken from https://github.com/jerryjhird/Flatworm

CC = clang
CFLAGS = $(shell pkg-config libusb-1.0 --cflags) -std=c99 -Wall -Wextra -Isrc -Isrc/libx360con -Isrc/devel -MMD -MP
LDFLAGS = $(shell pkg-config libusb-1.0 --libs) -lpthread

LIB_SRC_DIR = src/libx360con
EXAMPLE_SRC_DIR = src/example
BUILD_DIR = build
DIST_DIR = dist

LIB_SRCS := $(shell find $(LIB_SRC_DIR) -name '*.c')
LIB_OBJS := $(LIB_SRCS:$(LIB_SRC_DIR)/%.c=$(BUILD_DIR)/lib/%.o)

EXAMPLE_SRCS := $(shell find $(EXAMPLE_SRC_DIR) -name '*.c')
EXAMPLE_OBJS := $(EXAMPLE_SRCS:$(EXAMPLE_SRC_DIR)/%.c=$(BUILD_DIR)/example/%.o)

ALL_SRCS := $(LIB_SRCS) $(EXAMPLE_SRCS)
DEPS := $(LIB_OBJS:.o=.d) $(EXAMPLE_OBJS:.o=.d)

TARGET_LIB = $(DIST_DIR)/libx360con.a
TARGET_EXAMPLE = $(DIST_DIR)/example

all: $(DIST_DIR) $(TARGET_LIB) $(TARGET_EXAMPLE) compile_commands.json

$(DIST_DIR):
	mkdir -p $(DIST_DIR)

$(TARGET_LIB): $(LIB_OBJS)
	mkdir -p $(DIST_DIR)
	ar rcs $@ $(LIB_OBJS)

$(TARGET_EXAMPLE): $(EXAMPLE_OBJS) $(TARGET_LIB)
	mkdir -p $(DIST_DIR)
	$(CC) $(EXAMPLE_OBJS) $(TARGET_LIB) -o $@ $(LDFLAGS)

$(BUILD_DIR)/lib/%.o: $(LIB_SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/example/%.o: $(EXAMPLE_SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

compile_commands.json: $(ALL_SRCS)
	@echo "[" > $@
	@comma=""; \
	for src in $(ALL_SRCS); do \
		if [ "$$src" = "$(LIB_SRC_DIR)"* ]; then \
			rel_path="$${src#$(LIB_SRC_DIR)/}"; \
			obj="$(BUILD_DIR)/lib/$${rel_path%.*}.o"; \
		else \
			rel_path="$${src#$(EXAMPLE_SRC_DIR)/}"; \
			obj="$(BUILD_DIR)/example/$${rel_path%.*}.o"; \
		fi; \
		printf "%s\n  {\n    \"directory\": \"%s\",\n    \"command\": \"%s %s -c %s -o %s\",\n    \"file\": \"%s\",\n    \"output\": \"%s\"\n  }" \
			"$$comma" "$(CURDIR)" "$(CC)" "$(subst ",\",$(CFLAGS))" "$$src" "$$obj" "$$src" "$$obj" >> $@; \
		comma=","; \
	done
	@printf "\n]\n" >> $@

-include $(DEPS)

clean:
	rm -rf $(BUILD_DIR) $(DIST_DIR) compile_commands.json

.PHONY: all clean
