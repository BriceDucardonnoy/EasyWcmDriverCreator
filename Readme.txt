-- Compile for Linux
qmake
make
make distclean
-- Compile for Windows
-- 1st time
** MXE Installation **
git clone https://github.com/mxe/mxe.git
** cf http://mxe.cc/#requirements **
sudo apt-get install \
    autoconf automake autopoint bash bison bzip2 flex gettext\
    git g++ gperf intltool libffi-dev libgdk-pixbuf2.0-dev \
    libtool libltdl-dev libssl-dev libxml-parser-perl make \
    openssl p7zip-full patch perl pkg-config python ruby scons \
    sed unzip wget xz-utils g++-multilib libc6-dev-i386 libtool-bin
cd mxe
make qtbase
-- compile normal time
export PATH=$PATH:/home/$USER/git/mxe/usr/bin
i686-w64-mingw32.static-qmake-qt5
make
make distclean