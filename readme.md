# ptusa_main #

[![Open Hub Project Status](
    https://www.openhub.net/p/ptusa_main/widgets/project_thin_badge.gif)](
        https://www.openhub.net/p/ptusa_main/)
[![GitHub Actions CI](
    https://github.com/sav-oss-org/ptusa_main/actions/workflows/cmake.yml/badge.svg)](
        https://github.com/sav-oss-org/ptusa_main/actions/workflows/cmake.yml)
[![codecov](
    https://codecov.io/gh/sav-oss-org/ptusa_main/branch/master/graph/badge.svg?token=G1BK5054IJ)](
        https://codecov.io/gh/sav-oss-org/ptusa_main)

[![Downloads](
    https://img.shields.io/github/downloads/sav-oss-org/ptusa_main/total.svg)](
        https://github.com/sav-oss-org/ptusa_main/releases/)

Russian readme is [here](docs/russian_readme/readme.md).

Program **``ptusa_main``** directly controls technological process.

## Content ##

1. [General system architecture](#general-system-architecture)
2. [Common description](#common-description)
3. [How to build the project](#how-to-build-the-project )
4. [Using PLCnext Technology C++ Toolchain in
Windows](#using-plcnext-technology-c-toolchain-in-windows)
5. [How to run a project](#how-to-run-a-project)
6. [Documentation](#documentation)

## General system architecture ##

General system architecture of the automated process control system (**APCS**)
is shown below:

![System architecture](./docs/readme_images/system_architecture_en.png)

## Common description ##

Controller logic implemented by Lua-script. At runtime, control is passed
through special Lua functions that are responsible for the various stages of
the control program.

+ The general schema of controller program work (in
[Drakon](https://drakonhub.com) diagram language, made with
[Mermaid](https://mermaid.js.org/)):

![Control program](./docs/drakon_diagram/images/control_program_en.svg)

+ Execution of the technological process:

``` mermaid
%%{ init: {'theme': 'neutral'} }%%

flowchart TD
    A([<b>2 Process execution</b>]) --- B[/<p style='width:150px'> For each unit\]
    B --- C[[3 Execution of the unit <br/> procedure]]
    C --- D[\<p style='width:150px'> Next unit/]
    D --- E([End])

```

``` mermaid
%%{ init: {'theme': 'neutral'} }%%

flowchart TD
    A([<b>3 Execution <br/> of the unit procedure</b>]) --- B[/<p style='width:150px'> For each operation\]
    B --- C[[<p style='width:150px'>4 Execution <br/> of the operation]]
    C --- D[\<p style='width:150px'> Next operation/]
    D --- E([End])

```

![Operation execution](./docs/drakon_diagram/images/operation_en.svg)

``` mermaid
%%{ init: {'theme': 'neutral'} }%%

flowchart TD
    A([<b>5 Execution of the steps</b>]) --- A1[[6 Execution of the <br/> main step]]
    A1 --- B[/<p style='width:150px'> For each extra step\]
    B --- C[[6 Execution of the step]]
    C --- D[\<p style='width:150px'> Next extra step/]
    D --- E([End])

```

``` mermaid
%%{ init: {'theme': 'neutral'} }%%

flowchart TD
    A([<b>6 Execution of the step</b>]) --- B[/<p style='width:150px'> For each step\]
    B --- C[[7 Execution of the action]]
    C --- D[\<p style='width:150px'> Next action/]
    D --- E([End])

```

``` mermaid
%%{ init: {'theme': 'neutral'} }%%

flowchart TD
    A([<b>7 Execution of the action</b>]) --- B(<p style='width:150px'>Action logic)
    B --- E([End])

```

+ When operation is turned on (off) happens next:

![Setting mode](./docs/readme_images/tech_object__set_mode_en.png)

+ During the execution of command happens next:

![Execution of command](./docs/readme_images/tech_object__exec_cmd_en.png)

## How to build the project ##

To select and build multiple CMake configurations with the `install` target, run
`python tools/build.py`. See the [build script guide](tools/build.md) for saved
selections, parallel jobs, and command previews.

You could clone repository by next command:
`git clone --recurse-submodules
https://github.com/sav-oss-org/ptusa_main.git`

or, if you are cloned without initializing and updating submodules, try next:
`git submodule update --init --recursive`

### win ###

Requirements:

+ Qt Creator

You could open file CMakeLists.txt cloned repository in Qt.

Or clone repository in Qt:
    New -> Import Project -> Git Clone
    specify the repository link and check the box Recursive

+ Microsoft Visual Studio Community 2022

Before starting the project, you need to make sure that you have installed C++
CMake tools for Windows.
C++ CMake tools for Windows are installed using:

`Visual Studio Installer -> Development of classic applications in C++ -> C++
CMake tools for Windows`

Detailed installation information can be obtained [here](
    https://docs.microsoft.com/en-us/cpp/build/cmake-projects-in-visual-studio?view=msvc-160).
The CMake version required for the build is 3.31 and above.

You should open (in Visual Studio) root directory. This you get a CMake based
project.
If an error occurs after startup, then you should clear the CMake cache
`Project -> Delete cache and perform reconfiguration`.

### Linux ###

Eclipse IDE with CMake based project on Linux.

## Using PLCnext Technology C++ Toolchain in Windows ##

To configure CMake for SDKs installed on the current machine, run from the
repository root (Python 3.9+ and `plcncli` must be on `PATH`):

```shell
python tools/configure_plcnext_presets.py
cmake --list-presets
```

The script runs `plcncli get sdks` and generates `CMakeUserPresets.json` using
the reported SDK paths and target versions. Select a **Local AXCF...** preset
in Visual Studio, or use the configure/build commands printed by the script.
On Windows it also discovers `make.exe` inside each SDK; on Linux it uses Ninja.
Each SDK target gets a separate build directory. Run the script again after
installing, moving or removing SDKs. It replaces its own generated presets and
preserves other user presets. The local file is ignored by Git; shared
`CMakePresets.json` is not modified. If an SDK is inaccessible or the CLI fails,
the existing local file is preserved.

Download the latest versions from the Phoenix Contact
[website](https://www.phoenixcontact.com/products) (for example, from the **AXC
F 2152** area) ```PLCnext Technology C++ toolchain for Windows```(current is
2023.0 LTS) and ```sdk for Windows 64 bit``` (current is 2022.0.4 LTS), in the
section Downloads->Software $-$ <http://www.phoenixcontact.com/qr/2404267/softw>.

Navigate to the folder where downloads are located (typical path
`%userprofile%/Downloads`), unzip the archive
(`SDK_2021.9_Windows_AXC_F_2152.tar.xz.zip`).

### Installing (updating) the SDK ###

Navigate to the folder where downloads are previously unzipped, Call the CLI in
the console using the following command:

```ps
plcncli.exe install sdk –d [installation path] –p [path to archive file]
```

>If you install several SDKs, Phoenix Contact recommends to use the "target
name/firmware version" folder structure.

Also your installation path must match with path in CMake.

E.g.:

```ps
plcncli.exe install sdk -d C:\CLI\sdks\AXCF2152\2022_0\ -p pxc-glibc-x86_64-mingw32-axcf2152-image-mingw-cortexa9t2hf-neon-axcf2152-toolchain-2022.0.tar.xz
```

>The SDK is specified to the controller. The full list of controllers can be
found on the PHOENIX CONTACT International site ([Home > Products > PLCs and
I/O systems > PLCnext Control > Product list PLCnext Technology
components](https://www.phoenixcontact.com/online/portal/pi?1dmy&urile=wcm%3apath%3a/pien/web/main/products/list_pages/PLCnext_technology_components_P-21-14-01/f77f0eb0-2a70-40c3-8679-7df2450e26db)).

## How to run a project ##

To run demo project you should use this command line:

### windows ###

+ After building and deploying *`windows-default`* cmake-configuration:

```cmd
    bin\install\windows-default\Release\ptusa_main.exe ^
    --path demo_projects/T1-PLCnext-Demo/ ^
    --sys_path demo_projects/T1-PLCnext-Demo/sys/ ^
    --debug ^
    demo_projects/T1-PLCnext-Demo/main.plua
```

+ Launching a project in Microsoft Visual Studio Community 2022: `Select a
launch item -> ptusa_main.exe`.

## Documentation ##

[Конфигурационный файл standalone-приложения и время цикла](docs/russian_readme/readme.md#необязательный-конфигурационный-файл-standalone-приложения).

### Device Documentation ###

Documentation for supported devices can be found in the
[docs/devices](docs/devices/) directory.

Currently documented devices:

+ [TM311](docs/devices/TM311.md) - Endress+Hauser TM311 IO-Link Temperature
  Sensor (Technical Documentation)
+ [TM311 User Guide](docs/devices/TM311_user_guide.md) - User Guide for APCS
  Engineers (in Russian)

### Other Documentation ###

+ [Testing Strategy](docs/test/readme.md) - Information about testing approach
+ [Code Style Guide](docs/style.md) - Coding standards and conventions
+ [Contributing Guide](docs/CONTRIBUTING.md) - How to contribute to the project
