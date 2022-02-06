#include <vector>
#include <string>

#define cimg_verbosity 3
//#define cimg_use_png
#include "CImg.h"
using namespace cimg_library;
struct Color
{
	float red;
	float green;
	float blue;
	float depth;
};

struct Position
{
	float x;
	float y;
	float z;
};


class ImageData
{
private:
	std::vector<std::vector<Color>> m_Pixels;
	unsigned int m_Width = 0;
	unsigned int m_Height = 0;

	std::string m_ColorMapLocation;
	std::string m_DepthMapLocation;

	std::vector<std::vector<Position>> m_Position3d;

	float* m_PositionPtr;
	float* m_ColorPtr;
	unsigned int* m_Indiecies;


public:

	ImageData();
	ImageData(std::string ColorMapLocation, std::string DepthMapLocation);
	~ImageData();

	void SetColorMapLocation(std::string ColorMapLocation);
	void SetDepthMapLocation(std::string DepthMapLocation);
	void LoadImageColor(); // IMPORTANT: use it before LoadImageDepth -
	void LoadImageDepth(); // It also resizes the m_Pixels vector

	void GeneratePositionsVec();
	void GeneratePositionsPtr();
	void GenerateIndiecies();

	void GenerateColor();

	float* GetPositionPtr();
	unsigned int* GetIndiecies();
	float* GetColorPtr();

	unsigned int GetPositionPtrSize();
	unsigned int GetIndieciesSize();

};