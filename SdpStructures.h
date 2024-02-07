#ifndef SDPSTRUCTURES_H
#define SDPSTRUCTURES_H

#include "QtNetwork/qhostaddress.h"
#include <QObject>

struct SdpOrigin
{
    QString username;
    QString sessId;
    qint16  sessVersion;
    QString nettype;
    QString addrtype;
    QString unicastAddress;
};

struct SdpConnectionData
{
    QString nettype;
    QString addrtype;
    QHostAddress connectionAddress;
    qint16 connctionTtl;
};

struct SdpAttributes
{
    QString attribute;
    QString value;
};

struct SdpSessionOption
{
    QString                     iSessionInformation;//* (session information);
    QString                     uUriOfDescription;  //* (URI of description); ONLY STRING,NO FUNCTION
    QString                     eEmailAddress;      //* (email address); ONLY STRING,NO FUNCTION
    QString                     pPhoneNumber;       //* (phone number); ONLY STRING,NO FUNCTION
    QVector<SdpConnectionData>  cConnectionInformation; //* (connection information -- not required if included in all media)
    QString                     bBandwidth;         //* (zero or more bandwidth information lines) ONLY STRING,NO FUNCTION
    QString                     zTimeZone;          //* (time zone adjustments) ONLY STRING,NO FUNCTION
    QString                     kEncryptionKey;     //* (encryption key) ONLY STRING,NO FUNCTION
    QVector<SdpAttributes>      aAttribute;         //* (zero or more session attribute lines)
};

struct SdpTimeing
{
    qint64                      startTime;
    qint64                      stopTime;
};


struct SdpMediaType //currently "audio", "video", "text", "application", and "message"
{
    enum ContentType {
        Audio,
        Video,
        Text,
        Application,
        Message
    };

    QString contentTypeString;

    SdpMediaType() : contentTypeString("audio") {};
    SdpMediaType(ContentType type) {

        switch (type) {
        case Audio:
            contentTypeString = "audio";
            break;
        case Video:
            contentTypeString = "video";
            break;
        case Text:
            contentTypeString = "text";
            break;
        case Application:
            contentTypeString = "application";
            break;
        case Message:
            contentTypeString = "message";
            break;
        default:
            contentTypeString = "";
            break;
        }
    }
};

struct SdpMediaTransportProtocol
{
    enum ContentProtocol {
        UDP,
        RTPAVP,
        RTPSAVP
    };

    QString contentTypeString;
    SdpMediaTransportProtocol() : contentTypeString("RTP/AVP") {};
    SdpMediaTransportProtocol(ContentProtocol type) {

        switch (type) {
        case UDP:
            contentTypeString = "udp";
            break;
        case RTPAVP:
            contentTypeString = "RTP/AVP";
            break;
        case RTPSAVP:
            contentTypeString = "RTP/SAVP";
            break;
        default:
            contentTypeString = "";
            break;
        }
    }
};

struct SdpMedia
{
    SdpMediaType                mediaType;
    quint16                     port;
    SdpMediaTransportProtocol   transportProtocol;
    QString                     fmt;//format

};

struct SdpMediaOption
{
    QString                     iMediaTitle;        //* (media title)
    QVector<SdpConnectionData>  cConnectionInformation; //(connection information -- optional if included at session level)
    QString                     bBandwidth;         //* (zero or more bandwidth information lines) ONLY STRING,NO FUNCTION
    QString                     kEncryptionKey;     //* (encryption key) ONLY STRING,NO FUNCTION
    QVector<SdpAttributes>      aAttribute;         //* (zero or more session attribute lines)
};

struct SDP
{
    int                         version;            //(protocol version)
    SdpOrigin                   origin;             //(originator and session identifier)

    QString                     sessionName;        //(session name)
    SdpSessionOption            sessionOptions;     //* (session information)

    SdpTimeing                  timing;             //(time the session is active)
    QString                     repeatTimes;        //*(zero or more repeat times) ONLY STRING,NO FUNCTION

    SdpMedia                    media;              //(media name and transport address)
    SdpMediaOption              mdiaOptions;        //*
};

#endif // SDPSTRUCTURES_H
