#include "sdpfetch.h"

SdpFetch::SdpFetch(QObject *parent)
    : QObject{parent}
{
    sapAddress4.setAddress("239.255.255.255");
    sapPort.append("9875");

    udpSocket4.bind(QHostAddress::AnyIPv4, sapPort.toInt(), QUdpSocket::ShareAddress);
    udpSocket4.joinMulticastGroup(sapAddress4);
}
