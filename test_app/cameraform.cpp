#include "cameraform.h"
#include "ui_cameraform.h"


CameraForm::CameraForm(std::unique_ptr<CameraInterface> camera, QWidget *parent) :
    camera(std::move(camera)),
    QWidget(parent),
    ui(new Ui::CameraForm)
{
    ui->setupUi(this);
    videoForm = NULL;

    fillCameraCapabilitiesCB();

    connect(this->ui->brigtnessSlider, SIGNAL(valueChanged(int)), this, SLOT(changeBrightnessLabel(int)));
    connect(this->ui->saturationSlider, SIGNAL(valueChanged(int)), this, SLOT(changeSaturationLabel(int)));
    connect(this->ui->contrastSlider, SIGNAL(valueChanged(int)), this, SLOT(changeContrastLabel(int)));
}

CameraForm::~CameraForm()
{
    delete ui;
}


void CameraForm::fillCameraCapabilitiesCB()
{
    this->ui->formatComboBox->clear();
    this->ui->resolutionComboBox->clear();
    this->ui->fpsComboBox->clear();

    capabilityList = camera->getCapabilities();

    for (int i = 0; i < capabilityList.size(); i++) {
        this->ui->formatComboBox->addItem(formatToString(capabilityList.at(i).getPixelFormat()).c_str());
    }
}

void CameraForm::on_formatComboBox_currentIndexChanged(int index)
{
    if (index == -1) {
        return;
    }

    this->ui->resolutionComboBox->clear();
    this->ui->fpsComboBox->clear();

    std::vector<CapabilityResolution> resolutions = capabilityList.at(index).getResolutions();

    for (int j = 0; j < resolutions.size(); j++) {
        QString str = QString("%2 x %3").
                      arg(resolutions.at(j).getWidth()).
                      arg(resolutions.at(j).getHeight());
        this->ui->resolutionComboBox->addItem(str);
    }
}

void CameraForm::on_resolutionComboBox_currentIndexChanged(int index)
{
    if (index == -1) {
        return;
    }

    this->ui->fpsComboBox->clear();
    std::vector<CapabilityResolution> resolutions = capabilityList.
            at(this->ui->formatComboBox->currentIndex()).getResolutions();
    std::vector<CapabilityFps> fpsVector = resolutions.
                                           at(index).getFpses();

    for (int k = 0; k < fpsVector.size(); k++) {
        QString str = QString("%1").
                      arg(fpsVector.at(k).getFps());
        this->ui->fpsComboBox->addItem(str);
    }
}

void CameraForm::on_tabWidget_currentChanged(int index)
{
    if (index == 1) {
        fillVideoPropertySettings();
    }
}


void CameraForm::fillVideoPropertySettings()
{

    //Brightness
    VideoPropertyRange vprBrightness;

    if (!camera->getPropertyRange(VideoProperty::Brightness, vprBrightness)) {
        //CAN'T get VideoPropertyRange
        return;
    }

    int value;
    this->ui->brigtnessSlider->setRange(vprBrightness.getMinimum(),
                                        vprBrightness.getMaximum());
    this->ui->brigtnessSlider->setTickInterval(vprBrightness.getStepSize());
    this->ui->minBrightnessLabel->setText(QString::number(vprBrightness.getMinimum()));
    this->ui->maxBrightnessLabel->setText(QString::number(vprBrightness.getMaximum()));
    value = camera->getProperty(VideoProperty::Brightness);
    this->ui->brigtnessSlider->setValue(value);
    this->changeBrightnessLabel(value);

    //Contrast
    VideoPropertyRange vprContrast;

    if (!camera->getPropertyRange(VideoProperty::Contrast, vprContrast)) {
        //CAN'T get VideoPropertyRange
        return;
    }

    this->ui->contrastSlider->setRange(vprContrast.getMinimum(),
                                       vprContrast.getMaximum());
    this->ui->contrastSlider->setTickInterval(vprContrast.getStepSize());
    this->ui->minContrastLabel->setText(QString::number(vprContrast.getMinimum()));
    this->ui->maxContrastLabel->setText(QString::number(vprContrast.getMaximum()));
    value = camera->getProperty(VideoProperty::Contrast);
    this->ui->contrastSlider->setValue(value);
    this->changeContrastLabel(value);

    //Saturation
    VideoPropertyRange vprSaturation;

    if (!camera->getPropertyRange(VideoProperty::Saturation, vprSaturation)) {
        //CAN'T get VideoPropertyRange
        return;
    }

    this->ui->saturationSlider->setRange(vprSaturation.getMinimum(),
                                         vprSaturation.getMaximum());
    this->ui->saturationSlider->setTickInterval(vprSaturation.getStepSize());
    this->ui->minSaturationLabel->setText(QString::number(vprSaturation.getMinimum()));
    this->ui->maxSaturationLabel->setText(QString::number(vprSaturation.getMaximum()));
    value = camera->getProperty(VideoProperty::Saturation);
    this->ui->saturationSlider->setValue(value);
    this->changeSaturationLabel(value);
}

void CameraForm::changeBrightnessLabel(int value)
{
    this->ui->currentBrightnessLabel->setText(QString::number(value));
}

