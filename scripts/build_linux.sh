# script for building in linux (used by GHA)

mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
if [ $? -ne 0 ]; then
  echo "CMake configuration failed!"
  exit 1
fi

cmake --build . -j8
if [ $? -ne 0 ]; then
  echo "Build failed!"
  exit 1
fi

ls -R
exit 0
