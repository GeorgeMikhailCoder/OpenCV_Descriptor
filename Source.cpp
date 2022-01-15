#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "iostream"

using namespace cv;
using namespace std;

int histSize = 256;
int hist_w = 512, hist_h = 400;

vector<Mat> img2hist(Mat img)
{
    vector<Mat> bgr_planes;
    split(img, bgr_planes);


    float range[] = { 0, 256 }; //the upper boundary is exclusive
    const float* histRange[] = { range };
    bool uniform = true, accumulate = false;

    Mat b_hist, g_hist, r_hist;

    calcHist(&bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, histRange, uniform, accumulate);
    calcHist(&bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, histRange, uniform, accumulate);
    calcHist(&bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, histRange, uniform, accumulate);

    

    normalize(b_hist, b_hist, 0, hist_h, NORM_MINMAX, -1, Mat());
    normalize(g_hist, g_hist, 0, hist_h, NORM_MINMAX, -1, Mat());
    normalize(r_hist, r_hist, 0, hist_h, NORM_MINMAX, -1, Mat());

    vector<Mat> rgb_hist = {r_hist, g_hist, b_hist};
    return rgb_hist;
}

Mat hist2mat(vector<Mat> rgb_hist)
{

    Mat b_hist, g_hist, r_hist; 
    r_hist = rgb_hist[0];
    g_hist = rgb_hist[1];
    b_hist = rgb_hist[2];


    int bin_w = cvRound((double)hist_w / histSize);
    Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

    for (int i = 1; i < histSize; i++)
    {
        line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(b_hist.at<float>(i - 1))),
            Point(bin_w * (i), hist_h - cvRound(b_hist.at<float>(i))),
            Scalar(255, 0, 0), 2, 8, 0);
        line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(g_hist.at<float>(i - 1))),
            Point(bin_w * (i), hist_h - cvRound(g_hist.at<float>(i))),
            Scalar(0, 255, 0), 2, 8, 0);
        line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(r_hist.at<float>(i - 1))),
            Point(bin_w * (i), hist_h - cvRound(r_hist.at<float>(i))),
            Scalar(0, 0, 255), 2, 8, 0);
    }
    return histImage;
}

int diff(Mat img1, Mat img2)
{
    vector<Mat> rgb_hist1 = img2hist(img1);
    vector<Mat> rgb_hist2 = img2hist(img2);

    //  imshow("hist1", hist2mat(rgb_hist1));
    //  imshow("hist2", hist2mat(rgb_hist2));
    

    Mat diff, tmp1, tmp2;
    int normDiff=0;
    double eps=0;
    double res = 0;
    for (int i = 0; i < 3; i++)
    {
        diff = rgb_hist1[i] - rgb_hist2[i];
        diff = abs(diff);
        Scalar s = sum(diff);
        int si = s[0];
        double p = ((double)si) / (sum(rgb_hist1[i])[0] + sum(rgb_hist1[i])[0]);
        res += p / 3;
    }
    
    
    double percent_equal = 1 - res;
    return int(percent_equal*100);
}

void readConsole(int& porog, vector<string>& fileNames)
{
    cin >> porog;
    string buf;
    getline(cin,buf);
    buf = "start";

    for (; !buf.empty();)
    {
        getline(cin, buf);
        fileNames.push_back(buf);
    }
    fileNames.pop_back();
    fileNames.shrink_to_fit();
}

void processFiles(int porog, vector<string> fileNames)
{
    string fileName1, fileName2;
    Mat img1, img2;
    int numFiles = fileNames.size();

    for(int i=0;i<numFiles-1;i++)
        for (int j = i + 1; j < numFiles; j++)
        {
            fileName1 = fileNames[i];
            fileName2 = fileNames[j];

            img1 = imread(fileName1, cv::IMREAD_COLOR);
            img2 = imread(fileName2, cv::IMREAD_COLOR);

            if (img1.empty())
            {
                cout << "Error opening file: " << fileName1 << endl;
                break;
            }
                
            if(img2.empty())
            {
                cout << "Error opening file: " << fileName2 << endl;
                continue;
            }

            cvtColor(img1, img1, CV_8S);
            cvtColor(img2, img2, CV_8S);

            int eps = diff(img1, img2);
            if(eps>porog)
                cout <<fileName1<<"\t"<<fileName2 <<"\t"<<eps << "%" << endl;
        }
}

void task()
{
    int porog;
    vector<string> fileNames;
    
    readConsole(porog, fileNames);
    //  fileNames = {
    //      "C:\\Users\\george\\Desktop\\test_img\\test_pic_1.png",
    //      "C:\\Users\\george\\Desktop\\test_img\\test_pic_2.png",
    //      "C:\\Users\\george\\Desktop\\test_img\\test_pic_3.png",
    //  //    "C:\\Users\\george\\Desktop\\test_img\\test_pic_1_neg.png",
    //  //    "C:\\Users\\george\\Desktop\\test_img\\1.png",
    //  //    "C:\\Users\\george\\Desktop\\test_img\\2.jpg",
    //  //   "C:\\Users\\george\\Desktop\\test_img\\fig.png",
    //  //    "C:\\Users\\george\\Desktop\\test_img\\fig_neg.png"
    //  };
    //  porog = 0;
    processFiles(porog, fileNames);
}

int main()
{
    task();
    system("pause");
	return 0;
}