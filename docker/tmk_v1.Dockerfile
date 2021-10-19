FROM debian:bullseye

USER root

RUN \
    echo "deb http://deb.debian.org/debian/                 bullseye           main non-free contrib" > /etc/apt/sources.list \
    && echo "deb http://security.debian.org/debian-security bullseye-security  main non-free contrib" >> /etc/apt/sources.list \
    && echo "deb http://deb.debian.org/debian/              bullseye-updates   main non-free contrib" >> /etc/apt/sources.list \
    && echo "deb http://deb.debian.org/debian/              bullseye-backports main non-free contrib"  >> /etc/apt/sources.list \
    && apt-get update \
    && apt-get install -y tzdata \
    && apt-get install -y --fix-missing \
        bash sudo ca-certificates locales-all locales mc vim nano less \
        apt-utils aptitude numactl procps net-tools cpuinfo \
        git git-core gnupg python3 python3-kerberos repo \
        curl wget unzip zip pbzip2 file kmod \
        build-essential quilt gawk m4 autoconf automake autopoint libtool pkgconf libmpfr-dev \
        flex bison bc gcc-multilib g++-multilib gperf libgoogle-glog-dev libgoogle-glog-doc libgoogle-glog0v5\
        net-tools iproute2 numactl procps libboost-all-dev libopencv-dev python3-opencv nlohmann-json3-dev libgtest-dev libceres-dev\
        libssl-dev zlib1g-dev libpcre++-dev libncurses5-dev libpq-dev libpqxx-dev postgresql-server-dev-all\
    && apt-get install -y --no-install-recommends cmake ccache \
    && apt clean \
    && echo "dash dash/sh boolean false" | debconf-set-selections \
    && DEBIAN_FRONTEND=noninteractive dpkg-reconfigure dash \
    && echo en_US UTF-8 > /etc/locale.gen && locale-gen \
    && echo export LANG=en_US.UTF-8 > /etc/profile.d/utf8.sh \
    && chmod +x /etc/profile.d/utf8.sh

CMD [ "/bin/bash" ]
