#include <string.h>
#include <bcm2835.h>

#include <qmqtt_client.h>
#include <qmqtt_message.h>

#include <QRegExp>
#include <QByteArray>
#include "qpiupsmon.h"

qpiupsmon::qpiupsmon(const QHostAddress& host, const quint16 port, bool print, bool debug, QObject *parent) : QMQTT::Client(host, port, parent), out(stdout), err(stderr) {
    connect(&messtimer, SIGNAL(timeout()), this, SLOT(readRegisters()));
    connect(this, SIGNAL(connected()), this, SLOT(start()));

    debugging = debug;
    printing = print;
}

qpiupsmon::qpiupsmon(QString configFile, bool debug, QObject*) : out(stdout), err(stderr) {
    connect(&messtimer, SIGNAL(timeout()), this, SLOT(readRegisters()));
    connect(this, SIGNAL(connected()), this, SLOT(start()));

    config = new QSettings(configFile, QSettings::IniFormat);
    debugging = debug;

    if (debugging) out << "Lese Konfigurationsdatei: " << configFile << endl;
    config->beginGroup("MQTT-Broker");
    setHost(QHostAddress(config->value("host", "127.0.0.1").toString()));
    setPort(config->value("port", 1883).toInt());
    config->endGroup();

    config->beginGroup("QPiUPSMon");
    printing = config->value("printing", false).toBool();
    config->endGroup();
}

void qpiupsmon::start() {
    if (debugging) out << "starte Timer für Messintervall" << endl;
    messtimer.start(10000);
}

void qpiupsmon::readRegisters() {
    char reg;
    char buffer[20];

    bcm2835_i2c_setSlaveAddress(0x18);

    // read statusbyte
    if (debugging) out << "lese Statusbyte" << endl;
    reg = 0x00;
    memset(buffer, 0, sizeof(buffer));
    if (bcm2835_i2c_write(&reg, 1) != BCM2835_I2C_REASON_OK || bcm2835_i2c_read(buffer,1) != BCM2835_I2C_REASON_OK) {
        err << "read statusregister failed" << endl;
    } else {
        piupsVal.status = (int)buffer[0];
    }

    // read versionstring
    if (debugging) out << "lese Versionsstring" << endl;
    reg = 0x01;
    memset(buffer, 0, sizeof(buffer));
    if (bcm2835_i2c_write(&reg, 1) != BCM2835_I2C_REASON_OK || bcm2835_i2c_read(buffer, 12) != BCM2835_I2C_REASON_OK) {
        err << "read versionstring failed" << endl;
    } else {
        QRegExp versionPattern("^v[0-9]+\\.[0-9]+\\.[0-9]+");

        if (versionPattern.indexIn(QString::fromLatin1(buffer)) < 0) {
            err << "invalid versionstring" << endl;
        } else {
            piupsVal.version = versionPattern.cap(0);
        }
    }

    // read power values
    if (debugging) out << "lese Strom und Spannungswerte" << endl;
    reg = 0x02;
    memset(buffer, 0, sizeof(buffer));
    if (bcm2835_i2c_write(&reg, 1) != BCM2835_I2C_REASON_OK || bcm2835_i2c_read(buffer, 10)) {
       err << "read power values failed" << endl;
    } else {
       piupsVal.uAccu    = (256*(float)buffer[0] + (float)buffer[1]) / 1000.0f;
       piupsVal.iPi      = (256*(float)buffer[2] + (float)buffer[3]) / 1000.0f;
       piupsVal.uPi      = (256*(float)buffer[4] + (float)buffer[5]) / 1000.0f;
       piupsVal.uPrimary = (256*(float)buffer[6] + (float)buffer[7]) / 1000.0f;
       piupsVal.uExtern  = (256*(float)buffer[8] + (float)buffer[9]) / 1000.0f;
    }

    if(printing || debugging) {
        out << piupsVal.version << " | ";
        out << QString("0x%1").arg(piupsVal.status, 2, 16, QLatin1Char('0')) << " | ";
        out << QString("%1").arg(piupsVal.uAccu,  0, 'f', 3) << " | ";
        out << QString("%1").arg(piupsVal.iPi,  0, 'f', 3) << " | ";
        out << QString("%1").arg(piupsVal.uPi,  0, 'f', 3) << " | ";
        out << QString("%1").arg(piupsVal.uPrimary,  0, 'f', 3) << " | ";
        out << QString("%1").arg(piupsVal.uExtern,  0, 'f', 3);
        out << endl;
    }

    if(isConnectedToHost()) {
        if (debugging) out << "sende daten zum Broker" << endl;

        QMQTT::Message message(0, "piupsmon/version", piupsVal.version.toUtf8());
        publish(message);

        message.setTopic("piupsmon/uAccu");
        message.setPayload(QString("%1").arg(piupsVal.uAccu, 0, 'f', 3).toUtf8());
        publish(message);
        message.setTopic("piupsmon/iPi");
        message.setPayload(QString("%1").arg(piupsVal.iPi, 0, 'f', 3).toUtf8());
        publish(message);
        message.setTopic("piupsmon/uPi");
        message.setPayload(QString("%1").arg(piupsVal.uPi,0, 'f', 3).toUtf8());
        publish(message);
        message.setTopic("piupsmon/uPrimary");
        message.setPayload(QString("%1").arg(piupsVal.uPrimary,0, 'f', 3).toUtf8());
        publish(message);
        message.setTopic("piupsmon/uExtern");
        message.setPayload(QString("%1").arg(piupsVal.uExtern,0, 'f', 3).toUtf8());
        publish(message);
        message.setTopic("piupsmon/status");
        message.setPayload(QString("%1").arg(piupsVal.status).toUtf8());
        publish(message);
    } else {
        if (debugging) out << "keine Verbindung zu Broker" << endl;
    }
}
