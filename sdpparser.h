#ifndef SDPPARSER_H
#define SDPPARSER_H

#include <QObject>
#include "SdpStructures.h"

class SdpParser : public QObject
{
    Q_OBJECT
public:
    explicit SdpParser(QObject *parent = nullptr, const QByteArray& sdpRaw = QByteArray());

    void sdpSplitTest();

private:
    QByteArray sdpRaw;

    QByteArray sdpSessionDescription;
    QByteArray sdpTimeDescription;
    QByteArray sdpMediaDescription;

    void sdpSplitter(QByteArray);

signals:
};

#endif // SDPPARSER_H
