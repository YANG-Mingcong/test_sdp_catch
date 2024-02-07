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

void MainWindow::on_pushButton_clicked()
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

