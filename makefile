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

jsframework:
	@echo "Building jsframework..."
	node ./build/build-jsframework.cjs --clean=$(clean)

all: jsframework darwin android

.PHONY: jsframework darwin android all
