# Build library and examples
export PATH=/home/debian/joseph/cmake-3.27.3-linux-x86_64/bin:$PATH
export PATH=/home/debian/joseph/llvm/llvm-install/bin:$PATH

NLOPT_DIR=/home/debian/joseph/nlopt/install
RABBITMQ_DIR=/home/debian/joseph/rabbitmq-c/install
JANSSON_DIR=/home/debian/joseph/jansson/install

mkdir build
cd build
cmake -DNLOPT_DIR=$NLOPT_DIR -DRABBITMQ_DIR=$RABBITMQ_DIR -DJANSSON_DIR=$JANSSON_DIR -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ ..
make -j 4
