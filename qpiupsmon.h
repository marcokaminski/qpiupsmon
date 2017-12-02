#ifndef QPIUPSMON_H
#define QPIUPSMON_H

#include <QObject>
#include <QTimer>
#include <QTextStream>

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
    explicit qpiupsmon(const QHostAddress& host = QHostAddress::LocalHost,
                       const quint16 port = 1883,
                       QObject *parent = nullptr);

signals:

public slots:
    void readRegisters(void);
    void start(void);

private:
    QTextStream out, err;
    QTimer messtimer;

    t_piups piupsVal;
};

#endif // QPIUPSMON_H
