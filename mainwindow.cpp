#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QImage>
#include <QPixmap>
#include <QDebug>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->videoLabel->setText("請開啟圖片以套用 Dilation");
    ui->kernelSlider->setEnabled(false); // 開圖後才啟用
    ui->kernelSlider->setMinimum(1);
    ui->kernelSlider->setMaximum(31);
    ui->kernelSlider->setSingleStep(2);
    ui->kernelSlider->setValue(15);
    connect(ui->kernelSlider, &QSlider::valueChanged, this, &MainWindow::on_kernelSlider_valueChanged);

    connect(ui->radioButton_RECT, &QRadioButton::toggled, this, [=](bool){ on_kernelSlider_valueChanged(ui->kernelSlider->value()); });
    connect(ui->radioButton_ELLIPSE, &QRadioButton::toggled, this, [=](bool){ on_kernelSlider_valueChanged(ui->kernelSlider->value()); });
    connect(ui->radioButton_CROSS, &QRadioButton::toggled, this, [=](bool){ on_kernelSlider_valueChanged(ui->kernelSlider->value());});

    connect(ui->radioDilation, &QRadioButton::toggled, this, [=](bool checked){
        if (checked) {
            Morphological_Operation = cv::MORPH_DILATE;
            on_kernelSlider_valueChanged(ui->kernelSlider->value());
        }
    });

    connect(ui->radioErosion, &QRadioButton::toggled, this, [=](bool checked){
        if (checked) {
            Morphological_Operation = cv::MORPH_ERODE;
            on_kernelSlider_valueChanged(ui->kernelSlider->value());
        }
    });

    connect(ui->radioOpening, &QRadioButton::toggled, this, [=](bool checked){
        if (checked) {
            Morphological_Operation = cv::MORPH_OPEN;
            on_kernelSlider_valueChanged(ui->kernelSlider->value());
        }
    });

    connect(ui->radioClosing, &QRadioButton::toggled, this, [=](bool checked){
        if (checked) {
            Morphological_Operation = cv::MORPH_CLOSE;
            on_kernelSlider_valueChanged(ui->kernelSlider->value());
        }
    });


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_openButton_clicked() {
    QString filename = QFileDialog::getOpenFileName(this, "開啟圖片", "", "影像檔案 (*.png *.jpg *.bmp)");
    if (filename.isEmpty()) return;

    originalImage = cv::imread(filename.toStdString(), cv::IMREAD_GRAYSCALE);
    if (originalImage.empty()) {
        qDebug() << "無法載入圖片";
        return;
    }
    QImage qimg(originalImage.data, originalImage.cols, originalImage.rows, originalImage.step, QImage::Format_Grayscale8);

    if (!qimg.isNull()) {
        ui->videoLabel->setPixmap(QPixmap::fromImage(qimg).scaled(ui->videoLabel->size(), Qt::KeepAspectRatio));
    }

    ui->kernelSlider->setEnabled(true);
    on_kernelSlider_valueChanged(ui->kernelSlider->value());
}

void MainWindow::on_kernelSlider_valueChanged(int value)
{
    if (originalImage.empty()) return;

    int ksize = (value % 2 == 0) ? value + 1 : value;

    if (ui->sliderValueLabel)
        ui->sliderValueLabel->setText(QString("Kernel: %1").arg(ksize));

    int shapeType = cv::MORPH_RECT;
    if (ui->radioButton_ELLIPSE->isChecked()) shapeType = cv::MORPH_ELLIPSE;
    else if (ui->radioButton_CROSS->isChecked()) shapeType = cv::MORPH_CROSS;
    cv::Mat kernel = cv::getStructuringElement(shapeType, cv::Size(ksize, ksize));

    const int MORPH_NONE = -1;
    cv::Mat result;
    switch (Morphological_Operation) {
    case cv::MORPH_DILATE:
        cv::dilate(originalImage, result, kernel);
        break;
    case cv::MORPH_ERODE:
        cv::erode(originalImage, result, kernel);
        break;
    case cv::MORPH_OPEN:
    case cv::MORPH_CLOSE:
        cv::morphologyEx(originalImage, result, Morphological_Operation, kernel);
        break;
    case MORPH_NONE:
    default:
        result = originalImage.clone(); // fallback，不做任何處理
        break;
    }



    QImage qimg(result.data, result.cols, result.rows, result.step, QImage::Format_Grayscale8);
    ui->videoLabel_after->setPixmap(QPixmap::fromImage(qimg).scaled(ui->videoLabel_after->size(), Qt::KeepAspectRatio));
}

