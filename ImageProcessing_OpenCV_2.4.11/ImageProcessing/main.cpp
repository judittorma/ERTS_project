#include <windows.h>
#include <iostream>
#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

/**************************************** types ***************************************/

/*
* KernelType::GaussianBlur		blurs the original image
* KernelType::BoxBlur			blurs the original image
* KernelType::UnsharpMasking	sharpens the original image
* KernelType::Laplacian			detects edge of the original image
* KernelType::Identity			applies a 5x5 Identity kernel on the original image
*								the result is actually the original image
* KernelType::None				means filtering is not requested
*/

enum class KernelType { GaussianBlur, BoxBlur, UnsharpMasking, Laplacian, Identity, None };

/********************************** helper functions **********************************/

// writes to (Debug) Output (for Microsoft Visual Studio 2015)
// example usage: DBOUT("s = " << s << std::endl);
#define DBOUT(s)						  \
{										  \
   std::wostringstream os;				  \
   os << s;								  \
   OutputDebugStringW(os.str().c_str());  \
}

// writes matrix into a text file
void writeMatToFile(cv::Mat& m, const char* filename)
{
	// writes matrix into a text file if its values are uchars or floats
	if (m.type() != CV_8UC1  && m.type() != CV_8UC2  &&
		m.type() != CV_8UC3  && m.type() != CV_8UC4  &&
		m.type() != CV_32FC1 && m.type() != CV_32FC2 &&
		m.type() != CV_32FC3 && m.type() != CV_32FC4)
		return;

	std::ofstream fout(filename);

	if (!fout)
	{
		std::cout << "Error occurred while opening the file." << std::endl;
		return;
	}

	for (int i = 0; i < m.rows; ++i)
	{
		for (int j = 0; j < m.cols; ++j)
		{
			if (m.type() == CV_8UC1 || m.type() == CV_8UC2 || 
				m.type() == CV_8UC3 || m.type() == CV_8UC4)
				fout << m.at<uchar>(i, j) << "\t";
			else fout << m.at<float>(i, j) << "\t";
		}
		fout << std::endl;
	}

	fout.close();
}

/************************************** functions *************************************/

// loads an image from a file
cv::Mat3b ReadImageIntoMatrix(const std::string &filename)
{
	return cv::imread(filename);
}

// returns 5 x 5 Gaussian blur
//                  1   4   6   4   1
//                  4  16  24  16   4
//    1 / 256   *   6  24  36  24   6
//                  4  16  24  16   4
//                  1   4   6   4   1
cv::Mat Get5x5GaussianKernel()
{
	// first column vector (5 x 1) of Gaussian kernel
	cv::Mat k = (cv::Mat_<float>(5, 1) << 1, 4, 6, 4, 1);
	// transpose of vector k
	cv::Mat kt = k.t();
	// kernel (N x N, here: 5 x 5)
	cv::Mat Kernel = k * kt;
	// divide each kernel elements by the sum of kernel elements
	Kernel = (1 / cv::sum(Kernel)[0]) * Kernel;
	return Kernel;
}

// returns 5 x 5 Box blur
//                 1   1   1   1   1
//                 1   1   1   1   1
//    1 / 25   *   1   1   1   1   1
//                 1   1   1   1   1
//                 1   1   1   1   1
cv::Mat Get5x5BoxKernel()
{
	cv::Mat Kernel = cv::Mat::ones(5, 5, CV_32FC1);
	// divide each kernel elements by the sum of kernel elements
	Kernel = (1 / cv::sum(Kernel)[0]) * Kernel;
	return Kernel;
}

// returns 5 x 5 Unsharp masking
//                   1     4     6     4    1
//                   4    16    24    16    4
//    -1 / 256   *   6    24  -476    24    6
//                   4    16    24    16    4
//                   1     4     6     4    1
cv::Mat Get5x5UnsharpKernel()
{
	// Unsharp masking is based on the Gaussian kernel
	cv::Mat Kernel = Get5x5GaussianKernel();
	Kernel = -1 * Kernel;
	Kernel.at<float>(Kernel.rows / 2, Kernel.cols / 2) = 476.0 / 256.0;
	return Kernel;
}

// returns 5 x 5 Laplacian kernel
//    -1   -1   -1   -1   -1
//    -1   -1   -1   -1   -1
//    -1   -1   24   -1   -1
//    -1   -1   -1   -1   -1
//    -1   -1   -1   -1   -1
cv::Mat Get5x5LaplacianKernel()
{
	cv::Mat Kernel = cv::Mat::ones(5, 5, CV_32FC1);
	Kernel = -1 * Kernel;
	Kernel.at<float>(Kernel.rows / 2, Kernel.cols / 2) = 24;
	return Kernel;
}

