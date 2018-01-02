#include <iostream>
#include <vector>
#include "RGB.h"

/***************************************** types ****************************************/

/*
 * KernelType::GaussianBlur		blurs the original image
 * KernelType::BoxBlur			blurs the original image
 * KernelType::UnsharpMasking	sharpens the original image
 * KernelType::Laplacian		detects edge of the original image
 * KernelType::Identity			applies a 5x5 Identity kernel on the original image
 *								the result is actually the original image
 * KernelType::None				means filtering is not requested
 */

enum class KernelType { GaussianBlur, BoxBlur, UnsharpMasking, Laplacian, Identity, None };

typedef std::vector<std::vector<RGB> > Matrix;
typedef std::vector<RGB> Row;

/**************************************** kernels ***************************************/

// 5 x 5 Gaussian blur
//                  1   4   6   4   1
//                  4  16  24  16   4
//    1 / 256   *   6  24  36  24   6
//                  4  16  24  16   4
//                  1   4   6   4   1
const float GaussianKernel[5][5] = {
	{ 1.f / 256.f,   4.f / 256.f,   6.f / 256.f,   4.f / 256.f,  1.f / 256.f },
	{ 4.f / 256.f,  16.f / 256.f,  24.f / 256.f,  16.f / 256.f,  4.f / 256.f },
	{ 6.f / 256.f,  24.f / 256.f,  36.f / 256.f,  24.f / 256.f,  6.f / 256.f },
	{ 4.f / 256.f,  16.f / 256.f,  24.f / 256.f,  16.f / 256.f,  4.f / 256.f },
	{ 1.f / 256.f,   4.f / 256.f,   6.f / 256.f,   4.f / 256.f,  1.f / 256.f }
};

// 5 x 5 Box blur
//                 1   1   1   1   1
//                 1   1   1   1   1
//    1 / 25   *   1   1   1   1   1
//                 1   1   1   1   1
//                 1   1   1   1   1
const float BoxKernel[5][5] = {
	{ 1.f / 25.f,  1.f / 25.f,  1.f / 25.f,  1.f / 25.f,  1.f / 25.f },
	{ 1.f / 25.f,  1.f / 25.f,  1.f / 25.f,  1.f / 25.f,  1.f / 25.f },
	{ 1.f / 25.f,  1.f / 25.f,  1.f / 25.f,  1.f / 25.f,  1.f / 25.f },
	{ 1.f / 25.f,  1.f / 25.f,  1.f / 25.f,  1.f / 25.f,  1.f / 25.f },
	{ 1.f / 25.f,  1.f / 25.f,  1.f / 25.f,  1.f / 25.f,  1.f / 25.f }
};

// 5 x 5 Unsharp masking
//                   1     4     6     4    1
//                   4    16    24    16    4
//    -1 / 256   *   6    24  -476    24    6
//                   4    16    24    16    4
//                   1     4     6     4    1
const float UnsharpKernel[5][5] = {
	{ -1.f / 256.f,   -4.f / 256.f,   -6.f / 256.f,   -4.f / 256.f,  -1.f / 256.f },
	{ -4.f / 256.f,  -16.f / 256.f,  -24.f / 256.f,  -16.f / 256.f,  -4.f / 256.f },
	{ -6.f / 256.f,  -24.f / 256.f,  476.f / 256.f,  -24.f / 256.f,  -6.f / 256.f },
	{ -4.f / 256.f,  -16.f / 256.f,  -24.f / 256.f,  -16.f / 256.f,  -4.f / 256.f },
	{ -1.f / 256.f,   -4.f / 256.f,   -6.f / 256.f,   -4.f / 256.f,  -1.f / 256.f }
};

// 5 x 5 Laplacian kernel
//    -1   -1   -1   -1   -1
//    -1   -1   -1   -1   -1
//    -1   -1   24   -1   -1
//    -1   -1   -1   -1   -1
//    -1   -1   -1   -1   -1
const float LaplacianKernel[5][5] = {
	{ -1.f,  -1.f,  -1.f,  -1.f,  -1.f },
	{ -1.f,  -1.f,  -1.f,  -1.f,  -1.f },
	{ -1.f,  -1.f,  24.f,  -1.f,  -1.f },
	{ -1.f,  -1.f,  -1.f,  -1.f,  -1.f },
	{ -1.f,  -1.f,  -1.f,  -1.f,  -1.f }
};

// 5 x 5 Identity kernel
//    0  0  0  0  0
//    0  0  0  0  0
//    0  0  1  0  0
//    0  0  0  0  0
//    0  0  0  0  0
const float IdentityKernel[5][5] = {
	{ 0.f,  0.f,  0.f,  0.f,  0.f },
	{ 0.f,  0.f,  0.f,  0.f,  0.f },
	{ 0.f,  0.f,  1.f,  0.f,  0.f },
	{ 0.f,  0.f,  0.f,  0.f,  0.f },
	{ 0.f,  0.f,  0.f,  0.f,  0.f }
};

