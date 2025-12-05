.PHONY: first configure build test clean

first: build

configure:
	cmake -B build/

build: configure
	cmake --build build/

test: build
	cd build/test && ./unittests

clean:
	rm -rf build/
