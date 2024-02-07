#ifndef SDPPARSER_H
#define SDPPARSER_H

#include <QObject>
#include "SdpStructures.h"

#include <QRegularExpression>

class SdpParser : public QObject
{
    Q_OBJECT
public:
    explicit SdpParser(QObject *parent = nullptr, const QByteArray& sdpRaw = QByteArray());
    ~SdpParser();

    void sdpSplitTest();

private:
    QByteArray sdpRaw;

    QByteArray sdpSessionDescription;
    QByteArray sdpTimeDescription;
    QByteArray sdpMediaDescription;

    QRegularExpression sessionRegex;
    QRegularExpression timeRegex;
    QRegularExpression mediaRegex;



    QRegularExpression timingRegex;
    QRegularExpression repeatRegex;

    void sdpSplitter(const QByteArray&);


    void sdpTimeDescriptionParser();

    SDP *sdp;

    void printSDP();

signals:
};

#endif // SDPPARSER_H
