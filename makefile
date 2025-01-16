UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)  # macOS
		CORES := $(shell sysctl -n hw.ncpu)
else  # Linux
		CORES := $(shell nproc)
endif
JOBS := $(shell expr $(CORES) / 1)

# Define the CLEAN flag (default is no) that will clean the build directory
CLEAN ?= no

# Define the RELEASE flag (default is yes) that will build in release mode
RELEASE ?= no

# Define the VERBOSE flag (default is no) that will print the commands
VERBOSE ?= no

# Define the CRATES_ONLY flag (default is no) that will build only the crates
CRATES_ONLY ?= no

# Define the CRATES_FORCE_REBUILD flag (default is no) that will force the crates to be rebuilt
CRATES_FORCE_REBUILD ?= no

# Update the flags if RELEASE is set to yes
ifeq ($(RELEASE), yes)
	CLEAN := yes
endif

# This makefile defines a function `build_crates` to build Rust crates using Cargo.
# 
# Usage:
#   $(call build_crates, <target>)
#
# Parameters:
#   <target> - The target triple for which the crates should be built (e.g., x86_64-unknown-linux-gnu).
define build_crates
	@start_time=$$(date +%s); 																\
		build_args=""; 																					\
		if [ "$(RELEASE)" = "yes" ]; then 											\
			build_args="$$build_args --release"; 									\
		fi; 																										\
		if [ "$(VERBOSE)" = "yes" ]; then 											\
			build_args="$$build_args --verbose"; 									\
		fi; 																										\
		FORCE_REBUILD=$${CRATES_FORCE_REBUILD:-0} 							\
			cargo build --target=$(1) $$build_args;								\
		end_time=$$(date +%s); 																	\
		elapsed_time=$$((end_time - start_time)); 							\
		echo "Built the crates for $(1), +$${elapsed_time}s"
endef

# This target creates a universal Apple binary by combining the binaries for
# aarch64-apple-darwin and x86_64-apple-darwin architectures using the `lipo` tool.
# It performs the following steps:
# 1. Records the start time.
# 2. Creates the necessary output directory if it doesn't exist.
# 3. Uses `lipo` to create a universal binary for `libjsar_jsbindings.a` from the
#    aarch64 and x86_64 binaries.
# 4. Uses `lipo` to create a universal binary for `libjsar_jsbundle.a` from the
#    aarch64 and x86_64 binaries.
# 5. Records the end time.
# 6. Calculates and prints the elapsed time taken to create the universal binaries.
define create_universal_apple_binary
	@start_time=$$(date +%s); 																																							\
		RELEASE_DIR=$(if $(filter yes,$(RELEASE)),release,debug); 																						\
		mkdir -p build/output/crates/universal-apple-darwin/$$RELEASE_DIR; 																		\
		lipo -create -output build/output/crates/universal-apple-darwin/$$RELEASE_DIR/libjsar_jsbindings.a 		\
			build/output/crates/aarch64-apple-darwin/$$RELEASE_DIR/libjsar_jsbindings.a 												\
			build/output/crates/x86_64-apple-darwin/$$RELEASE_DIR/libjsar_jsbindings.a; 												\
		lipo -create -output build/output/crates/universal-apple-darwin/$$RELEASE_DIR/libjsar_jsbundle.a			\
			build/output/crates/aarch64-apple-darwin/$$RELEASE_DIR/libjsar_jsbundle.a 													\
			build/output/crates/x86_64-apple-darwin/$$RELEASE_DIR/libjsar_jsbundle.a; 													\
		end_time=$$(date +%s); 																																								\
		elapsed_time=$$((end_time - start_time));																															\
		echo "Created the crates for universal-apple-darwin/$${RELEASE_DIR}, +$${elapsed_time}s"
endef

jsbundle:
	@echo "Building jsbundle..."
	node ./build/build-jsbundle.cjs \
		--clean=$(CLEAN) \
		--minify=$(MINIFY) \
		--without-pack=$(NO_PACK)

darwin:
	@echo "Building for darwin(JOBS=${JOBS})..."
	@$(call build_crates,aarch64-apple-darwin)
	@$(call build_crates,x86_64-apple-darwin)
	@$(call create_universal_apple_binary)
ifeq ($(CRATES_ONLY), no)
	$(MAKE) -C ./build darwin -j$(JOBS)
endif

android:
	@echo "Building for android(JOBS=${JOBS})..."
	@$(call build_crates,aarch64-linux-android)
ifeq ($(CRATES_ONLY), no)
	$(MAKE) -C ./build android
endif

windows:
	@echo "Building for windows(JOBS=${JOBS})..."
	@$(call build_crates,x86_64-pc-windows-msvc)
ifeq ($(CRATES_ONLY), no)
	$(MAKE) -C ./build windows
endif

test:
	cargo test
ifeq ($(CRATES_ONLY), no)
	ctest --test-dir build/targets/darwin
endif

.PHONY: jsbundle darwin android test all
