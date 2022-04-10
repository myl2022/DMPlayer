#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "video.h"

#include <QWidget>
#include <QDebug>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_timer(nullptr)
{
    ui->setupUi(this);

    // 设置窗口没有边框
    setWindowFlags(Qt::FramelessWindowHint | windowFlags());

    m_video = new Video;

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(slot_update_time()));
    connect(m_video, SIGNAL(finished()), this, SLOT(slot_play_finished()));

}

MainWindow::~MainWindow()
{
    delete ui;

    delete m_video;
    m_video = nullptr;
}

void MainWindow::on_pushButton_clicked()
{
    m_video->video_play(true);
    m_timer->start(500);

    ui->pushButton->setStyleSheet("QPushButton#pushButton { \
                                  border-image: url(:/resource/pause.png); \
                              }");
}

void MainWindow::on_pushButton_2_clicked()
{
    m_video->video_stop();
}

void MainWindow::on_pushButton_3_clicked()
{
    // SDL 窗口嵌入 widget 窗口中
    m_video->set_video_window((void*)ui->video_label->winId());

    // 指定播放的视频文件
    m_video->set_video_file("D:\\workspace\\DMPlayer\\videoSamples\\ForrestGump.mp4");

    // 初始化播放环境
    bool ret = m_video->initialize();

    qDebug() << "The video duration: " <<m_video->duration();
    ui->horizontalSlider->setRange(0,  m_video->duration());
}

void MainWindow::on_pushButton_5_clicked()
{
    m_video->volume_down();
}

void MainWindow::on_pushButton_4_clicked()
{
    m_video->volume_up();

}

void MainWindow::slot_update_time()
{
    ui->horizontalSlider->setValue(m_video->play_time()*1000);
}

void MainWindow::on_pushButton_6_clicked()
{
    m_video->forward();
}

void MainWindow::on_pushButton_7_clicked()
{
    m_video->backward();
}

void MainWindow::slot_play_finished()
{
    qDebug() << "play finished";
    ui->pushButton->setStyleSheet("QPushButton#pushButton { \
                                  border-image: url(:/resource/play.png); \
                              }");
}
