#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QPixmap>
#include <opencv2/opencv.hpp>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_openButton_clicked();
    void on_kernelSlider_valueChanged(int value);


private:
    Ui::MainWindow *ui;
    cv::Mat originalImage;
    const int MORPH_NONE = -1;
    int Morphological_Operation = MORPH_NONE;
};

#endif // MAINWINDOW_H
