Synchrono
==============

## Project Overview

Synchrono is an autonomous vehicle simulation project developed by the Simulation Based Engineering Lab ([SBEL](https://sbel.wisc.edu/)) at the University of Wisconsin-Madison. 
This simulation framework is based around the ability to simulation one or multiple robots, autonomous vehicles, or other autonomous agents together in a unified framework.
Our current motivation comes from autonomous vehicle testing, so many of our demos and examples will draw on autonomous vehicles, but since the simulation is backed by Chrono, we can support any virtual environment.

Our goal is to extend Project Chrono's physics simulation into the multi-agent realm, specifically the multi-agent realm where the dynamics of the agents are not strongly coupled. 
For a simulation of say two robots underwater, where the currents created by one robot will impact the dynamics of the other robot, you are better off in Chrono, where the robots and the fluid will be contained together in a single Chrono model and where the dynamics will be jointly simulated.
Synchrono is suited for scenarios where the dynamics of each agent are important individually, but not in the way that they interact with each other. 
This is the case for autonomous vehicles where, barring a collision, the dynamics of one vehicle will not impact any other.
Synchrono synchronizes the motion of all agents, but allows their dynamics to be distributed across computing nodes rather than including all dynamics in one monolithic simulation.

## Table of Contents

- [General Synchrono Process](#General-Synchrono-Process)
- [Building and Running](#Building-and-Running)
- [Implementation Specific Details](#Implementation-Specific-Details)
- [Other](#Other)

## General Synchrono Process

0. Environment setup
   - Find an environment mesh: e.g. surface data for the specific roads or terrain your agent will move on)
   - Obtain agents: Chrono::Vehicle provides many vehicle models but you can of course create your own in Chrono
   - Define environment parameters: How many agents? What types are they? Where will they be? How long will the simulation run for? Will they have sensors? etc...
1. Consult the agent's brain
   - Each agent has a brain that tells it what to do, provided some inputs from the world around it.
     For a vehicle the outputs will be the thrust, braking and steering driving parameters, and the input can be anything accessible to simulation.
     There may be messages received from e.g. a stoplight.
     There may be data received from sensors e.g. GPS coordinates, lidar point clouds
   - Brains can be whatever the user would like and the goal of the Synchrono project is not to develop any particular brain, but rather to develop an environment where others can test their algorithms.
   - Synchrono does currently provide some simple brains such as a brain that will follow a path defined by GPS coordinates and maintain an appropriate following distance to any vehicles ahead of it based on lidar data.
2. Run Chrono simulation
   - Each computing node starts up starts running a Chrono simulation for the agent(s) it is responsible for.
     It will run this simulation until it gets to the next "heartbeat" or synchronization timestep.
     This step 1 is the same as a normal Chrono simulation, iterating a particular system's dynamics for a particular number of timesteps
   - For a vehicle this would be taking the thrust, braking, steering and computing how much the wheels will turn, how far the center of mass will travel, etc...
   - Further Chrono specific details can be found on the Project Chrono [website](http://www.projectchrono.org/).
3. Send state data to controller
   - Having computed its state at the particular heartbeat, each agent now communicates its state to the central controller so that the control can synchronize all agents
   - For a vehicle the state is just the locations and rotation angle of each of the wheels, for a stoplight it may just be the current color of the light, for a complex robot the state may be a large collection of the positions and rotations of each component of the robot.
4. Synchronize all agents
   - The central controller collects state data from every agent and sends it to every other agent in the simulation
   - Each agent will create a "zombie" version of all other agents.
     The dynamics for this zombie agent will not be simulated, it will just be placed in the world as a visual mesh so that the real agent can compute accurate sensor data.
5. Rinse, wash, repeat
   - Each Chrono simulation has a timestep and each heartbeat has a timestep, so repeatedly doing steps 1-4 advances the simulation through time

## Implementation Specific Details

### Multi-Agent

### Physics

The physics for Synchrono is based on the physics of Chrono.
For Vehicles this means Chrono::Vehicle's physics.

### Sensing

### V2X Communication

## Building and Running

#### Cloning
You should use git to retrieve the project, the develop branch is the most stable branch (although even that is relative in this early stage of the project).

Synchrono has a specific version of Chrono as a dependency and embeds it as a git submodule and also includes [FlatBuffers](https://github.com/google/flatbuffers) as a dependency in the same way. 
To bring in these submodules it is recommended to clone as \
`$git clone https://github.com/uwsbel/synchrono --recursive ` \
Another option is to clone and then run \
`$git submodule init `\
`$git submodule update`

#### Dependencies
 - CMake __: ()
 - OpenMPI 4: [OpenMPI](https://www.open-mpi.org/software/ompi/v4.0/) 

 You should already have Eigen and Irrlicht for your Chrono install, but Synchrono needs them as well
 - Eigen 3.3.7: [Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page)
 - Irrlicht 1.8.4: [Irrlicht](http://irrlicht.sourceforge.net/?page_id=10)

 Sensor dependencies:
 - OptiX 6.0.0: [OptiX](https://developer.nvidia.com/designworks/optix/downloads/6.0.0/linux64) (You will need to create an account to download)

 On Wilson specifically, most of the dependencies are already installed.
 The only things you should have to do are `module load mpi` (feel free to leave this in your `.bashrc`) and download OptiX (it cannot be group installed because of their licensing that requires you to create an account).

 Note that you also need a mesh for the Park St. demos.
 If you are a lab member, talk to Jay (or Asher) to have this shared with you.

#### MPI
For most systems (Linux, Mac OS X), you can use OpenMPI. For Windows there are two main options:
- Intel MPI: [Intel MPI](https://software.intel.com/en-us/mpi-library/choose-download/windows)
- MS-MPI: [MS-MPI](https://docs.microsoft.com/en-us/message-passing-interface/microsoft-mpi)

One consideration is the version of MPI that is supported. Intel MPI is MPI 3.1 compliant whereas MS-MPI is only MPI 2.2 compliant. This may or may not make a difference in your application. However future Synchrono development will not make special effort to support MPI 2.2.

#### Build chrono
Before building Sychrono, you must build the chrono submodule. To do this, it is recommended to run the following commands in Linux (starting from inside the cloned repository):

`$mkdir chrono/chrono-dev/build`\
`$cd chrono/chrono-dev/build`\
`$ccmake ..`

When running CMake, the following modules should be enabled: **Required:** Vehicle, Irrlicht **Optional:** Sensor

When you use Chrono::Sensor, you'll need many OptiX CMake variables set correctly.
Setting `OptiX_INSTALL_DIR` to the top-level directory you got from your OptiX install (e.g. `NVIDIA-OptiX-SDK-6.0.0-linux64`) is enough of a hint for CMake to fill out the rest automatically.
It is normal to get a CMake error about multiple OpenGL versions, you can safely ignore this.

Also note that on CentOS (i.e. Wilson), the flags `MPI_CXX_COMPILE_OPTIONS` and `MPI_C_COMPILE_OPTIONS` get incorrectly set, manually set them both to just `-pthread`.

With these flags in place CMake should complete properly and you can now build Chrono

`$make`

#### Build flatbuffers

You must also build Flatbuffers, following the instructions shown [here](https://google.github.io/flatbuffers/flatbuffers_guide_building.html).
These boil down to your particular flavour of CMake:

`$cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release`\
`$cmake -G "Visual Studio 10" -DCMAKE_BUILD_TYPE=Release`\
`$cmake -G "Xcode" -DCMAKE_BUILD_TYPE=Release`

Followed by make

`$make`

Note that flatbuffers gets built in place so you aren't creating an additional build sub-directory.
You should be able to run `flattests` to confirm that the build worked as expected

#### Running

The Park St. demos (`demo_MPI_parkst`) are based on a private mesh of the Park St. area. 
The mesh cannot be shared on this repository but if you are an SBEL member and Synchrono member, ask Jay (or Asher) to get access to it.
Synchrono will look for it at `/home/<user>/Continental/MeshExport`, but you can also change the CMake flags `PARKST_COL_MESH_PATH` and `PARKST_VIS_MESH_PATH`.

You should be able to run an MPI demo on four nodes with `mpiexec -n 4 ../path/to/demo` or `mpirun -n 4 ../path/to/demo`. 
We use node 0 as a controller and node 1 is a traffic light in some demos so `n = 4` likely gets you two sedans.
Also there seems to be a conflict between CUDA and OpenMPI that causes immediate segfaults. 
Some info about the bug is [here](https://github.com/open-mpi/ompi/issues/6789).
You can get around it by appending `--mca btl ^uct` to your call to run the program, so `mpiexec -n 4 ../path/to/demo --mca btl ^uct`.
We agree that this is pretty annoying so it is on our list to find a better solution and better understand the problem.

_Caution: the below is likely not entirely accurate, Jay is working on a re-write to be released late January/early February_

On Windows specifically, you'll need some CMake variables to be set correctly. For all of these variables, set both the CXX and C variation of them (e.g. both MPI_CXX_LIB_NAMES and MPI_C_LIB_NAMES). Example values are included for each variable, but they may vary depending on your specific system setup
- MPI_CXX_ADDITIONAL_INCLUDE_DIRS - `C:/Program Files (x86)/IntelSWTools/compilers_and_libraries_2019.5.281/windows/mpi/intel64/include`
- MPI_CXX_COMPILER - `C:/Program Files (x86)/IntelSWTools/compilers_and_libraries_2019.5.281/windows/mpi/intel64/bin/mpicl.bat`
- MPI_CXX_HEADER_DIR - `C:/Program Files (x86)/IntelSWTools/mpi/2019.5.281/intel64/include/`
- MPI_CXX_LIB_NAMES - set to `impi` if using Intel MPI or `msmpi` if using MS-MPI

Intel MPI specific:
- MPI_impi_LIBRARY - `C:/Program Files (x86)/IntelSWTools/compilers_and_libraries_2019.5.281/windows/mpi/intel64/lib/release/impi.lib`
- MPI_impicxx_LIBRARY - `C:/Program Files (x86)/IntelSWTools/compilers_and_libraries_2019.5.281/windows/mpi/intel64/lib/impicxx.lib`

MS-MPI specific, most of these can be set automatically by running `set MSMPI` in `cmd`.
- MPI_msmpi_LIBRARY - `C:/Program Files (x86)/Microsoft SDKs/MPI/Lib/x64/msmpi.lib`
- MSMPI_BIN
- MSMPI_INC
- MSMPI_LIB32
- MSMPI_LIB64

## Other
Contact Jay Taves ([jtaves@wisc.edu](mailto:jtaves@wisc.edu)) for any complaints or concerns about Synchrono in general

Contact Asher Elmquist ([amelmquist@wisc.edu](mailto:amelmquist@wisc.edu)) if you have any issues building the specific version of Chrono. Note: the chrono_sensor module will require an NVIDIA GPU Maxwell or newer and will bring in extra dependencies.