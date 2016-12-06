#include <QDebug>
#include "../src/pvp/dwpvcam.h"

int main(int argc, char **argv) {
    qDebug() << "Hello world";
    uns16 vsn;
    if (pl_pvcam_get_ver(&vsn)) {
        qDebug() << "Version:" << vsn;
    } else {
        qDebug() << "No version";
    }
    return 0;
}
