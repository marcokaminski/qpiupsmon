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

#ifndef QPIUPSMON_H
#define QPIUPSMON_H

#include <QObject>
#include <QTimer>
#include <QTextStream>
#include <QSettings>

#include <qmqtt_client.h>
class qpiupsmon : public QMQTT::Client {
    Q_OBJECT

typedef struct {
    int status;
    QString version;
    float uAccu;
    float iPi;
    float uPi;
    float uPrimary;
    float uExtern;
} t_piups;

public:
    explicit qpiupsmon(const QHostAddress& host,
                       const quint16 port,
                       bool print,
                       bool debug = false,
                       QObject *parent = nullptr);

    qpiupsmon(QString configFile,
              bool debug = false,
              QObject *parent = nullptr);

signals:

public slots:
    void readRegisters(void);
    void start(void);

private:
    QTextStream out, err;
    QTimer messtimer;

    t_piups piupsVal;
    QSettings *config;

    bool printHeader = true;
    bool printing;
    bool debugging;

    int updateIntervall = 10000;

    QString topic_App      = "qpiupsmon";
    QString topic_Version  = "version";
    QString topic_Status   = "status";
    QString topic_uAccu    = "uAccu";
    QString topic_iPi      = "iPi";
    QString topic_uPi      = "uPi";
    QString topic_uPrimary = "uPrimary";
    QString topic_uExtern  = "uExtern";
};

#endif // QPIUPSMON_H
