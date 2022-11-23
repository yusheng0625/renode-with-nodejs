## Build and execute tests

# Get the base Ubuntu image from Docker Hub
FROM ubuntu:20.04

# Update the base image and install base build environment
RUN apt clean  \
    && apt-get update \
    && apt-get install -y curl git openssh-client vim build-essential python3-distutils yarn \
    && curl -sL https://deb.nodesource.com/setup_17.x | bash - \
    && apt-get update \
    && apt-get install -y nodejs

# Install additional ngspice build dependencies
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get install -y flex bison
RUN apt-get install -y autoconf automake libtool texinfo libreadline8

# Install utilities
RUN npm install pm2 -g

# Push target build source to container
COPY /src /usr/src/sahara/src
COPY /__test__ /usr/src/sahara/__test__

COPY /*.gyp /usr/src/sahara/
COPY /*.js /usr/src/sahara/
COPY /*.json /usr/src/sahara/
COPY /*.sh /usr/src/sahara/
COPY /*.c /usr/src/sahara/
COPY /*.h /usr/src/sahara/
COPY /*.md /usr/src/sahara/

# Build
WORKDIR /usr/src/sahara
RUN npm install

# Entrypoint for testing
CMD [ "npm", "test" ]
