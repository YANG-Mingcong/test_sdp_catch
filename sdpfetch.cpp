#include "sdpfetch.h"

#include <QDebug>
#include <QtEndian>
#include <QDateTime>


QMap<QByteArray, QDateTime> sdpRawMap;


SdpFetch::SdpFetch(QObject *parent)
    : QObject{parent}
{
    sapAddress4.setAddress("239.255.255.255");
    sapPort.append("9875");

    udpSocket4.bind(QHostAddress::AnyIPv4, sapPort.toInt(), QUdpSocket::ShareAddress);
    udpSocket4.joinMulticastGroup(sapAddress4);

    connect(&udpSocket4, &QUdpSocket::readyRead,
            this, &SdpFetch::processPendingDatagrams);

    sdpRawMapTimeoutSecond = 120;
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this,
            &SdpFetch::checkTimeout);
    timer->start(5000); // 启动定时器，设定5000毫秒（即5秒）的定时周期

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

    if (timer) {
        timer->stop();
        delete timer;
    }
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

        // qDebug() << tr("SAP header: ");
        // for (int i = 0; i < 8; ++i)
        // {
        //     qDebug().noquote() << QString("%1 ").arg(static_cast<quint8>(datagram[i]), 8, 2, QChar('0'));
        // }

        if (datagram.contains("application/sdp")) {
            SdpFetch::sapParser(sapRaw);
        } else {
            qDebug() << "Datagram does not contain 'application/sdp', skipping...";
        }
    }
}

void SdpFetch::checkTimeout()
{
    // 尝试加锁，如果不能获得锁，说明上一次处理还未完成，跳过本次事件
    if (!sdpRawMapMutex.tryLock()) {
        qDebug() << "Failed to acquire lock, skipping event";
        return;
    }

    // 获取当前时间的秒数
    qint64 currentSeconds = QDateTime::currentSecsSinceEpoch();

    // 遍历 QMap，检查每个条目是否超时
    QMapIterator<QByteArray, QDateTime> iter(sdpRawMap);
    while (iter.hasNext()) {
        iter.next();

        // 计算时间差，以秒为单位
        qint64 elapsedTime = iter.value().secsTo(QDateTime::fromSecsSinceEpoch(currentSeconds));

        // 如果时间差大于等于你的超时阈值，执行相应的操作
        if (elapsedTime >= sdpRawMapTimeoutSecond) {
            // 超时处理逻辑
            qDebug() << "Entry" << iter.key() << "is out of time!";
            // 可以在这里删除超时的条目，例如 sdpRawMap.remove(iter.key());
            this->sdpRawMapDelete(iter.key());
        }
    }

    // 解锁
    sdpRawMapMutex.unlock();
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
    quint8 version = (versionAndFlags >> 5) & 0x07;  // Extracting the top 3 bits
    // bool aBit = (versionAndFlags & 0x10) != 0;  // Extracting the 4th bit (A)
    // bool rBit = (versionAndFlags & 0x08) != 0;  // Extracting the 5th bit (R)
    bool tBit = (versionAndFlags & 0x04) != 0;  // Extracting the 6th bit (T)
    // bool encryptionFlag = (versionAndFlags & 0x02) != 0;  // Extracting the 7th bit (E)
    // bool cBit = (versionAndFlags & 0x01) != 0;  // Extracting the 8th bit (C)


    // // 解析认证数据长度
    quint8 authLen = _datagram.at(1);

    // // 解析消息ID哈希
    // quint32 msgIdHash = qFromBigEndian<quint32>(reinterpret_cast<const uchar*>(_datagram.constData() + 2));

    // // 解析原始源
    // QByteArray originatingSource = _datagram.mid(6, 16);

    // // 解析可选认证数据
    // QByteArray authData = _datagram.mid(22, authLen);

    // // 解析可选负载类型
    // quint8 payloadType = _datagram.at(22 + authLen);

    // 解析负载
    QByteArray payload = _datagram.mid(23 + authLen + 1);

    // // 打印解析结果
    // qDebug() << "SAP Packet:";
    // qDebug() << "Version:" << version;
    // qDebug() << "A Bit (Address Type):" << aBit;
    // qDebug() << "R Bit (Reserved):" << rBit;
    // qDebug() << "T Bit (Message Type):" << tBit;
    // qDebug() << "Encryption Flag:" << encryptionFlag;
    // qDebug() << "C Bit (Compressed):" << cBit;
    // qDebug() << "Authentication Data Length:" << authLen;
    // qDebug() << "Message ID Hash:" << msgIdHash;
    // qDebug() << "Originating Source:" << originatingSource.toHex();
    // qDebug() << "Optional Authentication Data:" << authData.toHex();
    // qDebug() << "Optional Payload Type:" << payloadType;
    // qDebug() << "Payload:" << payload.toHex();


    if(!tBit)
    {
        // add or update sdpRawMap
        this->sdpRawMapAnnouncement(payload);
    }
    else
    {
        //delete sdpRawMap
        this->sdpRawMapDeletion(payload);
    }

    qInfo() << "sdpRawMap: \n"
            << QDateTime::currentDateTime().toString()
            << "\n"
            << sdpRawMap;


}

