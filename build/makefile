CLEAN ?= no
RELEASE ?= no

define CLEAN_AND_RECONFIGURE
	rm -rf targets/$1; \
		CONFIGURE_ARGS=; \
		if [ "$(RELEASE)" = "yes" ]; then \
			CONFIGURE_ARGS="--release"; \
		else \
			CONFIGURE_ARGS="--debug"; \
		fi; \
		if [ "$(INSPECTOR)" = "yes" ]; then \
			CONFIGURE_ARGS="$$CONFIGURE_ARGS --build-inspector"; \
		fi; \
		./configure.sh $1 $$CONFIGURE_ARGS
endef

define BUILD_INSTALL
	${MAKE} -C targets/$1 install
endef

# This macro defines the BUILD_ON process.
# It first checks if the CLEAN variable is defined.
# If CLEAN is defined, it calls the CLEAN_AND_RECONFIGURE function with the given argument ($1).
# Regardless of the CLEAN variable, it then calls the BUILD_INSTALL function with the given argument ($1).
define BUILD_ON
	@if [ "$(CLEAN)" = "yes" ]; then \
		$(call CLEAN_AND_RECONFIGURE,$1); \
	fi
	$(call BUILD_INSTALL,$1)
endef

darwin:
	$(call BUILD_ON,darwin)

android:
	$(call BUILD_ON,android)

windows:
	cmake -DCMAKE_BUILD_TYPE=Release -B targets/windows ..
	cmake --build ./targets/windows --config Release --target install

all: windows darwin android

.PHONY: windows darwin android all
