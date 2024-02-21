#include "sdpparser.h"
#include "SdpStructures.h"

#include <QRegularExpression>

#include <QMetaObject>
#include <QMetaProperty>


SdpParser::SdpParser(QObject *parent, const QByteArray& _datagram)
    : QObject{parent}, sdpRaw(_datagram)
{
    sessionRegex.setPattern("(v=.*?)(?=t=|$)");
    sessionRegex.setPatternOptions(QRegularExpression::DotMatchesEverythingOption);

    timeRegex.setPattern("(t=.*?)(?=m=|$)");
    timeRegex.setPatternOptions(QRegularExpression::DotMatchesEverythingOption);

    mediaRegex.setPattern("(m=.*?)(?=m=|$)");
    mediaRegex.setPatternOptions(QRegularExpression::DotMatchesEverythingOption);

    versionRegex.setPattern("v=(\\d+)");
    originRegex.setPattern("o=\\s*([^\\s]+)\\s*([^\\s]+)\\s*([^\\s]+)\\s*([^\\s]+)\\s*([^\\s]+)\\s*([^\\s]+)");
    sessionNameRegex.setPattern("s=(.*?)(?=\r\n|$)");
    sessionNameRegex.setPatternOptions(QRegularExpression::DotMatchesEverythingOption);
    sessionOptionRegex.setPattern("([iuepcbzka])=(.*?)(?=[iuepcbzka]=|$)");
    sessionOptionRegex.setPatternOptions(QRegularExpression::DotMatchesEverythingOption);
    sessionConnectionRegex.setPattern("([A-Za-z]+)\\s+(IP[46])\\s*([A-Fa-f0-9.:]+)?/?([0-9]+)?/?([0-9]+)?");

    timingRegex.setPattern("t=(\\d+) (\\d+)");
    repeatRegex.setPattern("r=(.*)(?=\r\n|$)");

    sdp = new SDP;

}

SdpParser::~SdpParser()
{
    delete sdp;
}

void SdpParser::sdpSplitTest()
{
    this->sdpSplitter(sdpRaw);
}

QString SdpParser::getParserResult()
{
    QString result;

    result += "SDP:\n";
    result += "  version: " + QString::number(sdp->version) + "\n";

    result += "  origin:\n";
    result += "    username: " + sdp->origin.username + "\n";
    result += "    sessId: " + sdp->origin.sessId + "\n";
    result += "    sessVersion: " + sdp->origin.sessVersion + "\n";
    result += "    nettype: " + sdp->origin.nettype + "\n";
    result += "    addrtype: " + sdp->origin.addrtype + "\n";
    result += "    unicastAddress: " + sdp->origin.unicastAddress + "\n";

    result += "  sessionName: " + sdp->sessionName + "\n";

    result += "  sessionOptions:\n";
    result += "    iSessionInformation: " + sdp->sessionOptions.iSessionInformation + "\n";
    result += "    uUriOfDescription: " + sdp->sessionOptions.uUriOfDescription + "\n";
    result += "    eEmailAddress: " + sdp->sessionOptions.eEmailAddress + "\n";
    result += "    pPhoneNumber: " + sdp->sessionOptions.pPhoneNumber + "\n";

    result += "    cConnectionInformation:\n";
    for (const auto& connectionData : sdp->sessionOptions.cConnectionInformation) {
        result += "      nettype: " + connectionData.nettype + "\n";
        result += "      addrtype: " + connectionData.addrtype + "\n";
        result += "      connectionAddress: " + connectionData.connectionAddress.toString() + "\n";
        result += "      connctionTtl: " + QString::number(connectionData.connectionTtl) + "\n";
    }

    result += "    bBandwidth:" + sdp->sessionOptions.bBandwidth + "\n";
    result += "    zTimeZone:" + sdp->sessionOptions.zTimeZone + "\n";
    result += "    kEncryptionKey:" + sdp->sessionOptions.kEncryptionKey + "\n";
    result += "    aAttributes:\n";
    for (const auto& attribute : sdp->sessionOptions.aAttribute) {
        result += "      attribute:" + attribute.attribute + "\n";
        result += "      value:" + attribute.value + "\n";
    }
    result += "  timing:\n";
    result += "    startTime: " + QString::number(sdp->timing.startTime) + "\n";
    result += "    stopTime: " + QString::number(sdp->timing.stopTime) + "\n";

    result += "  repeatTimes: " + sdp->repeatTimes + "\n";

    result += "  media:\n";
    result += "    mediaType:\n";
    result += "      contentTypeString: " + sdp->media.mediaType.contentTypeString + "\n";
    result += "    port: " + QString::number(sdp->media.port) + "\n";
    result += "    transportProtocol:\n";
    result += "      contentTypeString: " + sdp->media.transportProtocol.contentTypeString + "\n";
    result += "    fmt: " + sdp->media.fmt + "\n";

    result += "  mediaOptions:\n";
    result += "    iMediaTitle: " + sdp->mediaOptions.iMediaTitle + "\n";

    result += "    cConnectionInformation:\n";
    for (const auto& connectionData : sdp->mediaOptions.cConnectionInformation) {
        result += "      nettype: " + connectionData.nettype + "\n";
        result += "      addrtype: " + connectionData.addrtype + "\n";
        result += "      connectionAddress: " + connectionData.connectionAddress.toString() + "\n";
        result += "      connctionTtl: " + QString::number(connectionData.connectionTtl) + "\n";
    }

    result += "    bBandwidth:" + sdp->mediaOptions.bBandwidth + "\n";
    result += "    kEncryptionKey:" + sdp->mediaOptions.kEncryptionKey + "\n";
    result += "    aAttributes: \n";
    for (const auto& attribute : sdp->mediaOptions.aAttribute) {
        result += "      attribute:" + attribute.attribute + "\n";
        result += "      value:" + attribute.value + "\n";
    }
    return result;
}

