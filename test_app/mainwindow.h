#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <memory>
#include <vector>

#include <backend_implementation.h>
#include <backend_interface.h>
#include <camera_information.h>
#include <camera_interface.h>
#include <capability.h>

using namespace webcam_capture;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void CameraEventCaptureCallback(CameraInformation information, CameraConnectionStatus status);

private slots:
    void on_createBackendBtn_clicked();
    void on_createCameraBtn_clicked();
    void on_cameraListComboBox_currentIndexChanged(int index);
    void on_deleteBackendBtn_clicked();

    void on_stopNotificationsButton_clicked();

    void on_startNotificationsButton_clicked();

private:
    void fillFrameworkListCB();
    void fillCameraListCB();
    std::string formatToString(PixelFormat format);


    std::vector<BackendImplementation> backendList;
    std::vector<CameraInformation> cameraInfoList;
    std::unique_ptr<BackendInterface> backend;

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
