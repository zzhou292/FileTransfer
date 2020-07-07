## Some scripts

Here are some useful scripts for working with Synchrono, so far mostly when you're on Euler.
Loaded modules are specific to the current process, so most of these should be [sourced](https://superuser.com/questions/176783/what-is-the-difference-between-executing-a-bash-script-vs-sourcing-it) rather than just run in bash. 
Essentially `source somescript.sh` not `./somescript.sh`.

- `loadsynmods.sh` will load all modules that are required to build Synchrono.
This is primarily for use on Euler when you would like to test your build.

- `chrono_cmake.sh` will move you from this directory to the Chrono build directory and then run `cmake` with the appropriate flags for Chrono. 
Afterwords you should just be able to run `make` to get Chrono fully build

- `synchrono_cmake.sh` will move you from this directory to the Synchrono build directory and then run `cmake` with the appropriate flags (currently just Release mode).
Again, just run `make` to get Synchrono to fully build

- `runParkst.sh` will run the park st. demo on 6 mpi ranks. 