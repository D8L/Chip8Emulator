# Chip8Mac

Chip8Mac is another Chip-8 based emulator that uses C, C++, and SDL2 -- intended for MacOS.

![IBM Logo](https://imgur.com/a/eroq1G6)

![Brix](https://imgur.com/a/1mariQ6)

## Building and Running

This project requires *CMake* to build.

To build:

```
$ git clone https://github.com/D8L/Chip8Mac.git && cd Chip8
$ mkdir build && cd build
$ cmake ..
$ make
```

To run:

```
./chip8 [ROM filepath]
```

You can find some ROMs under the `roms` directory.

## References

* [CHIP-8](https://en.wikipedia.org/wiki/CHIP-8)
* [How to write an emulator][(https://tobiasvl.github.io/blog/write-a-chip-8-emulator/)
* [Cowgod's Chip-8 Technical Reference v1.0](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)
* [Lazy Foo' SDL2 Tutorial](http://lazyfoo.net/tutorials/SDL/index.php)
