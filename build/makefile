JOBS ?= 1

define CLEAN_AND_RECONFIGURE
	rm -rf targets/$1
	sh ./configure-$1.sh
endef

define BUILD_INSTALL
	make -C targets/$1 -j${JOBS} install
endef

darwin:
ifdef clean
	$(call CLEAN_AND_RECONFIGURE,darwin)
endif
	$(call BUILD_INSTALL,darwin)

android:
ifdef clean
	$(call CLEAN_AND_RECONFIGURE,android)
endif
	$(call BUILD_INSTALL,android)

windows:
	cmake -DCMAKE_BUILD_TYPE=Release -B targets/windows ..
	cmake --build ./targets/windows --config Release --target install

all: windows darwin android

.PHONY: windows darwin android all
