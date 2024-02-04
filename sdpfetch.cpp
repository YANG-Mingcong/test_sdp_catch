#include "sdpfetch.h"

#include <QDebug>
#include <QtEndian>

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

    sapRaw.clear();
}

void SdpFetch::sapParser(QByteArray _datagram)
{
    if (_datagram.size() < 12) // 最小数据包大小
    {
        qDebug() << "Invalid SAP packet size";
        return;
    }

    // 解析版本和标志
    quint8 versionAndFlags = _datagram.at(0);
    quint8 version = (versionAndFlags >> 5) & 0x07;
    bool aBit = (versionAndFlags & 0x04) != 0;
    bool rBit = (versionAndFlags & 0x02) != 0;
    bool tBit = (versionAndFlags & 0x01) != 0;
    bool authFlag = (versionAndFlags & 0x10) != 0;
    bool encryptionFlag = (versionAndFlags & 0x08) != 0;
    bool cBit = (versionAndFlags & 0x02) != 0; // Compressed Bit

    // 解析认证数据长度
    quint8 authLen = _datagram.at(1);

    // 解析消息ID哈希
    quint32 msgIdHash = qFromBigEndian<quint32>(reinterpret_cast<const uchar*>(_datagram.constData() + 2));

    // 解析原始源
    QByteArray originatingSource = _datagram.mid(6, 16);

    // 解析可选认证数据
    QByteArray authData = _datagram.mid(22, authLen);

    // 解析可选负载类型
    quint8 payloadType = _datagram.at(22 + authLen);

    // 解析负载
    QByteArray payload = _datagram.mid(23 + authLen);

    // 打印解析结果
    qDebug() << "SAP Packet:";
    qDebug() << "Version:" << version;
    qDebug() << "A Bit (Address Type):" << aBit;
    qDebug() << "R Bit (Reserved):" << rBit;
    qDebug() << "T Bit (Message Type):" << tBit;
    qDebug() << "C Bit (Compressed):" << cBit;
    qDebug() << "Authentication Flag:" << authFlag;
    qDebug() << "Encryption Flag:" << encryptionFlag;
    qDebug() << "Authentication Data Length:" << authLen;
    qDebug() << "Message ID Hash:" << msgIdHash;
    qDebug() << "Originating Source:" << originatingSource.toHex();
    qDebug() << "Optional Authentication Data:" << authData.toHex();
    qDebug() << "Optional Payload Type:" << payloadType;
    qDebug() << "Payload:" << payload.toHex();
}

void SdpFetch::processPendingDatagrams()
{
    QByteArray datagram;

    while(udpSocket4.hasPendingDatagrams()) {
        datagram.resize(qsizetype(udpSocket4.pendingDatagramSize()));
        udpSocket4.readDatagram(datagram.data(), datagram.size());

        sapRaw.clear();
        sapRaw.append(datagram);

        qDebug() << tr("SAP datagram: ");
        // qDebug() << datagram.toStdString();
        qDebug() << sapRaw;

        qDebug() << tr("SAP header: ");
        for (int i = 0; i < 8; ++i)
        {
            qDebug().noquote() << QString("%1 ").arg(static_cast<quint8>(datagram[i]), 8, 2, QChar('0'));
        }

        SdpFetch::sapParser(sapRaw);
    }
}
