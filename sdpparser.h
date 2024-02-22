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
    QString getParserResult();

    void setMediaDetailRegex(const QRegularExpression &newMediaDetailRegex);

private:
    QByteArray sdpRaw;

    QByteArray sdpSessionDescription;
    QByteArray sdpTimeDescription;
    QByteArray sdpMediaDescription;

    QRegularExpression sessionRegex;
    QRegularExpression timeRegex;
    QRegularExpression mediaRegex;

    QRegularExpression versionRegex;
    QRegularExpression originRegex;
    QRegularExpression sessionNameRegex;
    QRegularExpression sessionOptionRegex;
    QRegularExpression sessionConnectionRegex;

    QRegularExpression timingRegex;
    QRegularExpression repeatRegex;

    QRegularExpression mediaDetailRegex;
    QRegularExpression mediaOptionRegex;

    QRegularExpression mediaConnectionRegex;

    void sdpSplitter(const QByteArray&);

    void sdpSessionDescriptionParser();
    void sdpTimeDescriptionParser();
    void sdpMediaDescriptionParser();

    void sdpSessionConnectionInfoParser(QString);
    void sdpSessionAttributesParser(QString);

    void sdpMediaConnectionInfoParser(QString);
    void sdpMediaAttributesParser(QString);

    SDP *sdp;

    void printSDP();

    QHostAddress ipv6AddressShift(const QHostAddress& , quint16);

signals:
};

#endif // SDPPARSER_H
