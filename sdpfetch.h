#ifndef SDPFETCH_H
#define SDPFETCH_H

#include <QObject>
#include <QHostAddress>
#include <QUdpSocket>



class SdpFetch : public QObject
{
    Q_OBJECT
public:
    explicit SdpFetch(QObject *parent = nullptr);
    QByteArray sapRaw;

    ~SdpFetch();

private:
    QUdpSocket udpSocket4;
    QHostAddress sapAddress4;
    QString sapPort;
private slots:
    void processPendingDatagrams();


signals:
};

#endif // SDPFETCH_H