/*************************************** functions **************************************/

Matrix ReadImage(const std::string fileName)
{
	// ... TODO

	return Matrix(); // TODO: return the actual matrix
}

void SaveImage(Matrix mat)
{
	// ... TODO
}

void GetKernel(const KernelType& kernelType, float kernel[5][5])
{
	switch ((int)kernelType)
	{
	case (int)KernelType::GaussianBlur:
		memcpy(kernel, GaussianKernel, sizeof(GaussianKernel));
		break;
	case (int)KernelType::BoxBlur:
		memcpy(kernel, BoxKernel, sizeof(BoxKernel));
		break;
	case (int)KernelType::UnsharpMasking:
		memcpy(kernel, UnsharpKernel, sizeof(UnsharpKernel));
		break;
	case (int)KernelType::Laplacian:
		memcpy(kernel, LaplacianKernel, sizeof(LaplacianKernel));
		break;
	case (int)KernelType::Identity:
		memcpy(kernel, IdentityKernel, sizeof(IdentityKernel));
		break;
	default:
		memset(kernel, 0, sizeof(kernel[0][0]) * 5 * 5); // 5 x 5 kernel - full of zeros
		break;
	}
}

// applies the 5 x 5 kernel on the source (original) matrix
// result is assigned to the destination matrix
void Filter(const Matrix &src, Matrix &dst, const float kernel[5][5], const KernelType &kernelType)
{
	if (kernelType == KernelType::None)
		return;	// skip filtering

	int srcRows = src.size();
	int srcCols = src.at(0).size();
	dst.resize(srcRows);

	// idea: http://setosa.io/ev/image-kernels/
	for (int i = 0; i < srcRows; ++i)
	{
		dst.at(i).resize(srcCols);
		for (int j = 0; j < srcCols; ++j)
		{
			float red = 0.0;
			float green = 0.0;
			float blue = 0.0;
			int ik = 5 / 2;
			for (int k = 0; k < 5; ++k)
			{
				int jl = -5 / 2;
				for (int l = 0; l < 5; ++l)
				{
					if (0 <= i + ik && i + ik < srcRows && 0 <= j + jl && j + jl < srcCols)
					{
						red   += (src.at(i + ik).at(j + jl).R * kernel[k][l]);
						green += (src.at(i + ik).at(j + jl).G * kernel[k][l]);
						blue  += (src.at(i + ik).at(j + jl).B * kernel[k][l]);
					}
					++jl;
				}
				++ik;
			}
			//dst.at(i).push_back(RGB(red, green, blue));
			dst.at(i).at(j) = RGB(red, green, blue);
		}
	}
}

/************************************* main function ************************************/

int main()
{
	std::cout << "Image Processing" << std::endl;

	// input image file
	const std::string inputFile = "input_img.bmp";

	// filter type
	//KernelType kernelType = KernelType::GaussianBlur;
	//KernelType kernelType = KernelType::BoxBlur;
	//KernelType kernelType = KernelType::UnsharpMasking;
	//KernelType kernelType = KernelType::Laplacian;
	//KernelType kernelType = KernelType::Identity;
	KernelType kernelType = KernelType::None;

	// matrix for the original image
	Matrix Img = ReadImage(inputFile);

	// 5 x 5 kernel
	float Kernel[5][5];
	GetKernel(kernelType, Kernel);

	// matrix for the filtered image
	Matrix ResImg;

	// applies the kernel on the original matrix
	Filter(Img, ResImg, Kernel, kernelType);

	/*
	for (int i = 0; i < Img.size(); ++i)
	{
		for (int j = 0; j < Img.at(0).size(); ++j)
		{
			std::cout << Img[i][j].R << std::endl;
			std::cout << Img[i][j].G << std::endl;
			std::cout << Img[i][j].B << std::endl;
		}
		std::cout << std::endl;
	}
	/*/

	/*
	for (int i = 0; i < 5; ++i)
	{
		for (int j = 0; j < 5; ++j)
		{
			std::cout << Kernel[i][j] << std::endl;
		}
		std::cout << std::endl;
	}
	/*/

	/*
	for (int i = 0; i < ResImg.size(); ++i)
	{
		for (int j = 0; j < ResImg.at(0).size(); ++j)
		{
			std::cout << ResImg[i][j].R << std::endl;
			std::cout << ResImg[i][j].G << std::endl;
			std::cout << ResImg[i][j].B << std::endl;
		}
		std::cout << std::endl;
	}
	/*/

	SaveImage(ResImg);

	// close console window after obtaining any key
	getchar();

	return 0;
}
