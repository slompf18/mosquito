pushd $IDF_PATH/tools/unit-test-app
make flash monitor TEST_COMPONENTS='esp32-homekit'
popd