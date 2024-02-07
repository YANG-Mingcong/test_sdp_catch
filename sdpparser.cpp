#include "sdpparser.h"
#include "SdpStructures.h"

#include <QRegularExpression>

#include <QMetaObject>
#include <QMetaProperty>


SdpParser::SdpParser(QObject *parent, const QByteArray& _datagram)
    : QObject{parent}, sdpRaw(_datagram)
{
    sessionRegex.setPattern("(v=.*?)(?=m=|$)");
    sessionRegex.setPatternOptions(QRegularExpression::DotMatchesEverythingOption);

    timeRegex.setPattern("(t=.*?)(?=m=|$)");
    timeRegex.setPatternOptions(QRegularExpression::DotMatchesEverythingOption);

    mediaRegex.setPattern("(m=.*?)(?=m=|$)");
    mediaRegex.setPatternOptions(QRegularExpression::DotMatchesEverythingOption);

    timingRegex.setPattern("t=(\\d+) (\\d+)");
    repeatRegex.setPattern("r=(.*)");

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

    this->sdpTimeDescriptionParser();

    this->printSDP();

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
        qDebug() << "      connctionTtl:" << connectionData.connctionTtl;
    }
    qDebug() << "    bBandwidth:" << sdp->sessionOptions.bBandwidth;
    qDebug() << "    zTimeZone:" << sdp->sessionOptions.zTimeZone;
    qDebug() << "    kEncryptionKey:" << sdp->sessionOptions.kEncryptionKey;
    qDebug() << "    aAttribute:";
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
    qDebug() << "  mdiaOptions:";
    qDebug() << "    iMediaTitle:" << sdp->mdiaOptions.iMediaTitle;
    qDebug() << "    cConnectionInformation:";
    for (const auto& connectionData : sdp->mdiaOptions.cConnectionInformation) {
        qDebug() << "      nettype:" << connectionData.nettype;
        qDebug() << "      addrtype:" << connectionData.addrtype;
        qDebug() << "      connectionAddress:" << connectionData.connectionAddress;
        qDebug() << "      connctionTtl:" << connectionData.connctionTtl;
    }
    qDebug() << "    bBandwidth:" << sdp->mdiaOptions.bBandwidth;
    qDebug() << "    kEncryptionKey:" << sdp->mdiaOptions.kEncryptionKey;
    qDebug() << "    aAttribute:";
    for (const auto& attribute : sdp->mdiaOptions.aAttribute) {
        qDebug() << "      attribute:" << attribute.attribute;
        qDebug() << "      value:" << attribute.value;
    }
}

