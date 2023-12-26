UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)  # macOS
		CORES := $(shell sysctl -n hw.ncpu)
else  # Linux
		CORES := $(shell nproc)
endif
JOBS := $(shell expr $(CORES) / 2)

protofiles:
	@echo "Building proto..."
	node ./build-protofiles.js

jsframework:
	@echo "Building jsframework..."
	node ./build-jsframework.js --clean=$(clean)

darwin: protofiles
	@echo "Building for darwin(JOBS=${JOBS})..."
	rm -rf targets/darwin
	sh ./configure-darwin.sh
	make -C targets/darwin -j${JOBS} install

android: protofiles
	@echo "Building for android(JOBS=${JOBS})..." 
	rm -rf targets/android
	sh ./configure-android.sh
	make -C targets/android -j${JOBS} install

windows: protofiles
	cmake -DCMAKE_BUILD_TYPE=Release -B targets/windows ..
	cmake --build ./targets/windows --config Release --target install

all: jsframework darwin android

.PHONY: jsframework darwin android all
