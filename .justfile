[private]
default:
  just --list

# compile project
compile:
  mkdir -p build
  cmake -S . -B ./build
  cmake --build ./build --parallel

# run project
run:
  [ -x ./build/container ] || just compile
  [ -x ./build/container ] && ./build/container