void CameraForm::changeSaturationLabel(int value)
{
    this->ui->currentSaturationLabel->setText(QString::number(value));
}
void CameraForm::changeContrastLabel(int value)
{
    this->ui->currentContrastLabel->setText(QString::number(value));
}

void CameraForm::on_captureVideoBtb_clicked()
{


    CapabilityFormat capFormat = capabilityList.at(this->ui->formatComboBox->currentIndex());
    CapabilityResolution capResolution = capabilityList.at(this->ui->formatComboBox->currentIndex()).
                                         getResolutions().at(this->ui->resolutionComboBox->currentIndex());
    CapabilityFps capFps = capabilityList.at(this->ui->formatComboBox->currentIndex()).
                           getResolutions().at(this->ui->resolutionComboBox->currentIndex()).
                           getFpses().at(this->ui->fpsComboBox->currentIndex());

    videoForm = new VideoForm(*camera, capResolution.getWidth(), capResolution.getHeight());
    videoForm->setAttribute(Qt::WA_DeleteOnClose);
    videoForm->show();

    if (camera->start(capFormat.getPixelFormat(), capResolution.getWidth(), capResolution.getHeight(), capFps.getFps(), videoForm->getFrameCallback()) < 0) {
        delete videoForm;
    } else {
        videoForm->setCapturingStatus(true);
    }
}

void CameraForm::on_applyImageSettingsButton_clicked()
{
    camera->setProperty(VideoProperty::Brightness, this->ui->brigtnessSlider->value());
    camera->setProperty(VideoProperty::Contrast, this->ui->contrastSlider->value());
    camera->setProperty(VideoProperty::Saturation, this->ui->saturationSlider->value());
}

std::string  CameraForm::formatToString(PixelFormat format)
{
    switch (format) {
        ///Uncompressed RGB Formats
        case PixelFormat::RGB8:
            return "RGB8";

        case PixelFormat::RGB555:
            return "RGB555";

        case PixelFormat::RGB565:
            return "RGB565";

        case PixelFormat::RGB24:
            return "RGB24";

        case PixelFormat::RGB32:
            return "RGB32";

        case PixelFormat::ARGB32:
            return "ARGB32";

        ///YUV Formats: 8-Bit and Palettized
        case PixelFormat::AI44:
            return "AI44";

        case PixelFormat::AYUV:
            return "AYUV";

        case PixelFormat::I420:
            return "I420";

        case PixelFormat::IYUV:
            return "IYUV";

        case PixelFormat::NV11:
            return "NV11";

        case PixelFormat::NV12:
            return "NV12";

        case PixelFormat::UYVY:
            return "UYVY";

        case PixelFormat::Y41P:
            return "Y41P";

        case PixelFormat::Y41T:
            return "Y41T";

        case PixelFormat::Y42T:
            return "Y42T";

        case PixelFormat::YUY2:
            return "YUY2";

        case PixelFormat::YV12:
            return "YV12";
            \

        ///YUV Formats: 10-Bit and 16-Bit
        case PixelFormat::P010:
            return "P010";

        case PixelFormat::P016:
            return "P016";

        case PixelFormat::P210:
            return "P210";

        case PixelFormat::P216:
            return "P216";

        case PixelFormat::v210:
            return "v210";

        case PixelFormat::v216:
            return "v216";

        case PixelFormat::v40:
            return "v40";

        case PixelFormat::Y210:
            return "Y210";

        case PixelFormat::Y216:
            return "Y216";

        case PixelFormat::Y40:
            return "Y40";

        case PixelFormat::Y416:
            return "Y416";

        ///Encoded Video Types
        case PixelFormat::DV25:
            return "DV25";

        case PixelFormat::DV50:
            return "DV50";

        case PixelFormat::DVC:
            return "DVC";

        case PixelFormat::DVH1:
            return "DVH1";

        case PixelFormat::DVHD:
            return "DVHD";

        case PixelFormat::DVSD:
            return "DVSD";

        case PixelFormat::DVSL:
            return "DVSL";

        case PixelFormat::H263:
            return "H263";

        case PixelFormat::H264:
            return "H264";

        case PixelFormat::H264_ES:
            return "H264_ES";

        case PixelFormat::HEVC:
            return "HEVC";

        case PixelFormat::HEVC_ES:
            return "HEVC_ES";

        case PixelFormat::M4S2:
            return "M4S2";

        case PixelFormat::MJPG:
            return "MJPG";

        case PixelFormat::MP43:
            return "MP43";

        case PixelFormat::MP4S:
            return "MP4S";

        case PixelFormat::MP4V:
            return "MP4V";

        case PixelFormat::MPEG2:
            return "MPEG2";

        case PixelFormat::MPG1:
            return "MPG1";

        case PixelFormat::MSS1:
            return "MSS1";

        case PixelFormat::MSS2:
            return "MSS2";

        case PixelFormat::WMV1:
            return "WMV1";

        case PixelFormat::WMV2:
            return "WMV2";

        case PixelFormat::WMV3:
            return "WMV3";

        case PixelFormat::WVC1:
            return "WVC1";

        case PixelFormat::O420:
            return "420O";

        default:
            return "Unknown format";
    }
}


