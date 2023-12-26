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

jsbundle:
	@echo "Building jsbundle..."
	node ./build/build-jsbundle.cjs --clean=$(clean)

all: jsbundle darwin android

.PHONY: jsbundle darwin android all
