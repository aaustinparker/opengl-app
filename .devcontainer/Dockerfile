
FROM mcr.microsoft.com/devcontainers/cpp:1-debian-11

ARG REINSTALL_CMAKE_VERSION_FROM_SOURCE="none"

# Optionally install the cmake for vcpkg
COPY ./reinstall-cmake.sh /tmp/

RUN if [ "${REINSTALL_CMAKE_VERSION_FROM_SOURCE}" != "none" ]; then \
        chmod +x /tmp/reinstall-cmake.sh && /tmp/reinstall-cmake.sh ${REINSTALL_CMAKE_VERSION_FROM_SOURCE}; \
    fi \
    && rm -f /tmp/reinstall-cmake.sh

# [Optional] Uncomment this section to install additional vcpkg ports.
# RUN su vscode -c "${VCPKG_ROOT}/vcpkg install <your-port-name-here>"


WORKDIR /workspaces

# [Optional] Uncomment this section to install additional packages.
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
     && apt-get -y install --no-install-recommends mesa-common-dev libglu1-mesa-dev mesa-utils freeglut3-dev libwayland-dev libxkbcommon-dev xorg-dev libglew-dev libglfw3-dev libglm-dev libao-dev libmpg123-dev


RUN sudo curl -L "https://github.com/glfw/glfw/archive/refs/tags/3.4.tar.gz" -o "glfw.tar.gz" \
    && sudo tar zxvf ./glfw.tar.gz \
    && cd glfw-3.4/ \
    && sudo cmake . \
    && sudo make \
    && sudo make install

COPY external/include /usr/local/include