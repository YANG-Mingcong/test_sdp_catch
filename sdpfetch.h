#ifndef SDPFETCH_H
#define SDPFETCH_H

#include <QObject>
#include <QHostAddress>
#include <QUdpSocket>
#include <QMap>
#include <QMutex>

#include <QTimer>


class SdpFetch : public QObject
{
    Q_OBJECT
public:
    explicit SdpFetch(QObject *parent = nullptr);
    QByteArray sapRaw;

    ~SdpFetch();

    void setSdpRawMapTimeoutSecond();
    qint16 getSdpRawMapTimeoutSecond;


private slots:
    void processPendingDatagrams();

    void checkTimeout();

private:
    QUdpSocket udpSocket4;
    QHostAddress sapAddress4;
    QString sapPort;

    void sapParser(QByteArray);
    void sdpRawMapInit();
    void sdpRawMapAnnouncement(QByteArray);
    void sdpRawMapDeletion(QByteArray);

    QTimer *timer;
    qint16 sdpRawMapTimeoutSecond;

    QMutex sdpRawMapMutex; // 用于保护sdpRawMap的互斥锁


signals:
};

#endif // SDPFETCH_H
