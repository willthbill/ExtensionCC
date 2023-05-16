# CG:SHOP 2023 CHALLENGE CODE
- code from the challenge and from benchmarks performed after the end of the challenge
- will be cleaned up in later commits

# Development environment
## Dependencies
* git-lfs
* cmake
* ninja
* make
* gcc and g++
* (nix-shell)
* (direnv)
## Tools
* scripts/startshell.sh starts the nix-shell
* there is a tmuxinator configuration in .tmuxinator.yml
* bin/configure_debug configures cmake for debug
* bin/configure_release configures cmake for release
* bin/build builds c++ libs based on cmake configuration
* bin/clean cleans the c++ build directory
* bin/producezip <directory> produces submission.zip based on a directory of output files in output/solutions/<directory>
* bin/rundev <algorithm> <testset> [nogui] runs the in development mode
* bin/runevaluate <algorithm> <testset> evalues an algorithm on a testcase and puts output into the output directory
* bin/rungenimages <testset> generates image of a given testset
* bin/runinparallel <algorith> <testset> is like rundev but in parallel

# Dependencies
## Python
* cgshop2023-pyutils
* pybind11
* loguru
* matplotlib
* multiprocess
* tabulate

## c++
* cgal at least version 5, and its dependecies (https://doc.cgal.org/latest/Manual/thirdparty.html#title2)
* boost

# Setting up on arch-linux (no git, no nix, no direnv)
```bash
pacman -Sy make cgal cmake gcc ninja python-pip scons openmpi [tmux] [vim]
pip install pybind11 loguru matplotlib multiprocess cgshop2023-pyutils
```
