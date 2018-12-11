#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>

#include "function.h"

using namespace std;
using namespace cv;

Point mousepoint = Point(-1, -1);

Mat gray_all;
vector<vector<cv::Point>> contours_all;
vector<cv::Vec4i> hierarchy_all;

int main1() {
  RNG g_rng(12345);
  string filelist1[] = {
      "../boardtest/1/b.bmp", "../boardtest/1/y.bmp", "../boardtest/2/b.bmp",
      "../boardtest/2/y.bmp", "../boardtest/3/b.bmp", "../boardtest/3/y.bmp",
  };
  string filelist2[] = {"../boardtest/1/cali.bmp", "../boardtest/2/cali.bmp",
                        "../boardtest/3/cali.bmp"};

  /*Mat src = imread(filelist1[1]), temp;*/

  Mat temp;
  // Mat src = imread("../CSD/8888.jpg", 1);
  Mat src = imread("pic/", 1);
  float scale = 0.5;
  Size dsize = Size(src.cols * scale, src.rows * scale);

  Mat org = Mat(dsize, CV_32S);
  resize(src, org, dsize);

  int pre_x = -1;
  while (waitKey(30) != 27) {
    org.copyTo(temp); //������ʾ���������Լ���ʱ�ķ���
    namedWindow("org");
    setMouseCallback("org", on_mouse1, (void *)&org);
    imshow("org", org);

    //��temp���д�������ʾ
    // mousepoint = Point(100, 100);

    Mat bgr, hsv, dst;
    temp.convertTo(bgr, CV_32FC3, 1.0 / 255, 0);
    // cvtColor(bgr, hsv, CV_BGR2HSV);
    cvtColor(bgr, hsv, CV_BGR2Lab);
    // imshow("hsv", hsv);

    int x = mousepoint.x;
    // int y = mousepoint.y;
    if (x != -1 && x != pre_x) {

      Vec3f a = hsv.at<Vec3f>(mousepoint);
      Vec3d low, high;
      for (int i = 0; i < 3; i++) {
        if (i == 0) {
          double s = 1;
          float range = 50; // 15
          low[i] = (a[i] - range) * s;
          high[i] = (a[i] + range) * s;
          cout << "h:low:" << (double)low[i] << " ,high:" << (double)high[i];
        } else if (i == 1) {
          float s = 1;
          float range = 15; // 1  hsv
          low[i] = (a[i] - range) * s;
          high[i] = (a[i] + range) * s;
          cout << "s:low:" << (double)low[i] << " ,high:" << (double)high[i];
        } else if (i == 2) {
          float s = 1;
          float range = 15; // 0.2 hsv
          low[i] = (a[i] - range) * s;
          high[i] = (a[i] + range) * s;
          cout << "v:low:" << (double)low[i] << " ,high:" << (double)high[i]
               << endl;
        }
        cout << endl;
      }

      Mat mask;
      inRange(hsv, Scalar(low[0], low[1], low[2]),
              Scalar(high[0], high[1], high[2]), mask);
      dst = Mat::zeros(temp.size(), CV_32FC3);
      Vec3f ff = Vec3f(255, 255, 255);
      Vec3f f0 = Vec3f(0, 0, 0);
      for (int r = 0; r < bgr.rows; r++) {
        for (int c = 0; c < bgr.cols; c++) {
          if (mask.at<uchar>(r, c) == 255) {
            dst.at<Vec3f>(r, c) = ff;
          } else {
            dst.at<Vec3f>(r, c) = f0; // bgr.at<Vec3f>(r, c);
          }
        }
      }

      Mat save;
      dst.convertTo(save, CV_32FC3, 255, 0);

      save.convertTo(gray_all, CV_8UC3, 255.0);

      cvtColor(gray_all, gray_all, CV_BGR2GRAY);

      GaussianBlur(gray_all, gray_all, Size(3, 3), 0);
      threshold(gray_all, gray_all, 100, 255, THRESH_BINARY); // 90����

      Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));
      // dilate(gray_all, gray_all, element);
      // erode(gray_all, gray_all, element);

      namedWindow("img"); // CV_WINDOW_NORMAL
      imshow("img", gray_all);

      cv::findContours(gray_all, contours_all, hierarchy_all, CV_RETR_TREE,
                       cv::CHAIN_APPROX_NONE, cv::Point(0, 0));

      //������������
      int maxindex = 0;
      int maxsize = 0;
      for (int i = 0; i < contours_all.size(); i++) {
        RotatedRect rotaterect1 = minAreaRect(contours_all[i]);
        float width1 = rotaterect1.size.width;
        float height1 = rotaterect1.size.height;
        float rectarea1 = width1 * height1;
        float realarea1 = contourArea(contours_all[i]);
        float index1 = realarea1 / rectarea1;

        if (contours_all[i].size() > maxsize && index1 > 0.9) {
          maxsize = contours_all[i].size();
          maxindex = i;
        }
      }

      Mat temp = Mat::zeros(gray_all.size(), CV_8UC3);
      drawContours(temp, contours_all, maxindex, CV_RGB(255, 0, 0),
                   1); // rng.uniform(0,255)  ��CV_FILLED

      // namedWindow("������", 4);
      // imshow("������", temp);

      RotatedRect rotaterect = minAreaRect(contours_all[maxindex]);
      float width = rotaterect.size.width;
      float height = rotaterect.size.height;
      cout << "width:" << width << ",height:" << height << endl;

      //�������ζ�
      float rectarea = width * height;
      float realarea = contourArea(contours_all[maxindex]);
      float index = realarea / rectarea;
      cout << "index:" << index << endl;

      //����dst,Ϊʲô���治����
      // IplImage qImg;
      // qImg = IplImage(save);
      // cvSaveImage("888.jpg", &qImg);
      imwrite("8888.jpg", save);
    }
    pre_x = x;
  }
  return 0;
}

