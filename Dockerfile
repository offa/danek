ARG COMPILER

FROM registry.gitlab.com/offa/docker-images/${COMPILER}:stable

RUN mkdir deps && cd deps && \
        git clone https://github.com/google/googletest.git && \
        if [ "$(echo ${CXX} | cut -c -5)" = "clang" ]; then \
            export CXXFLAGS="-stdlib=libc++" && \
            echo "Installation of cxxabi.h and __cxxabi_config.h" && \
            CXX_ABIURL=https://raw.githubusercontent.com/llvm-mirror/libcxxabi/master/include/ && \
            curl -sSL ${CXX_ABIURL}/cxxabi.h -o /usr/include/c++/v1/cxxabi.h && \
            curl -sSL ${CXX_ABIURL}/__cxxabi_config.h -o /usr/include/c++/v1/__cxxabi_config.h; \
        fi && \
        cd googletest && \
        # Workaround for #106; use commit prior the broken one
        git checkout 0663ce9^ && \
        mkdir build && cd build && \
        cmake .. && \
        make && make install && \
        cd ../.. && rm -rf deps