// returns 5 x 5 Identity kernel
//    0  0  0  0  0
//    0  0  0  0  0
//    0  0  1  0  0
//    0  0  0  0  0
//    0  0  0  0  0
cv::Mat Get5x5IdentityKernel()
{
	cv::Mat Kernel = cv::Mat::zeros(5, 5, CV_32FC1);
	Kernel.at<float>(Kernel.rows / 2, Kernel.cols / 2) = 1.0;
	return Kernel;
}

// returns a 5 x 5 kernel
cv::Mat Get5x5Kernel(const KernelType& kernelType)
{
	if (kernelType == KernelType::GaussianBlur)
		return Get5x5GaussianKernel();
	else if (kernelType == KernelType::BoxBlur)
		return Get5x5BoxKernel();
	else if (kernelType == KernelType::UnsharpMasking)
		return Get5x5UnsharpKernel();
	else if (kernelType == KernelType::Laplacian)
		return Get5x5LaplacianKernel();
	else if (kernelType == KernelType::Identity)
		return Get5x5IdentityKernel();
	else  // kernelType == KernelType::None
		return cv::Mat(); // empty matrix
}

// applies the 5 x 5 kernel on the source (original) matrix
// result is assigned to the destination matrix
void Filter(const cv::Mat3b &src, cv::Mat3b &dst, const cv::Mat &kernel)
{
	if (kernel.empty()) // if kernel is empty then kernelType == KernelType::None
		return;			// skip filtering

	cv::Mat3f dstf = cv::Mat3f(dst.size(), CV_32FC1);
	// idea: http://setosa.io/ev/image-kernels/
	for (int i = 0; i < src.rows; ++i)
	{
		for (int j = 0; j < src.cols; ++j)
		{
			float red   = 0.0;
			float green = 0.0;
			float blue  = 0.0;
			int ik = -kernel.rows / 2;
			for (int k = 0; k < kernel.rows; ++k)
			{
				int jl = -kernel.cols / 2;
				for (int l = 0; l < kernel.cols; ++l)
				{
					if (0 <= i + ik && i + ik < src.rows && 0 <= j + jl && j + jl < src.cols)
					{
						red   += (src.at<cv::Vec3b>(i + ik, j + jl).val[0] * kernel.at<float>(k, l));
						green += (src.at<cv::Vec3b>(i + ik, j + jl).val[1] * kernel.at<float>(k, l));
						blue  += (src.at<cv::Vec3b>(i + ik, j + jl).val[2] * kernel.at<float>(k, l));
					}
					++jl;
				}
				++ik;
			}
			dstf.at<cv::Vec3f>(i, j).val[0] = red;
			dstf.at<cv::Vec3f>(i, j).val[1] = green;
			dstf.at<cv::Vec3f>(i, j).val[2] = blue;
		}
	}
	dstf.convertTo(dst, src.type());
}

// displays image in a window
void ShowImageInWindow(const KernelType& kernelType, cv::Mat mat)
{
	std::string windowName;
	if (kernelType == KernelType::GaussianBlur)
		windowName = "Gaussian filtered image";
	else if (kernelType == KernelType::BoxBlur)
		windowName = "Box filtered image";
	else if (kernelType == KernelType::UnsharpMasking)
		windowName = "Unsharp masked image";
	else if (kernelType == KernelType::Laplacian)
		windowName = "Laplacian filtered image";
	else if (kernelType == KernelType::Identity)
		windowName = "Identity filtered image";
	else // kernelType == KernelType::None
		windowName = "Original image";
	cv::namedWindow(windowName, CV_WINDOW_AUTOSIZE);
	cv::imshow(windowName, mat);
}

/*************************************** main function ****************************************/

int _stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
	// input image file
	const std::string inputFile = "input_img.png";
	//const std::string inputFile = "input_img_1.png";
	//const std::string inputFile = "input_img_2.png";
	//const std::string inputFile = "grey.png";
	//const std::string inputFile = "stars.png";

	// matrix of original image
	cv::Mat3b Img = ReadImageIntoMatrix(inputFile);

	// filter type
	KernelType kernelType = KernelType::GaussianBlur;
	//KernelType kernelType = KernelType::BoxBlur;
	//KernelType kernelType = KernelType::UnsharpMasking;
	//KernelType kernelType = KernelType::Laplacian;
	//KernelType kernelType = KernelType::Identity;
	//KernelType kernelType = KernelType::None;

	// 5 x 5 kernel
	cv::Mat Kernel = Get5x5Kernel(kernelType);

	// matrix for the filtered image
	cv::Mat3b ResImg = cv::Mat3b(Img.size(), Img.type());
	
	// applies the kernel on the original matrix
	Filter(Img, ResImg, Kernel);

	// shows the original image in a window
	ShowImageInWindow(KernelType::None, Img);
	
	// shows the filtered image in another window
	// if there is a request for filtering
	if (kernelType != KernelType::None)
		ShowImageInWindow(kernelType, ResImg);
	
	// both windows can be closed by entering any key
	cv::waitKey();

	return 0;
}
