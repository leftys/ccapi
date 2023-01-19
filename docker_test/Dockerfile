# This can be (slowly) crosscompiled on x86 processors using https://www.stereolabs.com/docs/docker/building-arm-container-on-x86/

# ARG TARGETPLATFORM
# FROM arm64v8/alpine:latest as build
FROM alpine:latest as build

LABEL description="Build container - ccapi"

RUN apk update && apk add --no-cache \
	autoconf build-base binutils cmake file gcc g++ git libgcc libtool make linux-headers openssl-dev libcrypto1.1
RUN mkdir -p docker_test/out
ADD include /src/include
ADD dependency /src/dependency
COPY docker_test/CMakeLists.txt /src/docker_test/
COPY docker_test/cotire.cmake /src/docker_test/
RUN touch /src/docker_test/main.cpp
# ADD docker_test/main.cpp /src/docker_test/
WORKDIR /src/docker_test/out
RUN cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON ..
ADD docker_test/main.cpp /src/docker_test/
RUN --mount=type=cache,target=/src/docker_test/out/CMakeFiles,id=cache-amd64 \
    cmake --build .

# FROM arm64v8/alpine:latest as runtime
FROM alpine:latest as runtime

LABEL description="Run container - ccapi"

RUN apk update && apk add --no-cache libstdc++

RUN mkdir /src
COPY --from=build /src/docker_test/out/market_making /src/

WORKDIR /src/

CMD ./market_making
# ENTRYPOINT ["./market_making"]
