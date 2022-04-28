i3-tools
=======
I3wm utility for switching focus between current and previous focused windows.

## Requirements

* cmake (>= 3.10)
* C++20 compiler
* sigc++ 2.0 ([i3ipc++](https://github.com/drmgc/i3ipcpp) dependency)
* jsoncpp ([i3ipc++](https://github.com/drmgc/i3ipcpp) dependency)

## Build

```bash
git clone https://github.com/dinAlt/i3-tools.git --recursive
cd i3-tools
mkdir build && cd build
cmake ./..
cmake --build .
```

## Usage

```bash
./i3-tools -h
```

```
usage: i3-tools [-h] [--switch-window]

Focus previos focused window for i3wm.

This app should be launched with i3wm without options
and then use you may use it with --switch-window option.

options:
  -h, help         show help and exit
  --switch-window  focus previos focused window
```