void SdpParser::sdpSplitter(const QByteArray& _datagram)
{
    QRegularExpressionMatch match;

    // Extract Session description
    match = sessionRegex.match(_datagram);
    if (match.hasMatch()) {
        sdpSessionDescription = match.captured(1).toUtf8();
    }

    // Extract Time description
    match = timeRegex.match(_datagram);
    if (match.hasMatch()) {
        sdpTimeDescription = match.captured(1).toUtf8();
    }

    // Extract Media description
    match = mediaRegex.match(_datagram);
    if (match.hasMatch()) {
        sdpMediaDescription = match.captured(1).toUtf8();
    }

    qDebug() << "Session Description:" << sdpSessionDescription;
    qDebug() << "Time Description:" << sdpTimeDescription;
    qDebug() << "Media Description:" << sdpMediaDescription;

    this->sdpSessionDescriptionParser();

    this->sdpTimeDescriptionParser();

    this->printSDP();

}

void SdpParser::sdpSessionDescriptionParser()
{
    // Match version field
    auto versionMatch = versionRegex.match(sdpSessionDescription);
    if (versionMatch.hasMatch()) {
        sdp->version = versionMatch.captured(1).toInt();
    }

    // Match origin field
    auto originMatch = originRegex.match(sdpSessionDescription);
    if (originMatch.hasMatch()) {
        sdp->origin.username = originMatch.captured(1);
        sdp->origin.sessId = originMatch.captured(2);
        sdp->origin.sessVersion = originMatch.captured(3);
        sdp->origin.nettype = originMatch.captured(4);
        sdp->origin.addrtype = originMatch.captured(5);
        sdp->origin.unicastAddress = originMatch.captured(6);
    }

    // Match session name field
    auto sessionNameMatch = sessionNameRegex.match(sdpSessionDescription);
    if (sessionNameMatch.hasMatch()) {
        sdp->sessionName = sessionNameMatch.captured(1);
    }

    // Match session options
    int pos = 0;
    while (pos < sdpSessionDescription.size()) {
        auto optionMatch = sessionOptionRegex.match(sdpSessionDescription, pos);
        if (optionMatch.hasMatch()) {
            QString optionType = optionMatch.captured(1);
            QString optionValue = optionMatch.captured(2);

            switch (optionType.at(0).toLatin1()) {
            case 'i':
                sdp->sessionOptions.iSessionInformation = optionValue.trimmed();
                break;
            case 'u':
                sdp->sessionOptions.uUriOfDescription = optionValue.trimmed();
                break;
            case 'e':
                sdp->sessionOptions.eEmailAddress = optionValue.trimmed();
                break;
            case 'p':
                sdp->sessionOptions.pPhoneNumber = optionValue.trimmed();
                break;
            case 'c':
                this->sdpSessionConnectionInfoParser(optionValue); // Implement this function
                break;
            case 'b':
                sdp->sessionOptions.bBandwidth = optionValue.trimmed();
                break;
            case 'z':
                sdp->sessionOptions.zTimeZone = optionValue.trimmed();
                break;
            case 'k':
                sdp->sessionOptions.kEncryptionKey = optionValue.trimmed();
                break;
            // case 'a':
            //     parseAttributes(optionValue); // Implement this function
            //     break;
            }

            pos = optionMatch.capturedEnd();
        } else {
            break;
        }
    }
}

