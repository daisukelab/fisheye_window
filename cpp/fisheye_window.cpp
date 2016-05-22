#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <stdio.h>
#include <vector>
#include <array>
#include <cmath>
#include <cassert>

using namespace std;

#define NUMOF_STOREABLE_MAPS 3

class FishEyeWindow {
private:
	int srcW_, srcH_, destW_, destH_;
	float al_, be_, th_, R_, zoom_;
	vector<cv::Mat *> mapXs_, mapYs_;
public:
	
    FishEyeWindow(int srcW, int srcH, int destW, int destH)
	: srcW_(srcW), srcH_(srcH), destW_(destW), destH_(destH),
	  al_(0), be_(0), th_(0), R_(srcW / 2.0), zoom_(1.0),
	  mapXs_(NUMOF_STOREABLE_MAPS, NULL), mapYs_(NUMOF_STOREABLE_MAPS, NULL) {}

	~FishEyeWindow() {
		array<vector<cv::Mat *> *, 2> maps = {&mapXs_, &mapYs_};
		for (int i = 0; i < maps.size(); i++) {
			for (vector<cv::Mat *>::iterator it = maps[i]->begin(); it != maps[i]->end(); it++) {
				delete *it;
			}
		}
	}

	void buildMap(float alpha, float beta, float theta, float zoom, int idx = 0) {
		assert(0 <= idx && idx < NUMOF_STOREABLE_MAPS);
		cv::Mat *mapX = new cv::Mat(destH_, destW_, CV_32FC1);
		cv::Mat *mapY = new cv::Mat(destH_, destW_, CV_32FC1);
		// # Set the angle parameters
		al_ = alpha;
		be_ = beta;
		th_ = theta;
		//R_ = R(R, R_)[R == None]
		zoom_ = zoom;
		// # Build the fisheye mapping
		float al = al_ / 180.0;
		float be = be_ / 180.0;
        float th = th_ / 180.0;
        float A = cos(th) * cos(al) - sin(th) * sin(al) * cos(be);
		float B = sin(th) * cos(al) + cos(th) * sin(al) * cos(be);
        float C = cos(th) * sin(al) + sin(th) * cos(al) * cos(be);
        float D = sin(th) * sin(al) - cos(th) * cos(al) * cos(be);
		float mR = zoom_ * R_;
		float mR2 = mR * mR;
		float mRsinBesinAl = mR * sin(be) * sin(al);
		float mRsinBecosAl = mR * sin(be) * cos(al);
		int centerV = int(destH_ / 2.0);
		int centerU = int(destW_ / 2.0);
		float centerY = srcH_ / 2.0;
		float centerX = srcW_ / 2.0;
		// # Fill in the map
		for (int absV = 0; absV < destH_; absV++) {
			float v = absV - centerV;
			float vv = v * v;
			for (int absU = 0; absU < destW_; absU++) {
				float u = absU - centerU;
				float uu = u * u;
				float upperX = R_ * (u * A - v * B + mRsinBesinAl);
				float lowerX = sqrt(uu + vv + mR2);
				float upperY = R_ * (u * C - v * D - mRsinBecosAl);
				float lowerY = lowerX;
				float x = upperX / lowerX + centerX;
				float y = upperY / lowerY + centerY;
				int _v = centerV <= v ? v : v + centerV;
				int _u = centerU <= u ? u : u + centerU;
				mapX->at<float>(_v, _u) = x;
				mapY->at<float>(_v, _u) = y;
			}
		}
		// # Append as new map
		if (mapXs_[idx] != NULL) delete mapXs_[idx];
		if (mapYs_[idx] != NULL) delete mapYs_[idx];
		mapXs_[idx] = mapX;
		mapYs_[idx] = mapY;
	}

	void getImage(cv::Mat &src, cv::Mat &dest, int idx) {
		cv::remap(src, dest, *mapXs_[idx], *mapYs_[idx],
				  CV_INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
	}
};

static void errorExit(const char *format, const char *arg)
{
    fprintf(stderr, "Error: ");
    fprintf(stderr, format, arg);
    fprintf(stderr, "\nexit.\n");
    exit(-1);
}

int main(int ac, char *av[])
{
    if (ac <= 1) {
		printf("Usage: ./%s your-fisheye-image-file\n", av[0]);
		return -1;
    }

	cv::Mat src_img = cv::imread(av[1]);
	if (!src_img.data) {
		errorExit("No such file: %s", av[1]);
		return 0;
    }
	cv::namedWindow("Original");
	cv::namedWindow("Result");
	cv::imshow("Original", src_img);
	cv::Mat dest_img(320, 320, src_img.type());

    FishEyeWindow few(src_img.rows, src_img.cols, 320, 320);
    float alpha = -270.0;
    float beta = 0.0;
    float theta = 270.0;
    float zoom = 1.0;

	printf("Hit followings to move your view:\n"
		   "'r' or 'f' to zoom\n"
		   "'g' or 't' to rotate alpha\n"
		   "'h' or 'y' to rotate beta\n"
		   "'j' or 'u' to rotate theta\n"
		   "'s' to save current view to ./result.png\n"
		   "\n"
		   "Hit ESC to exit.\n");

	while (true) {
		few.buildMap(alpha, beta, theta, zoom);
		few.getImage(src_img, dest_img, 0);
		cv::imshow("Result", dest_img);

		int key = cv::waitKey(0);
        if (key == 27) {
			break;
		}
		switch (key) {
			case 'r':	zoom -= 0.1; break;
			case 'f':	zoom += 0.1; break;
			case 'g':	alpha += 90; break;
			case 't':	alpha -= 90; break;
			case 'h':	beta += 45; break;
			case 'y':	beta -= 45; break;
			case 'j':	theta += 90; break;
			case 'u':	theta -= 90; break;
			case 's':	cv::imwrite("./result.png", dest_img); break;
		}
        printf("alpha=%f, beta=%f, theta=%f, zoom=%f\n", alpha, beta, theta, zoom);
	}

	cv::destroyAllWindows();

	return 0;
}
