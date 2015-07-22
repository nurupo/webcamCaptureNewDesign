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
    explicit CameraForm(std::unique_ptr<CameraInterface> camera, QWidget *parent = 0);
    ~CameraForm();

private slots:
    void on_captureVideoBtb_clicked();
    std::string CameraForm::formatToString(PixelFormat format);
    void fillCameraCapabilitiesCB();
    void fillVideoPropertySettings();

    void changeBrightnessLabel(int value);
    void changeContrastLabel(int value);
    void changeSaturationLabel(int value);

    void on_tabWidget_currentChanged(int index);
    void on_resolutionComboBox_currentIndexChanged(int index);
    void on_formatComboBox_currentIndexChanged(int index);

    void on_applyImageSettingsButton_clicked();

private:
    Ui::CameraForm *ui;
    std::unique_ptr<CameraInterface> camera;
    std::vector<CapabilityFormat> capabilityList;
    VideoForm *videoForm;
};

#endif // CAMERAFORM_H
