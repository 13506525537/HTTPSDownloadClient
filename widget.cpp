#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    networkManager = new QNetworkAccessManager(this);
    ui->progressBar->setValue(0);
    ui->checkBox->setChecked(true);
}

Widget::~Widget()
{
    delete ui;
}

// 获取默认路径
void Widget::on_btnDefautPath_clicked()
{
    QString currentPath = QDir::currentPath();
    QDir dir(currentPath);
    QString newPath = currentPath + "/download/";

    if (!dir.exists(newPath)){
        dir.mkdir(newPath);
    }
    ui->lePath->setText(newPath);
}

// 根据URL下载文件
void Widget::on_btnDownload_clicked()
{
    QString urlStr = ui->leUrl->text().trimmed(); // 获取除去空格的文本
    if (urlStr.isEmpty()){
        QMessageBox::information(this, "提示", "请先输入URL地址");
        return;
    }

    QUrl newUrl = QUrl::fromUserInput(urlStr);
    if (!(newUrl.isValid())){
        QMessageBox::information(this, "提示",
                                 QString("输入的地址无效:%1 \n 错误信息:%2").arg(urlStr, newUrl.errorString()));
        return;
    }
    else
        qDebug() << "地址有效";

    // 判断路径是否为空
    QString path = ui->lePath->text().trimmed();
    if (path.isEmpty()){
        QMessageBox::information(this, "提示", "请输入文件保存路径");
        return;
    }

    // 判断路径是否存在
    QDir dir(path);
    if (!dir.exists())
    {
        QMessageBox::information(this, "提示", "文件保存地址无效");
        return;
    }
    else{
        qDebug() << "文件夹存在";
    }
    // 获取新地址
    QString fileName = (newUrl.fileName().isEmpty()? "new": newUrl.fileName())+ ".jpg";

    downloadFile = new QFile(dir.absoluteFilePath(fileName));

    // 判断原文件是否存在
    if (downloadFile->exists()){
        downloadFile->remove();
    }

    if (downloadFile->open(QIODevice::WriteOnly))
    {
        QNetworkRequest request(newUrl);
        request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
        // 发送请求
        reply = networkManager->get(request);

        // 连接reply信号槽
        connect(reply, &QNetworkReply::downloadProgress, this, &Widget::onDownloadProgress);
        connect(reply, &QNetworkReply::finished, this, &Widget::onFinished);
        connect(reply, &QNetworkReply::readyRead, this, &Widget::onReadyRead);

        if (reply->error() == QNetworkReply::NoError){
            ui->btnDownload->setEnabled(false);
        }
        else{
            QMessageBox::information(this, "提示", "请求失败");
        }
    }
    else{
        QMessageBox::information(this, "提示", "文件打开失败");
        return;
    }

}

// 处理进度条
void Widget::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    ui->progressBar->setMaximum(bytesTotal);
    ui->progressBar->setValue(bytesReceived);
}

void Widget::onFinished()
{
    ui->btnDownload->setEnabled(true);
    QFileInfo fileInfo(downloadFile->fileName());
    qDebug() << downloadFile->fileName();
    QMessageBox::information(this, "提示", "下载完成");

    downloadFile->close();

    // 删除reply和downloadFile
    delete downloadFile;
    downloadFile = nullptr;

    reply->deleteLater();

    // 判断是否需要打开
    if (ui->checkBox->isChecked()){
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
    }

}

void Widget::onReadyRead()
{
    downloadFile->write(reply->readAll());
}

