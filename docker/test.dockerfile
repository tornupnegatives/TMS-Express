FROM ubuntu:jammy
WORKDIR /workdir

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install -y \
        cmake \
        build-essential \
        git \
        libsndfile1-dev \
        qt6-base-dev \
        qt6-multimedia-dev && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

RUN useradd -m app && \
    chown -R app:app /workdir
USER app

COPY --chown=app:app . .

RUN cmake -B build \
    -DCMAKE_BUILD_TYPE=Debug \
    -DTMSEXPRESS_BUILD_TESTS=ON \
    -DTMSEXPRESS_BUILD_GUI=OFF
RUN cmake --build ./build --config Debug
CMD ["cmake", "--build", "./build", "--target", "test"]
