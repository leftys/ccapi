# Arm can be (slowly) crosscompiled on x86 processors using https://www.stereolabs.com/docs/docker/building-arm-container-on-x86/
# Run example:
# DOCKER_BUILDKIT=1 docker build -f binding/Dockerfile --build-arg TARGETPLATFORM=arm64v8 -t leftys/ccapi-python:latest --progress=plain --load .
# docker run --privileged --rm --entrypoint cat leftys/ccapi-python:latest out/python/packaging/0.2.3/dist/ccapi-0.2.3-cp311-cp311-linux_x86_64.whl >ccapi-0.2.3-cp311-cp311-linux_x86_64.whl
# sudo docker create leftys/ccapi-python:latest
# sudo docker cp 4613f5ee85604a6f712f9609db8fcbaa66d665aa5dbbdeff4b3f2157d0c7cc8e:/src/binding/out/python/packaging/0.2.3/dist/ccapi-0.2.3-cp311-cp311-linux_aarch64.whl - >ccapi-0.2.3-cp311-cp311-linux_aarch64.whl

# TODO python 3.12 jeste nefunguje s ppa?!

ARG TARGETPLATFORM
FROM ${TARGETPLATFORM}/debian:bookworm
# as build

#LABEL description="Build container - ccapi"

# Python3.11
RUN apt update && apt install -y software-properties-common python3-launchpadlib
RUN apt-add-repository -P ppa:deadsnakes/ppa

# Build requrements
RUN apt update && apt install -y --no-install-recommends \
  build-essential cmake libssl-dev swig python3.11-dev python3.11-venv python3-pip zlib1g-dev

RUN mkdir -p /src/binding/out
ADD include /src/include
ADD binding /src/binding
WORKDIR /src/binding/out
RUN update-alternatives --install /usr/bin/python python /usr/bin/python3.11 1
# RUN --mount=type=cache,target=/src/binding/out/,id=binding-cache-${TARGETPLATFORM} \
RUN \
	cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DBUILD_VERSION=0.2.3 -DPython_EXECUTABLE=/usr/bin/python3.11 \
	-DBUILD_PYTHON:BOOL=ON -DCMAKE_PROJECT_INCLUDE=/src/binding/user_specified_cmake_include.cmake .. && \
    cmake --build .

# -DPython_EXECUTABLE=/usr/bin/python3.12
# FROM ${TARGETPLATFORM}/debian:bullseye as runtime

# LABEL description="Run container - ccapi"

# RUN apt update && apt install python3.12

# RUN mkdir /src
# COPY --from=build /src/binding/out/* /src/

WORKDIR /src/binding
CMD /bin/bash

# FROM scratch as custom-exporter
# WORKDIR /src/binding
# COPY --from=build out/python/packaging/dist/*.whl .