void SdpFetch::sdpRawMapInit()
{
    sdpRawMap.clear();
}

void SdpFetch::sdpRawMapAnnouncement(QByteArray _sapPayload)
{
    QMutexLocker locker(&sdpRawMapMutex); // 加锁

    if(!this->sdpRawMapContain(_sapPayload))
    {
        qDebug() << "sdpRawMapAnnouncement: - Not found";
        this->sdpRawMapAdd(_sapPayload, false);
        qDebug() << "sdpRawMapAnnouncement:" << _sapPayload.toStdString();
    }
    else
    {
        qDebug() << "sdpRawMapAnnouncement: - found";
        this->sdpRawMapUpdate(_sapPayload);
        qDebug() << "sdpRawMapAnnouncement:" << _sapPayload.toStdString();
    }
}

void SdpFetch::sdpRawMapDeletion(QByteArray _sapPayload)
{
    QMutexLocker locker(&sdpRawMapMutex); // 加锁

    if(!this->sdpRawMapContain(_sapPayload))
    {
        qDebug() << "sdpRawMapDeletion: - Not found";
        return;
    }
    else
    {
        qDebug() << "sdpRawMapDeletion: - found";
        this->sdpRawMapDelete(_sapPayload);
        qDebug() << "sdpRawMapDeleted" << _sapPayload.toStdString();
    }
}

void SdpFetch::sdpRawManuleAdd(QByteArray _data)
{
    QMutexLocker locker(&sdpRawMapMutex); // 加锁

    if(!this->sdpRawMapContain(_data))
    {
        qDebug() << "sdpRawMapAnnouncement: - Not found";
        this->sdpRawMapAdd(_data, true);
        qDebug() << "sdpRawMapAnnouncement:" << _data.toStdString();
    }
    else
    {
        qDebug() << "sdpRawMapAnnouncement: - found";
        return;
    }
}

void SdpFetch::sdpRawManuleDelete(QByteArray _data)
{
    QMutexLocker locker(&sdpRawMapMutex); // 加锁

    if(!this->sdpRawMapContain(_data))
    {
        qDebug() << "sdpRawMapManuleDeletion: - Not found";
        return;
    }
    else
    {
        qDebug() << "sdpRawMapManuleDeletion: - found";
        this->sdpRawMapDelete(_data);
        qDebug() << "sdpRawMapManuleDeleted" << _data.toStdString();
    }
}

QMap<QByteArray, QDateTime> SdpFetch::getSdpRawMap()
{
    QMutexLocker locker(&sdpRawMapMutex); // 加锁

    // 创建 sdpRawMap 的副本并返回
    return sdpRawMap;
}

quint16 SdpFetch::getSdpRawMapSize()
{
    return sdpRawMap.size();
}

QList<QByteArray> SdpFetch::getSdpRawMapKeyList()
{
    QList<QByteArray> _return;

    QMapIterator<QByteArray, QDateTime> iter(sdpRawMap);
    while (iter.hasNext()) {
        iter.next();

        _return.append(iter.key());
    }
    return _return;
}


void SdpFetch::sdpRawMapAdd(QByteArray _data, bool _isManuleAdd)
{
    if(!_isManuleAdd)
    {
        sdpRawMap.insert(_data, QDateTime::currentDateTime());
    }
    else
    {
        sdpRawMap.insert(_data, QDateTime::fromMSecsSinceEpoch(std::numeric_limits<qint64>::max()));
    }
}

void SdpFetch::sdpRawMapUpdate(QByteArray _data)
{
    sdpRawMap[_data] = QDateTime::currentDateTime();
}

void SdpFetch::sdpRawMapDelete(QByteArray _data)
{
    sdpRawMap.remove(_data);
}

bool SdpFetch::sdpRawMapContain(QByteArray _data)
{
    return sdpRawMap.contains(_data);
}


