# Clone repo with all submodules
# git clone --recursive https://github.com/uwsbel/synchrono.git && cd synchrono

# Install all necessary packages using brew [https://brew.sh/]
if ! command -v bash > /dev/null; then
  /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install.sh)"
  exit 1
else 
  brew install irrlicht eigen boost cmake ccache open-mpi
fi

# You will need the xcode command line tools
if ! command -v xcode-select > /dev/null; then
  echo "You need to install xcode command line tools. Please run xcode-select --install and try again."
  exit 1
fi

# Build chrono
cd chrono/chrono-dev
mkdir build && cd build
cmake \
  -DCMAKE_BUILD_TYPE:STRING=Release \
  -DCMAKE_C_COMPILER=$(which clang) \
  -DCMAKE_CXX_COMPILER=$(which clang++) \
  -DENABLE_MODULE_VEHICLE:BOOL=ON \
  -DENABLE_MODULE_IRRLICHT:BOOL=ON \
  .. \
  && make -j8
cd ../../

# Build flatbuffers
cd flatbuffers
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release && make -j8
cd ../

# Build synchrono
mkdir build && cd build

