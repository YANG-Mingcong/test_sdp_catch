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

    QString textBoxData = ui->sdpInputArea->toPlainText();

    QByteArray textBoxDatagram = textBoxData.replace("\n", "\r\n").toUtf8();

    sdpFetch->sdpRawManuleAdd(textBoxDatagram);

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

