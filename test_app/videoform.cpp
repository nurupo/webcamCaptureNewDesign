#include "videoform.h"
#include "ui_videoform.h"

using namespace std::placeholders; //for std::bind _1

//TO SAVE mjpg to file
#include <fstream>
using namespace std;
std::fstream outfile;
/////////////////////

#include <qdebug.h>
#include <windows.h>


VideoForm::VideoForm(CameraInterface &camera, int width, int height, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VideoForm),
    camera(camera),
    isCapturing(false)
{
    ui->setupUi(this);
    this->setFixedSize(width, height);
    this->ui->videoLabel->setFixedSize(width, height);
}

VideoForm::~VideoForm()
{

    camera.stop();
    Sleep(1000);
    delete ui;
}

void VideoForm::FrameCaptureCallback(Frame &frame)
{
    this->ui->videoLabel->setPixmap(QPixmap::fromImage(YUV422toRGBA32(frame)));

//    //TO SAVE mjpg to file
//    /// Test of mjpeg writing.
//    printf("Frame callback: %lu bytes, stride: %lu \n", buffer.nbytes, buffer.stride[0]);
//    static int count = 0;
//    static char fname[1024];

//    sprintf(fname, "test_%04d.jpeg", count);

//    std::ofstream ofs(fname, std::ios::out | std::ios::binary);
//    ofs.write((char*)buffer.plane[0], buffer.nbytes);
//    ofs.close();
//    count++;
}

void VideoForm::setCapturingStatus(bool isCapturing)
{
    this->isCapturing = isCapturing;
}

QImage VideoForm::YUV422toRGBA32(Frame &frame)
{
    const unsigned char *plane = frame.plane[0];
    size_t frameSize = frame.nbytes;

    size_t height = frame.height[0];
    size_t width = frame.width[0];

    std::vector<int> redContainer;
    std::vector<int> greenContainer;
    std::vector<int> blueContainer;

    // Loop through 4 bytes at a time
    int cnt = -1;

    for (unsigned int i = 0 ; i <= frameSize - 4 ; i += 4) {
        // Extract yuv components
        int y1  = (int)plane[i];
        int u = (int)plane[i + 1];
        int y2  = (int)plane[i + 2];
        int v = (int)plane[i + 3];

        // Define the RGB
        int r1 = 0 , g1 = 0 , b1 = 0;
        int r2 = 0 , g2 = 0 , b2 = 0;
        // u and v are +-0.5
        u -= 128;
        v -= 128;

        // Conversion
        r1 = y1 + 1.403 * v;
        g1 = y1 - 0.344 * u - 0.714 * v;
        b1 = y1 + 1.770 * u;
        r2 = y2 + 1.403 * v;
        g2 = y2 - 0.344 * u - 0.714 * v;
        b2 = y2 + 1.770 * u;

        // Increment by one so we can insert
        cnt += 1;

        // Append to the array holding our RGB Values
        redContainer.push_back(r1);
        greenContainer.push_back(g1);
        blueContainer.push_back(b1);

        // Increment again since we have 2 pixels per uv value
        cnt += 1;

        // Store the second pixel
        redContainer.push_back(r2);
        greenContainer.push_back(g2);
        blueContainer.push_back(b2);
    }

    QImage rgbImage = QImage((int)width, (int)height, QImage::Format_RGBA8888);
    int pixelCounter = -1;

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            pixelCounter += 1;
            rgbImage.setPixel(j, i, qRgb(redContainer.at(pixelCounter) ,  greenContainer.at(pixelCounter) ,
                                         blueContainer.at(pixelCounter)));
        }
    }

    return rgbImage;
}

FrameCallback VideoForm::getFrameCallback()
{
    return std::bind(&VideoForm::FrameCaptureCallback, this, _1);
}