void SdpParser::sdpTimeDescriptionParser()
{
    int pos = 0;

    // Match t= line
    auto timingMatch = timingRegex.match(sdpTimeDescription, pos);
    if (timingMatch.hasMatch()) {
        sdp->timing.startTime = timingMatch.captured(1).toULongLong();
        sdp->timing.stopTime = timingMatch.captured(2).toULongLong();
        pos = timingMatch.capturedEnd();
    }

    // Match r= line
    auto repeatMatch = repeatRegex.match(sdpTimeDescription, pos);
    if (repeatMatch.hasMatch()) {
        sdp->repeatTimes = repeatMatch.captured(1);
    }
}

void SdpParser::sdpSessionConnectionInfoParser(QString _connInfo)
{
    QRegularExpressionMatch SessionConnectionMatch = sessionConnectionRegex.match(_connInfo);
    if (SessionConnectionMatch.hasMatch()) {

        QString address = SessionConnectionMatch.captured(3);

        QHostAddress baseAddress(address);

        if (baseAddress.protocol() == QAbstractSocket::IPv4Protocol) {
            if(SessionConnectionMatch.captured(5)==""){
                SdpConnectionData connectionData;
                connectionData.nettype = SessionConnectionMatch.captured(1);
                connectionData.addrtype = SessionConnectionMatch.captured(2);
                connectionData.connectionAddress = baseAddress;
                if(SessionConnectionMatch.captured(4)!="")
                {
                    connectionData.connectionTtl = SessionConnectionMatch.captured(4).toUInt();
                }
                else
                {
                    connectionData.connectionTtl = -1;
                }
                sdp->sessionOptions.cConnectionInformation.append(connectionData);
            }
            else if(SessionConnectionMatch.captured(5).toUInt())
            {
                int numberOfAddresses = SessionConnectionMatch.captured(5).toUInt();

                for (int i = 0; i < numberOfAddresses; ++i) {
                    SdpConnectionData connectionData;
                    connectionData.nettype = SessionConnectionMatch.captured(1);
                    connectionData.addrtype = SessionConnectionMatch.captured(2);
                    connectionData.connectionAddress = QHostAddress(baseAddress.toIPv4Address() + i);
                    if(SessionConnectionMatch.captured(4)!="")
                    {
                        connectionData.connectionTtl = SessionConnectionMatch.captured(4).toUInt();
                    }
                    else
                    {
                        connectionData.connectionTtl = -1;
                    }
                    sdp->sessionOptions.cConnectionInformation.append(connectionData);
                }
            }
        } else if (baseAddress.protocol() == QAbstractSocket::IPv6Protocol) {

            if(SessionConnectionMatch.captured(4)==""){
                SdpConnectionData connectionData;
                connectionData.nettype = SessionConnectionMatch.captured(1);
                connectionData.addrtype = SessionConnectionMatch.captured(2);
                connectionData.connectionAddress = baseAddress;
                connectionData.connectionTtl = -1;

                sdp->sessionOptions.cConnectionInformation.append(connectionData);
            }
            else if(SessionConnectionMatch.captured(4).toUInt())
            {
                int numberOfAddresses = SessionConnectionMatch.captured(4).toUInt();

                for (int i = 0; i < numberOfAddresses; ++i) {

                    SdpConnectionData connectionData;
                    connectionData.nettype = SessionConnectionMatch.captured(1);
                    connectionData.addrtype = SessionConnectionMatch.captured(2);

                    connectionData.connectionAddress = this->ipv6AddressShift(baseAddress, i);
                    connectionData.connectionTtl = -1;

                    sdp->sessionOptions.cConnectionInformation.append(connectionData);
                }
            }
        }

    }else{
        qDebug() << "C infor NOT MATCH: " << _connInfo;
    }
}

