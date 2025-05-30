./install/bin/test_microHorizon
if [ $? -ne 0 ]; then
  echo "Tests failed!"
  exit 1
fi
