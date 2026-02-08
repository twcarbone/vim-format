.PHONY: first
first: build

.PHONY: configure
configure:
	cmake -B build/

.PHONY: build
build: configure
	cmake --build build/

.PHONY: build/debug
build/debug:
	cmake -B build/ -DCMAKE_BUILD_TYPE=Debug
	cmake --build build/

.PHONY: test
test: build
	cd build/test && ./unittests

.PHONY: clean
clean:
	rm -rf build/

.PHONY: format
format:
	find src/ test/ -type f \( -name '*.h' -or -name '*.cpp' \) -exec clang-format -i {} +

.PHONY: format/check
format/check:
	find src/ test/ -type f \( -name '*.h' -or -name '*.cpp' \) -exec clang-format --dry-run -Werror {} +
