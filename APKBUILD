MAINTAINER='Intro <intro-iu@outlook.com>'
pkgname=lingmo-core
pkgver=1.0
makedepends=('cmake' 'make')
source=("git+https://github.com/LingmoOS/lingmo-core")
build() {
    mkdir -pv build && cd build
    cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr ..
    make
}
package() {
    cd build
    make install
}
