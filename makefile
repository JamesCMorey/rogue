# Nice names
run:./build/rogue
	./build/rogue

compile:build
	meson compile -C build

setup:
	meson setup build

# Real files
./build/rogue:build main.c
	meson compile -C build

build:
	meson setup build