void SdpParser::printSDP()
{
    qDebug() << "SDP:";
    qDebug() << "  version:" << sdp->version;
    qDebug() << "  origin:";
    qDebug() << "    username:" << sdp->origin.username;
    qDebug() << "    sessId:" << sdp->origin.sessId;
    qDebug() << "    sessVersion:" << sdp->origin.sessVersion;
    qDebug() << "    nettype:" << sdp->origin.nettype;
    qDebug() << "    addrtype:" << sdp->origin.addrtype;
    qDebug() << "    unicastAddress:" << sdp->origin.unicastAddress;
    qDebug() << "  sessionName:" << sdp->sessionName;
    qDebug() << "  sessionOptions:";
    qDebug() << "    iSessionInformation:" << sdp->sessionOptions.iSessionInformation;
    qDebug() << "    uUriOfDescription:" << sdp->sessionOptions.uUriOfDescription;
    qDebug() << "    eEmailAddress:" << sdp->sessionOptions.eEmailAddress;
    qDebug() << "    pPhoneNumber:" << sdp->sessionOptions.pPhoneNumber;
    qDebug() << "    cConnectionInformation:";
    for (const auto& connectionData : sdp->sessionOptions.cConnectionInformation) {
        qDebug() << "      nettype:" << connectionData.nettype;
        qDebug() << "      addrtype:" << connectionData.addrtype;
        qDebug() << "      connectionAddress:" << connectionData.connectionAddress;
        qDebug() << "      connctionTtl:" << connectionData.connectionTtl;
    }
    qDebug() << "    bBandwidth:" << sdp->sessionOptions.bBandwidth;
    qDebug() << "    zTimeZone:" << sdp->sessionOptions.zTimeZone;
    qDebug() << "    kEncryptionKey:" << sdp->sessionOptions.kEncryptionKey;
    qDebug() << "    aAttributes:";
    for (const auto& attribute : sdp->sessionOptions.aAttribute) {
        qDebug() << "      attribute:" << attribute.attribute;
        qDebug() << "      value:" << attribute.value;
    }
    qDebug() << "  timing:";
    qDebug() << "    startTime:" << sdp->timing.startTime;
    qDebug() << "    stopTime:" << sdp->timing.stopTime;
    qDebug() << "  repeatTimes:" << sdp->repeatTimes;
    qDebug() << "  media:";
    qDebug() << "    mediaType:";
    qDebug() << "      contentTypeString:" << sdp->media.mediaType.contentTypeString;
    qDebug() << "    port:" << sdp->media.port;
    qDebug() << "    transportProtocol:";
    qDebug() << "      contentTypeString:" << sdp->media.transportProtocol.contentTypeString;
    qDebug() << "    fmt:" << sdp->media.fmt;
    qDebug() << "  mediaOptions:";
    qDebug() << "    iMediaTitle:" << sdp->mediaOptions.iMediaTitle;
    qDebug() << "    cConnectionInformation:";
    for (const auto& connectionData : sdp->mediaOptions.cConnectionInformation) {
        qDebug() << "      nettype:" << connectionData.nettype;
        qDebug() << "      addrtype:" << connectionData.addrtype;
        qDebug() << "      connectionAddress:" << connectionData.connectionAddress;
        qDebug() << "      connctionTtl:" << connectionData.connectionTtl;
    }
    qDebug() << "    bBandwidth:" << sdp->mediaOptions.bBandwidth;
    qDebug() << "    kEncryptionKey:" << sdp->mediaOptions.kEncryptionKey;
    qDebug() << "    aAttributes:";
    for (const auto& attribute : sdp->mediaOptions.aAttribute) {
        qDebug() << "      attribute:" << attribute.attribute;
        qDebug() << "      value:" << attribute.value;
    }
}

QHostAddress SdpParser::ipv6AddressShift(const QHostAddress& baseAddress, quint16 shiftAddOfAddress)
{
    // Convert the base address to a string
    QString baseAddressStr = baseAddress.toString();

    // Parse the IPv6 address part (remove the "::" prefix if present)
    QString ipv6Part = baseAddressStr.mid(baseAddressStr.lastIndexOf(':') + 1);

    // Convert the IPv6 address part to an integer
    bool ok;
    quint64 ipv6Int = ipv6Part.toULongLong(&ok, 16);
    if (!ok) {
        qDebug() << "Invalid IPv6 address format.";
    }

    // Perform the shift operation
    ipv6Int += shiftAddOfAddress;

    // Convert the shifted value back to a string
    QString shiftedAddressStr = QString::number(ipv6Int, 16);

    // Reconstruct the full IPv6 address
    QString newAddressStr = baseAddressStr.left(baseAddressStr.lastIndexOf(':') + 1) + shiftedAddressStr;

    // Create a new QHostAddress from the shifted address
    QHostAddress shiftedAddress(newAddressStr);

    // Return an invalid address if the input is not IPv6
    return shiftedAddress;
}

