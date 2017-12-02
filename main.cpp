#include <bcm2835.h>

#include <QCoreApplication>
#include <QTextStream>

#include "qpiupsmon.h"

int main(int argc, char *argv[]) {
    QTextStream out(stdout);

    // init bcm2835
    if (!bcm2835_init()) {
        out << "bcm2835_init failed. Are you running as root?" << endl;
        return 1;
    }

    // init i2c
    if (!bcm2835_i2c_begin()) {
        out << "bcm2835_i2c_begin failed. Are you running as root?" << endl;
        return 1;
    }

    // set i2c-clock to 100 kHz
    bcm2835_i2c_set_baudrate(100000);

    QCoreApplication a(argc, argv);

    qpiupsmon monitor;
    monitor.connectToHost();
    monitor.start();

    return a.exec();
}
