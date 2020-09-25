ARG COMPILER

FROM registry.gitlab.com/offa/docker-images/${COMPILER}:stable

RUN mkdir deps && cd deps && \
        if [ "$(echo ${CXX} | cut -c -5)" = "clang" ]; then \
            export CXXFLAGS="-stdlib=libc++" && \
        fi && \
        git clone --depth=1 https://github.com/google/googletest.git && \
        cd googletest && \
        mkdir build && cd build && \
        cmake -DCMAKE_CXX_STANDARD=17 .. && \
        make && make install && \
        cd ../.. && rm -rf deps
