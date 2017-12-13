ARG COMPILER

FROM registry.gitlab.com/offa/docker-images/${COMPILER}:stable

RUN mkdir deps && cd deps && \
    git clone --depth=1 https://github.com/google/googletest.git gtest && \
    cd gtest && mkdir build && cd build && \
    case ${CXX} in clang* ) \
        export CXXFLAGS="-stdlib=libc++" && \
        echo "Installation of cxxabi.h and __cxxabi_config.h" && \
        CXX_ABIURL=https://raw.githubusercontent.com/llvm-mirror/libcxxabi/master/include/ && \
        curl -sSL ${CXX_ABIURL}/cxxabi.h -o /usr/include/c++/v1/cxxabi.h && \
        curl -sSL ${CXX_ABIURL}/__cxxabi_config.h -o /usr/include/c++/v1/__cxxabi_config.h; \
    esac; \
    cmake .. && make && make install && \
    cd ../.. && \
    cd .. && rm -rf deps

