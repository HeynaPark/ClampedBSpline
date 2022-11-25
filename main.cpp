#include <iostream>
#include <vector>
#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"

#pragma comment(lib, "opencv_core453.lib")
#pragma comment(lib, "opencv_highgui453.lib")
#pragma comment(lib, "opencv_imgcodecs453.lib")
#pragma comment(lib, "opencv_imgproc453.lib")

using namespace std;
using namespace cv;

Point2d Interpolate(double t, int degree, vector<Point2d> points, vector<int> knots);

Mat output = Mat::zeros(400, 400, CV_8UC3);

int main() {

	// input data
	vector<Point2d> data;
	data.push_back(Point2d(-1.0, 0.0));
	data.push_back(Point2d(-0.5, 0.5));
	data.push_back(Point2d(0.5, -0.5));
	data.push_back(Point2d(1.0, 0.0));

	for (int i = 0; i < data.size(); i++) {
		circle(output, data[i] * 100 + Point2d(200, 200), 3, Scalar(0, 0, 255), -1);

	}

	imshow("output", output);
	waitKey(10);

	// set parameter
	int degree = 2;
	vector<int> knots{ 0,0,0,1,2,2,2 };
	vector<Point2d> point;
	for (double t = 0; t < 1; t += 0.01) {
		point.push_back(Interpolate(t, degree, data, knots));
	}

	for (int i = 0; i < point.size(); i++) {
		circle(output, point[i] * 100 + Point2d(200, 200), 1, Scalar(0, 255, 0), -1);
	}
	cout << point << endl;

	imshow("output", output);
	waitKey(0);

	data.clear();
	return 0;
}

Point2d Interpolate(double t, int degree, vector<Point2d> points, vector<int> knots) {
	int i, j, s, l;
	int n = points.size();
	int d = 2;

	if (degree < 1) {
		printf("[BSpline] degree must be at lease 1");
	}
	if (degree > (n - 1)) {
		printf("[BSpline] degree must be less than or equal to point count - 1");
	}

	if (knots.size() != n + degree + 1) {
		printf("[BSpline] degree must be less than or equal to point count - 1");
	}

	int domain0 = degree;
	int domain1 = knots.size() - 1 - degree;

	float low = knots[domain0];
	float high = knots[domain1];
	t = t * (high - low) + low;
	//cout << t << endl;

	if (t<low || t>high) {
		printf("[BSpline] out of bounds ");
	}

	//
	for (s = domain0; s < domain1; s++) {
		if (t >= knots[s] && t <= knots[s + 1]) {
			//cout << "s " << s << endl;
			break;
		}
	}

	//convert points to homogeneous coordinates
	vector<vector<double>> v(n, vector<double>(3, 0));
	for (i = 0; i < n; i++) {
		for (j = 0; j < d; j++) {
			if (j == 0)
				v[i][j] = points[i].x;
			else
				v[i][j] = points[i].y;
		}
	}

	double alpha;
	double tx, ty;
	double ch;

	for (l = 1; l <= degree + 1; l++) {
		for (i = s; i > s - degree - 1 + l; i--) {
			alpha = (t - knots[i]) / (double)(knots[i + degree + 1 - l] - knots[i]);

			for (j = 0; j < d + 1; j++) {

				ch = (1 - alpha) * v[i - 1][j] + alpha * v[i][j];
				v[i][j] = (1 - alpha) * v[i - 1][j] + alpha * v[i][j];

				if (i == s && j == 2) {
					cout << s << endl;
					printf("%f\n", alpha);
					printf("%f\n", v[i][j]);
				}
			}
		}
	}
	tx = v[s][0];
	ty = v[s][1];
	//cout << v[s][2] << endl;

	//circle(output, pt[t] * 100 + Point2d(200, 200), 1, Scalar(0, 255, 0), -1);

	return Point2d(tx, ty);
}