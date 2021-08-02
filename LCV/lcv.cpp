#include <windows.h>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <vector>

using namespace cv;
using namespace std;

/*
* H 0 10
* S 60 100
* V 65 100
*/

Scalar redLow = { 0.0, 153, 165.75 };
Scalar redHigh = { 5.0, 255.0, 255.0 };

Mat
Hwnd2Mat(HWND hwnd, LONG width, LONG height, LONG offsetX, LONG offsetY)
{
  HDC windowDC = GetDC(hwnd);
  HDC windowCompDC = CreateCompatibleDC(windowDC);
  SetStretchBltMode(windowCompDC, COLORONCOLOR);
  RECT rect = {};
  GetClientRect(hwnd, &rect);
  LONG srcHeight = rect.bottom;
  LONG srcWidth = rect.right;
  Mat mat = {};
  mat.create(height, width, CV_8UC4);
  HBITMAP bitmap = CreateCompatibleBitmap(windowDC, width, height);
  BITMAPINFOHEADER  bitmapInfo = {};
  bitmapInfo.biSize = sizeof(BITMAPINFOHEADER);
  bitmapInfo.biWidth = width;
  bitmapInfo.biHeight = -height;
  bitmapInfo.biPlanes = 1;
  bitmapInfo.biBitCount = 32;
  bitmapInfo.biCompression = BI_RGB;
  bitmapInfo.biSizeImage = 0;
  bitmapInfo.biXPelsPerMeter = 0;
  bitmapInfo.biYPelsPerMeter = 0;
  bitmapInfo.biClrUsed = 0;
  bitmapInfo.biClrImportant = 0;
  SelectObject(windowCompDC, bitmap);
  StretchBlt(windowCompDC, 0, 0, width, height, windowDC, srcWidth - offsetX, srcHeight - offsetY, offsetX, offsetY, SRCCOPY);
  GetDIBits(windowCompDC, bitmap, 0, height, mat.data, (BITMAPINFO*)&bitmapInfo, DIB_RGB_COLORS);
  DeleteObject(bitmap);
  DeleteDC(windowCompDC);
  ReleaseDC(hwnd, windowDC);
  return mat;
}

INT
main()
{
  HWND desktop = GetDesktopWindow();
  INT key = 0;
  while (key != 27)
  {
    Mat image = Hwnd2Mat(desktop, 512, 512, 280, 280);
    Mat background = {};
    image.copyTo(background);
    cvtColor(image, image, COLOR_BGR2HSV);
    Mat mask = {};
    inRange(image, redLow, redHigh, mask);
    vector<vector<Point>> contours = {};
    findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    for (SIZE_T i = 0; i < contours.size(); ++i)
    {
      Rect bounds = boundingRect(contours[i]);
      if (bounds.area() > 350)
      {
        rectangle(background, bounds.tl(), bounds.br(), Scalar{ 0, 255, 0 }, 2);
      }
    }
    imshow("background", background);
    key = waitKey(60);
  }
  return 0;
}