FROM ubuntu:focal
MAINTAINER A. S. Budden (abudden@gmail.com)
RUN apt-get update && apt-get install -y \
	build-essential \
	locales \
	mercurial \
	python3 \
	python3-pip \
	wget
RUN useradd -s /bin/bash -m -u 1000 al
RUN locale-gen en_GB.UTF-8
RUN chown -R al /home/al
RUN python3 -m pip install mbed-cli
WORKDIR /home/al
USER al
RUN wget --content-disposition "https://developer.arm.com/-/media/Files/downloads/gnu-rm/8-2019q3/RC1.1/gcc-arm-none-eabi-8-2019-q3-update-linux.tar.bz2?revision=c34d758a-be0c-476e-a2de-af8c6e16a8a2?product=GNU%20Arm%20Embedded%20Toolchain,64-bit,,Linux,8-2019-q3-update"
RUN tar -xf gcc-arm-none-eabi*.tar.bz2
RUN mv gcc-arm-none*/ gcc-arm-none
ENV LC_ALL en_GB.UTF-8
WORKDIR /home/al/project
ENTRYPOINT ["/bin/bash", "docker/buildscript.sh"]
CMD ["--all"]
