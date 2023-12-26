darwin:
	rm -rf targets/darwin
	sh ./configure-darwin.sh
	make -C targets/darwin -j${JOBS} install

android:
	rm -rf targets/android
	sh ./configure-android.sh
	make -C targets/android -j${JOBS} install

windows:
	cmake -DCMAKE_BUILD_TYPE=Release -B targets/windows ..
	cmake --build ./targets/windows --config Release --target install

all: windows darwin android

.PHONY: windows darwin android all
