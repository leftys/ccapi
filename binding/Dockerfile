# Arm can be (slowly) crosscompiled on x86 processors using https://www.stereolabs.com/docs/docker/building-arm-container-on-x86/
# Run example:
# DOCKER_BUILDKIT=1 docker build -f binding/Dockerfile --build-arg TARGETPLATFORM=arm64v8 -t leftys/ccapi-python:latest --progress=plain --output out .
# docker run --privileged --rm --entrypoint cat leftys/ccapi-python:latest out/python/packaging/dist/ccapi-0.2.0-cp311-cp311-linux_x86_64.whl >ccapi-0.2.0-cp311-cp311-linux_x86_64.whl

ARG TARGETPLATFORM
FROM ${TARGETPLATFORM}/debian:bookworm as build

LABEL description="Build container - ccapi"

RUN apt update && apt install -y --no-install-recommends \
  build-essential cmake libssl-dev swig python3.11-dev python3.11-venv python3-pip
RUN mkdir -p /src/binding/out
ADD include /src/include
ADD dependency /src/dependency
ADD binding /src/binding
WORKDIR /src/binding/out
RUN update-alternatives --install /usr/bin/python python /usr/bin/python3.11 1 && pip install wheel setuptools
RUN --mount=type=cache,target=/src/binding/out/CMakeFiles,id=binding-cache2-${TARGETPLATFORM} \
	cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DBUILD_VERSION=0.2.0 -DPython_EXECUTABLE=/usr/bin/python3.11 \
	-DBUILD_PYTHON:BOOL=ON -DCMAKE_PROJECT_INCLUDE=/src/binding/user_specified_cmake_include.cmake .. && \
    cmake --build .

# -DPython_EXECUTABLE=/usr/bin/python3.11
# FROM ${TARGETPLATFORM}/debian:bullseye as runtime

# LABEL description="Run container - ccapi"

# RUN apt update && apt install python3.11

# RUN mkdir /src
# COPY --from=build /src/binding/out/* /src/

WORKDIR /src/binding
CMD /bin/bash

# FROM scratch as custom-exporter
# WORKDIR /src/binding
# COPY --from=build out/python/packaging/dist/*.whl .
