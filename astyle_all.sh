#!/usr/bin/env sh
astyle --options=.astyle --recursive "include/*.h" "src/*.h" "src/*.cpp" "test_app/*.h" "test_app/*.cpp"
