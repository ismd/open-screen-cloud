#!/bin/bash
VERSION=$1
VERSION=${VERSION#"v"}

docker --version

# AUR
docker build --build-arg VERSION=$VERSION $TRAVIS_BUILD_DIR/deploy/arch
