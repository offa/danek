ARG COMPILER

FROM registry.gitlab.com/offa/docker-images/${COMPILER}:stable

RUN mkdir deps && cd deps && \
    git clone --depth=1 https://github.com/google/googletest.git gtest && \
    cd gtest && mkdir build && cd build && \
    case ${CXX} in clang* ) export CXXFLAGS="-stdlib=libc++"; esac; \
    cmake .. && make && make install && \
    cd ../.. && \
    cd .. && rm -rf deps

