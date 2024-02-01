#include "sdpfetch.h"

#include <QDebug>

SdpFetch::SdpFetch(QObject *parent)
    : QObject{parent}
{
    sapAddress4.setAddress("239.255.255.255");
    sapPort.append("9875");

    udpSocket4.bind(QHostAddress::AnyIPv4, sapPort.toInt(), QUdpSocket::ShareAddress);
    udpSocket4.joinMulticastGroup(sapAddress4);

    connect(&udpSocket4, &QUdpSocket::readyRead,
            this, &SdpFetch::processPendingDatagrams);
}

SdpFetch::~SdpFetch()
{
    disconnect(&udpSocket4, &QUdpSocket::readyRead,
               this, &SdpFetch::processPendingDatagrams);

    udpSocket4.leaveMulticastGroup(sapAddress4);

    udpSocket4.deleteLater();
    sapPort.clear();
    sapAddress4.clear();

    sdpRaw.clear();
}

void SdpFetch::processPendingDatagrams()
{
    QByteArray datagram;

    while(udpSocket4.hasPendingDatagrams()) {
        datagram.resize(qsizetype(udpSocket4.pendingDatagramSize()));
        udpSocket4.readDatagram(datagram.data(), datagram.size());

        sdpRaw.clear();
        sdpRaw.append(datagram);

        qDebug() << tr("SAP datagram: ");
        // qDebug() << datagram.toStdString();
        qDebug() << sdpRaw.toStdString();
    }
}
