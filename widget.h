#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QDir>
#include <QDebug>
#include <QMessageBox>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDesktopServices>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_btnDefautPath_clicked();

    void on_btnDownload_clicked();

    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

    void onFinished();

    void onReadyRead();

private:
    Ui::Widget *ui;

    // 网络管理
    QNetworkAccessManager * networkManager;

    // http请求返回
    QNetworkReply * reply;

    // 下载的文件
    QFile * downloadFile;
};
#endif // WIDGET_H
