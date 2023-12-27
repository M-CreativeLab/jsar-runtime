UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)  # macOS
		CORES := $(shell sysctl -n hw.ncpu)
else  # Linux
		CORES := $(shell nproc)
endif
JOBS := $(shell expr $(CORES) / 2)

protofiles:
	@echo "Building proto..."
	node ./build/build-protofiles.cjs

crates:
	cargo build \
		--target-dir ./build/output/crates \
		--release
	./build/output/crates/release/test_jsbundle_loader

jsbundle:
	@echo "Building jsbundle..."
	node ./build/build-jsbundle.cjs \
		--clean=$(clean) \
		--minify=$(minify) \
		--without-pack=$(without-pack)

darwin: protofiles crates
	@echo "Building for darwin(JOBS=${JOBS})..."
	make -C ./build -j${JOBS} darwin

android: protofiles crates
	@echo "Building for android(JOBS=${JOBS})..."
	make -C ./build -j${JOBS} android

windows: protofiles crates
	@echo "Building for windows(JOBS=${JOBS})..."
	make -C ./build -j${JOBS} windows

all: jsbundle crates darwin android

.PHONY: jsbundle crates darwin android all
