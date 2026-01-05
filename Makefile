.PHONY: first configure build test clean

first: build

configure:
	cmake -B build/

build: configure
	cmake --build build/

build-debug:
	cmake -B build/ -DCMAKE_BUILD_TYPE=Debug
	cmake --build build/

test: build
	cd build/test && ./unittests

clean:
	rm -rf build/

format:
	find src/ test/ -type f \( -name '*.h' -or -name '*.cpp' \) -exec clang-format -i {} +
