#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "sdpparser.h"

#include "sdpfetch.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btn_sdpParser_clicked();

    void on_btn_addSdp_clicked();

    void on_btn_delSdp_clicked();

    void on_btn_sdpParserTest_clicked();

private:
    Ui::MainWindow *ui;

    SdpFetch* sdpFetch;

    SdpParser* sdpParser;
};
#endif // MAINWINDOW_H
