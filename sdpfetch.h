#ifndef SDPFETCH_H
#define SDPFETCH_H

#include <QObject>
#include <QHostAddress>
#include <QUdpSocket>
#include <QMap>



class SdpFetch : public QObject
{
    Q_OBJECT
public:
    explicit SdpFetch(QObject *parent = nullptr);
    QByteArray sapRaw;

    ~SdpFetch();

private slots:
    void processPendingDatagrams();

private:
    QUdpSocket udpSocket4;
    QHostAddress sapAddress4;
    QString sapPort;
    QMap<QByteArray, QDateTime>sdpRawMap;

    void sapParser(QByteArray);
    void sdpRawMapInit();
    void sdpRawMapAdd(QByteArray);


signals:
};

#endif // SDPFETCH_H
