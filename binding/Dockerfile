# Arm can be (slowly) crosscompiled on x86 processors using https://www.stereolabs.com/docs/docker/building-arm-container-on-x86/
# Run example:
# DOCKER_BUILDKIT=1 docker build -f binding/Dockerfile --build-arg TARGETPLATFORM=arm64v8 -t leftys/ccapi-python:latest --progress=plain --load .
# docker run --privileged --rm --entrypoint cat leftys/ccapi-python:latest out/python/packaging/0.2.3/dist/ccapi-0.2.3-cp311-cp311-linux_x86_64.whl >ccapi-0.2.3-cp311-cp311-linux_x86_64.whl
# sudo docker create leftys/ccapi-python:latest
# sudo docker cp 4613f5ee85604a6f712f9609db8fcbaa66d665aa5dbbdeff4b3f2157d0c7cc8e:/src/packaging/0.2.3/dist/ccapi-0.2.3-cp311-cp311-linux_aarch64.whl - >ccapi-0.2.3-cp311-cp311-linux_aarch64.whl

ARG TARGETPLATFORM
FROM ${TARGETPLATFORM}/debian:bookworm
# as build

#LABEL description="Build container - ccapi"

RUN apt update && apt install --no-install-recommends -y gnupg
RUN echo "deb http://ppa.launchpad.net/deadsnakes/ppa/ubuntu jammy main" >> /etc/apt/sources.list.d/deadsnakes.list && \
	apt-key adv --keyserver keyserver.ubuntu.com --recv-keys BA6932366A755776

# Build requrements
RUN apt update && apt install -y --no-install-recommends \
  build-essential cmake libssl-dev swig python3.13-dev python3.13-venv zlib1g-dev ccache
RUN update-alternatives --install /usr/bin/python python /usr/bin/python3.13 1
RUN python -m ensurepip && python -m pip install setuptools

RUN mkdir -p /src/binding/out
ADD include /src/include
ADD binding /src/binding
WORKDIR /src/binding/out
ENV CCACHE_DIR=/src/binding/out/ccache
ENV THREADS=2
RUN --mount=type=cache,target=/src/binding/out/,id=binding-cache-${TARGETPLATFORM} \
	cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -DBUILD_VERSION=0.2.3 -DPython_EXECUTABLE=/usr/bin/python3.13 \
	-DBUILD_PYTHON:BOOL=ON -DCMAKE_PROJECT_INCLUDE=/src/binding/user_specified_cmake_include.cmake \
	-DCMAKE_CXX_COMPILER_LAUNCHER=ccache -DCMAKE_C_COMPILER_LAUNCHER=ccache .. && \
	cmake --build . -- -j $THREADS && \
	cp -r /src/binding/out/python/packaging/ /src/packaging && \
	ccache -s

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
