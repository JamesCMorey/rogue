# Nice names
run:./build/rogue
	./build/rogue

compile:build
	meson compile -C build

setup:
	meson setup build

# Real files
./build/rogue:build *.c
	meson compile -C build

build:
	meson setup build --buildtype debug
