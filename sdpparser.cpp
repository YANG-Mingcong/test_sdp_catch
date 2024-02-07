#include "sdpparser.h"
#include "SdpStructures.h"

#include <QRegularExpression>


SdpParser::SdpParser(QObject *parent, const QByteArray& _datagram)
    : QObject{parent}, sdpRaw(_datagram)
{


}

void SdpParser::sdpSplitTest()
{
    this->sdpSplitter(sdpRaw);
}

void SdpParser::sdpSplitter(QByteArray _datagram)
{
    QRegularExpression sessionRegex("(v=.*?)(?=t=|$)", QRegularExpression::DotMatchesEverythingOption);
    QRegularExpression timeRegex("(t=.*?)(?=m=|$)", QRegularExpression::DotMatchesEverythingOption);
    QRegularExpression mediaRegex("(m=.*?)(?=m=|$)", QRegularExpression::DotMatchesEverythingOption);

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

}
