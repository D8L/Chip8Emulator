# Chip8Emulator

Chip8Mac is another Chip-8 based emulator that uses C, C++, SDL2, and CMake.

![IBM Logo](https://i.imgur.com/429qxi3.png)
![Brix](https://i.imgur.com/NVQt0hd.png)

## Getting Started

Follow these steps to set up the project on your local machine:

1. Clone the repository:

```
git clone https://github.com/D8L/Chip8Emulator.git
```

2. Navigate to the project directory:

```
cd Chip8
```

3. Build the project:

```
mkdir build && cd build
cmake ..
make
```

4. Run the project:

```
./chip8 [ROM filepath]
```
You can find some ROMs under the `roms` directory.

## Built With

* C
* C++
* SDL2
* CMake

## Authors

* D8L

## References

* [CHIP-8](https://en.wikipedia.org/wiki/CHIP-8)
* [How to write an emulator](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/)
* [Cowgod's Chip-8 Technical Reference v1.0](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)
* [Lazy Foo' SDL2 Tutorial](http://lazyfoo.net/tutorials/SDL/index.php)
