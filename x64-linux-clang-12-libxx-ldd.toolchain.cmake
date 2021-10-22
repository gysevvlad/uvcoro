set(CMAKE_CXX_COMPILER "/usr/bin/clang++-12")
set(CMAKE_CXX_FLAGS "-stdlib=libc++")
set(CMAKE_SHARED_LINKER_FLAGS_CXX "-fuse-ld=lld -rtlib=compiler-rt -lunwind")
set(CMAKE_EXE_LINKER_FLAGS_CXX "-fuse-ld=lld -rtlib=compiler-rt -lunwind")
