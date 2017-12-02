/**
 * BSD 3-Clause License
 * 
 * Copyright (c) 2017, Marco Kaminski <marco.kaminski@t-online.de>
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <bcm2835.h>

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QTextStream>

#include "qpiupsmon.h"

int main(int argc, char *argv[]) {
    QTextStream err(stderr);
    QTextStream out(stdout);

    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName("QPiUPSMon");
    QCoreApplication::setApplicationVersion("0.1");

    QCommandLineParser clParser;
    clParser.setApplicationDescription("Monitor für die Raspberry Pi Zusatzplatine PiUPS+");
    clParser.addHelpOption();
    clParser.addVersionOption();
    clParser.addOptions({
        {"host", "MQTT-Broker hostaddress", "ip", "127.0.0.1"},
        {"port", "MQTT-Broker port", "port", "1833"},
        {"clientid", "Client-ID for MQTT-Broker", "name", ""},
        {{"p", "print"}, "Daten auf stdout ausgeben"},
        {{"c", "config"}, "Konfigurationsdatei", "file", ""},
        {{"d", "debug"}, "Debug-Asugaben aktivieren"},
    });

    clParser.process(a);

    bool print = clParser.isSet("print");
    bool debug = clParser.isSet("debug");
    QHostAddress host = clParser.isSet("host") ? QHostAddress(clParser.value("host")) : QHostAddress::LocalHost;
    quint16 port = clParser.isSet("port") ? clParser.value("port").toInt() : 1883;
    QString clientid = clParser.isSet("clientid") ? clParser.value("clientid") : "";

    if (debug) out << "Intialisiere bcm2835 Bibliothek" << endl;
    // init bcm2835
    if (!bcm2835_init()) {
        err << "bcm2835_init failed. Are you running as root?" << endl;
        return 1;
    }

    // init i2c
    if (debug) out << "i2c Schnittstelle" << endl;
    if (!bcm2835_i2c_begin()) {
        err << "bcm2835_i2c_begin failed. Are you running as root?" << endl;
        return 1;
    }

    // set i2c-clock to 100 kHz
    if (debug) out << "setze i2c Baudrate" << endl;
    bcm2835_i2c_set_baudrate(100000);

    if (debug) out << "erzeuge Monitor-Objekt" << endl;
    qpiupsmon *monitor;
    if (clParser.isSet("config")) {
	monitor = new qpiupsmon(clParser.value("config"), debug);
    } else {
	monitor = new qpiupsmon(host, port, print, debug);
    }

    monitor->setClientId(clientid);
    monitor->setCleanSession(true);
    monitor->setAutoReconnect(true);
    monitor->setAutoReconnectInterval(5000);

    if (debug) out << "verbinde mit broker" << endl;
    monitor->connectToHost();
    if (debug) out << "starte den Monitor" << endl;
    monitor->start();

    return a.exec();
}
