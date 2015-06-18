#ifndef CAMERAFORM_H
#define CAMERAFORM_H

#include <QWidget>
#include <camera_interface.h>
#include "videoform.h"

using namespace webcam_capture;

namespace Ui {
class CameraForm;
}

class CameraForm : public QWidget
{
    Q_OBJECT

public:
    explicit CameraForm(CameraInterface* camera, QWidget *parent = 0);
    ~CameraForm();
    void fillCameraCapabilitiesCB();

private slots:
    void on_captureVideoBtb_clicked();
    std::string  CameraForm::formatToString(Format format);

private:
    Ui::CameraForm *ui;
    CameraInterface* camera;
    std::vector<Capability> capabilityList;
    VideoForm *videoForm;
};

#endif // CAMERAFORM_H
