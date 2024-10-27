# chip-8-emu
An "emulator"/interpreter for the CHIP-8 system

Completed as a weekend project to get started in emulation development.

The CHIP-8 system dates back to 1977 when it was present on computers such as the COSMAC VIP and the Telmac 1800.

You can run ROMs by drag-and-dropping files onto the compiled chip-8.exe or by running `chip-8.exe <path-to-rom>` in a terminal. ROMs are widely accessible online and I highly recommend visiting the [CHIP-8 Archive](https://johnearnest.github.io/chip8Archive/) to source some CC0-licensed games to try, especially from the Octojam project.

Huge thanks to [Tobias V. Langhoff's high level guide](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/) on understanding CHIP-8 for guiding my thoughts.

## Compiling
I included the Visual Studio solution file and related configs that I use to compile the executable. I use MSVS 2022 to build the solution targeting C++20 and run the `chip-8.exe` file present in `\x64\debug`.
The project relies on SDL2 being present at `C:\SDL2` for the links and imports unless you change the config locally on your machine. Additionally, `SDL2.dll` must be present in the same directory as the `chip-8.exe` file.

This probably isn't ideal for sharing, but that is current situation. I will include pictures online to show the emulator working if you can't or otherwise don't want to compile the executable yourself.