void on_mouse1(int event, int x, int y, int flags, void *ustc) {
  Mat &image =
      *(Mat *)
          ustc; //�����Ϳ��Դ���Mat��Ϣ�ˣ��ܻ���
  char temp[16];
  switch (event) {
  case CV_EVENT_LBUTTONDOWN: //��������
  {
    cout << "x:" << x << " ,y:" << y << endl;
    mousepoint = Point(x, y);

    //����ͼƬ���д���
    // string  str = "(" + to_string(mousepoint.x) + "," +
    // to_string(mousepoint.y) + ")"; putText(image, str, mousepoint,
    // FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0, 255));
  } break;
  default:
    break;
  }
}

//===============================================================================
Point ptL, ptR; //���껭�����ο����������յ�
Mat imageSource1, imageSourceCopy;
Mat ROI; //ԭͼ��Ҫ�޸�������ROI
Mat Gray;

Mat imageSource;

//����Ȧ��������ֵ����+Mask���ʹ���
int main2() {
  // imageSource1 = imread("../wireall/36.jpg");
  imageSource1 = imread("../wiretest/55.jpg");

  double scale = 0.2;
  Size dsize = Size(imageSource1.cols * scale, imageSource1.rows * scale);

  imageSource = Mat(dsize, CV_32S);
  resize(imageSource1, imageSource, dsize);

  if (!imageSource.data) {
    return -1;
  }
  imshow("ԭͼ", imageSource);
  setMouseCallback("ԭͼ", OnMouse2);
  waitKey();

  return 0;
}
void OnMouse2(int event, int x, int y, int flag, void *ustg) {
  if (event == CV_EVENT_LBUTTONDOWN) {
    ptL = Point(x, y);
    ptR = Point(x, y);
  }
  if (flag == CV_EVENT_FLAG_LBUTTON) {
    ptR = Point(x, y);
    imageSourceCopy = imageSource.clone();
    rectangle(imageSourceCopy, ptL, ptR, Scalar(255, 0, 0));
    imshow("ԭͼ", imageSourceCopy);
  }
  if (event == CV_EVENT_LBUTTONUP) {
    if (ptL != ptR) {
      ROI = imageSource(Rect(ptL, ptR));
      imshow("ROI", ROI);
      waitKey();
    }
  }
  //���������Ҽ���ʼͼ���޸�
  if (event == CV_EVENT_RBUTTONDOWN) {
    imageSourceCopy = ROI.clone();
    Mat imageGray;
    cvtColor(ROI, imageGray, CV_RGB2GRAY); //ת��Ϊ�Ҷ�ͼ
    Mat imageMask = Mat(ROI.size(), CV_8UC1, Scalar::all(0));

    //ͨ����ֵ��������Mask
    threshold(imageGray, imageMask, 235, 255, CV_THRESH_BINARY);
    Mat Kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
    dilate(imageMask, imageMask, Kernel); //��Mask���ʹ���
    inpaint(ROI, imageMask, ROI, 9, INPAINT_TELEA); //ͼ���޸�
    imshow("Mask", imageMask);
    imshow("�޸���", imageSource);
  }
}
