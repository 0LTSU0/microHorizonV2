# script for building in linux (used by GHA)

mkdir build
cd build

#note to self: DCMAKE_INSTALL_PREFIX needed to override some install paths from thirdparty components
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../install ..
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

cmake --install . --prefix ../install

exit 0
