#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Solid To Frequency");

#if defined(Q_OS_IOS) || defined(Q_OS_ANDROID)    
    ui->m_textStatus->setStyleSheet("font-size: 12pt; color: #cccccc; background-color: #003333;");
#else    
    ui->m_textStatus->setStyleSheet("font-size: 18pt; color: #cccccc; background-color: #003333;");
#endif
    ui->graphicsView->setStyleSheet("font-size: 24pt; color:#ECF0F1; background-color: #212F3C; padding: 6px; spacing: 6px;");
    ui->pushExit->setStyleSheet("font-size: 24pt; font: bold; color: #ffffff; background-color: #336699;");    

    //QString currentTime = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
    ui->graphicsView->setScene(new QGraphicsScene(this));
    QScreen *primaryScreen = QGuiApplication::primaryScreen();
    QRect screenGeometry = primaryScreen->availableGeometry();
    setGeometry(screenGeometry);

    ui->graphicsView->scene()->addItem(&pixmap);

    rppg = new ProcessFrame();
    connect(rppg, &ProcessFrame::sendInfo, this, &MainWindow::printInfo);
    rppg->load(0);

    m_frames = new Frames();
    connect(m_frames, &Frames::sendInfo, this, &MainWindow::printInfo);
    connect(m_frames, &Frames::frameCaptured, this, &MainWindow::processFrame);
    m_frames->initCam();
}

MainWindow::~MainWindow()
{
    if(m_frames)
        delete m_frames;

    if(rppg)
        delete rppg;

    delete ui;
}

void MainWindow::createFile(const QString &fileName)
{
    QString data;
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Error: Could not open file for reading: " << fileName;
        return;
    } else {
        data = file.readAll();
        file.close();
    }

    QString filename = fileName.mid(fileName.lastIndexOf("/") + 1);
    QString tempFilePath;

#if defined(Q_OS_IOS) || defined(Q_OS_ANDROID)
    QString iosWritablePath = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    tempFilePath = iosWritablePath + "/" + filename;
#else
    tempFilePath = filename;
#endif

    QFile temp(tempFilePath);

    if (temp.exists()) {
        qDebug() << "Warning: File already exists: " << tempFilePath;
        return;
    }

    if (temp.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&temp);
        // Write the data to the file
        stream << data;
        // Close the file
        temp.close();
        qDebug() << "Data written to file: " << tempFilePath;
    } else {
        // Handle the case where the file could not be opened
        qDebug() << "Error: Could not open file for writing: " << tempFilePath;
    }
}


void MainWindow::processFrame(QVideoFrame &frame)
{
    if (frame.isValid())
    {
        QVideoFrame cloneFrame(frame);
        cloneFrame.map(QVideoFrame::ReadOnly);

        QImage img = cloneFrame.toImage();
        cloneFrame.unmap();

        img = img.convertToFormat(QImage::Format_RGB888);
        Mat frameRGB(img.height(),
                     img.width(),
                     CV_8UC3,
                     img.bits(),
                     img.bytesPerLine());

        if(!frameRGB.empty())
        {

            rppg->processFrame(frameRGB);

            QImage img_face((uchar*)frameRGB.data, frameRGB.cols, frameRGB.rows, frameRGB.step, QImage::Format_RGB888);
            pixmap.setPixmap( QPixmap::fromImage(img_face));
            ui->graphicsView->fitInView(&pixmap, Qt::KeepAspectRatioByExpanding);
        }
    }
}

void MainWindow::processImage(QImage &img_face)
{    
    pixmap.setPixmap( QPixmap::fromImage(img_face));
    ui->graphicsView->fitInView(&pixmap, Qt::KeepAspectRatio);
}

void MainWindow::printInfo(QString info)
{
    ui->m_textStatus->append(info);
}

void MainWindow::on_pushExit_clicked()
{
    qApp->exit();
}

