#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    sdpFetch = new SdpFetch(this);
}

MainWindow::~MainWindow()
{
    delete ui;

    sdpFetch->deleteLater();
}

void MainWindow::on_btn_sdpParser_clicked()
{
    quint16 sdpRawMapsize = sdpFetch->getSdpRawMapSize();

    if(0==sdpRawMapsize)
    {
        qDebug() << "No SDP data";
    }
    else
    {
        for (int var = 0; var < sdpRawMapsize; ++var)
        {
            sdpParser = new SdpParser(this, sdpFetch->getSdpRawMapKeyList().at(var));
            sdpParser->sdpSplitTest();
            sdpParser->deleteLater();
        }

    }
}


void MainWindow::on_btn_addSdp_clicked()
{
    // QByteArray testSDP;
    // testSDP.append("v=0\r\no=- 1 2871560083 IN IP4 192.168.10.17\r\ns=BD12_stream_1\r\nt=0 0\r\nm=audio 5004 RTP/AVP 98\r\ni=Stream 1\r\nc=IN IP4 239.69.12.1/128\r\na=source-filter: incl IN IP4 239.69.12.1 192.168.10.17\r\na=rtpmap:98 L24/48000/8\r\na=sync-time:0\r\na=clock-domain:PTPv2 0\r\na=framecount:7\r\na=recvonly\r\na=mediaclk:direct=0\r\na=ts-refclk:ptp=IEEE1588-2008:A0-BB-3E-FF-FE-20-13-B8:0\r\na=ptime:0.14\r\na=maxptime:0.14\r\n");

    QString textBoxData = ui->sdpInputArea->toPlainText();

    QByteArray textBoxDatagram = textBoxData.replace("\n", "\r\n").toUtf8();

    sdpFetch->sdpRawManuleAdd(textBoxDatagram);

    // qDebug() << "testSDP_Data——QByteArray: " << testSDP;
    // qDebug() << "textBoxData——QString: " << textBoxData;
    // qDebug() << "textBoxData——QByteArray: " << textBoxData;


}


void MainWindow::on_btn_delSdp_clicked()
{
    QString textBoxData = ui->sdpInputArea->toPlainText();

    QByteArray textBoxDatagram = textBoxData.replace("\n", "\r\n").toUtf8();

    sdpFetch->sdpRawManuleDelete(textBoxDatagram);
}


void MainWindow::on_btn_sdpParserTest_clicked()
{
    QString textBoxData = ui->sdpInputArea->toPlainText();

    QByteArray textBoxDatagram = textBoxData.replace("\n", "\r\n").toUtf8();

    sdpParser = new SdpParser(this, textBoxDatagram);
    sdpParser->sdpSplitTest();
    ui->sdpParserOutput->setPlainText(sdpParser->getParserResult());
    sdpParser->deleteLater();

}

