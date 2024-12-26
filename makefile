UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)  # macOS
		CORES := $(shell sysctl -n hw.ncpu)
else  # Linux
		CORES := $(shell nproc)
endif
JOBS := $(shell expr $(CORES) / 2)

define build_crates
	cargo build --release --target=$(1)
endef

define create_universal_apple_binary
	@echo "Creating universal-apple-darwin..."
	mkdir -p build/output/crates/universal-apple-darwin/release
	lipo -create -output build/output/crates/universal-apple-darwin/release/libjsar_jsbindings.a \
		build/output/crates/aarch64-apple-darwin/release/libjsar_jsbindings.a \
		build/output/crates/x86_64-apple-darwin/release/libjsar_jsbindings.a
	lipo -create -output build/output/crates/universal-apple-darwin/release/libjsar_jsbundle.a \
		build/output/crates/aarch64-apple-darwin/release/libjsar_jsbundle.a \
		build/output/crates/x86_64-apple-darwin/release/libjsar_jsbundle.a
endef

jsbundle:
	@echo "Building jsbundle..."
	node ./build/build-jsbundle.cjs \
		--clean=$(clean) \
		--minify=$(minify) \
		--without-pack=$(without-pack)

darwin:
	@echo "Building for darwin(JOBS=${JOBS})..."
	@$(call build_crates,aarch64-apple-darwin)
	@$(call build_crates,x86_64-apple-darwin)
	@$(call create_universal_apple_binary)
	make -C ./build darwin JOBS=${JOBS}

android:
	@echo "Building for android(JOBS=${JOBS})..."
	@$(call build_crates,aarch64-linux-android)
	make -C ./build android JOBS=${JOBS}

windows:
	@echo "Building for windows(JOBS=${JOBS})..."
	@$(call build_crates,x86_64-pc-windows-msvc)
	make -C ./build windows JOBS=${JOBS}

test:
	cargo test
	ctest --test-dir build/targets/darwin

.PHONY: jsbundle darwin android test all
