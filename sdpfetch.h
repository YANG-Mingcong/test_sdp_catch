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

    void sdpRawManuleAdd(QByteArray);
    void sdpRawManuleDelete(QByteArray);

    QMap<QByteArray, QDateTime> getSdpRawMap();

    quint16 getSdpRawMapSize();
    QList<QByteArray> getSdpRawMapKeyList();


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

    void sdpRawMapAdd(QByteArray, bool);
    void sdpRawMapUpdate(QByteArray);
    void sdpRawMapDelete(QByteArray);
    bool sdpRawMapContain(QByteArray);


    QTimer *timer;
    qint16 sdpRawMapTimeoutSecond;

    QMutex sdpRawMapMutex; // 用于保护sdpRawMap的互斥锁


signals:
};

#endif // SDPFETCH_H
