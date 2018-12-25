# appmodmap

**appmodmap** is a daemon that watches the currently active window and dynamically adjusts your system settings (such as keyboard, etc.) as the active window class changes. You create a set of primitive operations and then you can select any combination of them for any particular X11 window class. It was written for Linux but should work on any POSIXy thing with recent X11 libraries.

**appmodmap** was originally written as a better way of dynamically enabling the Command key "like it oughta work" by enabling and disabling mapping it to the Control key based on application support and the system environment. This original usage is included as a set of demo primitives which can be used "out of the box."

## Setting it up

**appmodmap** comes as a collection of shell scripts, a `Makefile`, a single C source file, and a header file that is compiled into that source file called `config.h` with configuration information. The main daemon has no dependencies other than X11 itself, though the included "demo" shell scripts it uses as primitives call `setxkbmap` and manipulate GNOME settings.

When you run the `Makefile` with `make`, a directory `~/.appmodmaps` will be created. This folder contains the demo `.do` and `.undo` primitives (see below), which are shell scripts, and the local copy of `config.h` which you modify for your own settings. Don't change the ones actually in the repository unless you intend to generate a PR to make your changes the default.

The `Makefile` will then compile two binaries: `obj/ammd`, the main daemon, which can be run from any Terminal window or as part of your X startup scripts, and `obj/winclass`, which is a debugging tool to tell you the class hint of the currently active window (it displays the window ID, class hint window name and the window class as you move between windows). You can move these binaries anywhere convenient.

Whenever you run `make` again, your local copy of `config.h` in `~/.appmodmaps` is used, not the `config.h` that comes with the repository. If you want to reset it or update it with later changes such as updated primitives, either merge your directory together with the new copy of `appmodmap`, or remove the `~/.appmodmaps` directory entirely and it will be recreated.

## Running it

Just start `ammd` (in the background if you like with `&` or as your shell requires). When the daemon is terminated, killed or otherwise, it will reset everything back to the default before exiting.

Similarly, to watch windows and class hints without running primitives, just start `winclass` in the same fashion, and Control-C to quit it.

## Configuring your own settings

The default "demo" files that comes with **appmodmap**  implement toggling Command-key remapping with certain apps. We'll use this here to illustrate how to create more complex situations, or add more applications.

In `~/.appmodmaps/config.h` is a mapping of window names (as specified in `XClassHint`, which you can obtain from `winclass`) to bit values. The demo files have a single bit (`1<<31`) which is set for those apps that need it, but you can use up to any combination of 32 individual primitive states by just `or`ing bits together. Window classes that do _not_ appear in `~/.appmodmaps/config.h` are considered to have a bit value of 0.

When `ammd` sees a new window accept input, it retrieves the name from the class hint and its required new bit value (or 0). If the current bit value does not match what the new window requires, it will iterate through each bit of the new value to make the current value match. Bits that need to be set will trigger calling the "do" primitive for that bit value; bits that need to be unset will trigger calling the "undo" primitive.

Let's show how this works with the included Command key demo. We switch from GNOME terminal, which allows you to remap its keyboard shortcuts, to Nautilus, which doesn't. In the terminal, the bit value is 0 (use no settings), because it does not appear in `~/.appmodmaps/config.h`. Nautilus, however, has a value of `1<<31`. When we switch to Nautilus, `ammd` computes that we need to turn on bit 31 to get from 0, and runs that primitive by calling the shell script `~/.appmodmaps/31.do` to set the necessary keyboard changes. If we switch from Nautilus to any other app with the same bit value, `ammd` does nothing, since the new value is unchanged. But if we switch back to GNOME terminal, `ammd` computes we now need to turn off bit 31, and calls the shell script `~/.appmodmaps/31.undo` to undo those keyboard changes.

If we terminate `ammd` while bits are still set, `ammd` will call the "undo" primitive for each set bit to reset your system back to the original state. In this case, if bit 31 was still on, then it will call `~/.appmodmaps/31.undo` to undo those keyboard changes.

To add new bits, just put them into `~/.appmodmaps/config.h` and then create corresponding "do" and "undo" primitives. For example, to use bit 3, just `|(1<<3)` (or the `#define` you set for that bit) with the bit value in the required mapping entries in `~/.appmodmaps/config.h`, and create `~/.appmodmaps/3.do` and `~/.appmodmaps/3.undo`. The shell scripts for the "do" and "undo" primitives should be mirror images: what one does, the other should exactly undo, and vice versa, without side effects if possible. Make sure these scripts are executable (`chmod +x`). Then run a `make` to rebuild `ammd` and stop and restart `ammd` if it was already running.

For local use, to avoid conflict with future built-in primitives in future versions of **appmodmap**, we advise starting from bit 0 and working your way up for local primitives you implement.

To add new application window name types, obtain the window name from the `XClassHint` using `winclass` (or any similar tool), and then insert a new entry into the mappings in `~/.appmodmaps/config.h` with the needed bit value. Then run a `make` to rebuild `ammd` and stop and restart `ammd` if it was already running.

## To-do

Make a friendlier way of configuring the daemon, maybe even allowing live changes. However, this would require coming up with some sort of configuration file format.

Include additional typical system primitives. (If you have some ideas, file a PR.)

We use the window name as given in the class hint because that's usually what you want, but sometimes we actually do want the window class. (And the window name given in the class hint doesn't usually match what's in the titlebar, which can be confusing.)

## License

**appmodmap** is offered to you under the BSD license.

Copyright (c) 2018, Cameron Kaiser.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * The names of its contributors may not be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


