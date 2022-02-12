#include "ImageData.h"
#include <iostream>

ImageData::ImageData()
{
}

ImageData::ImageData(std::string ColorMapLocation, std::string DepthMapLocation)
{
	SetColorMapLocation(ColorMapLocation);
	SetDepthMapLocation(DepthMapLocation);

	LoadImageColor();
	LoadImageDepth();
}

ImageData::~ImageData()
{
	delete[] m_PositionPtr;
	delete[] m_Indiecies;
}

void ImageData::SetColorMapLocation(std::string ColorMapLocation)
{
	m_ColorMapLocation = ColorMapLocation;
}

void ImageData::SetDepthMapLocation(std::string DepthMapLocation)
{
	m_DepthMapLocation = DepthMapLocation;
}

void ImageData::LoadImageColor()
{
	CImg<unsigned char> imageColor(m_ColorMapLocation.c_str());
	m_Width = imageColor.width();
	m_Height = imageColor.height();
	
	m_Pixels.resize(m_Height);
	for (unsigned int i = 0; i < m_Height; i++) // rows
	{
		m_Pixels[i].resize(m_Width);
		for (unsigned int j = 0; j < m_Width; j++) // columns
		{
			m_Pixels[i][j].red = (float)imageColor(i, j, 0, 0) / 255.0f;
			m_Pixels[i][j].green = (float)imageColor(i, j, 0, 1) / 255.0f;
			m_Pixels[i][j].blue = (float)imageColor(i, j, 0, 2) / 255.0f;

		}
	}

}

void ImageData::LoadImageDepth()
{
	CImg<unsigned char> imageDepth(m_DepthMapLocation.c_str());
	/* TODO: Compare size with imageColor, when different - resize */

	float tempAvgColor;

	for (unsigned int i = 0; i < m_Height; i++) // rows
	{
		m_Pixels[i].resize(m_Width);
		for (unsigned int j = 0; j < m_Width; j++) // columns
		{ 
			/*
			tempAvgColor = (((float)imageDepth(i, j, 0, 0) / 255.0f) +
				((float)imageDepth(i, j, 0, 1) / 255.0f) +
				((float)imageDepth(i, j, 0, 2) / 255.0f)) / 3.0f;
			*/
			tempAvgColor = (float)imageDepth(i, j, 0, 0) / 255.0f;
			m_Pixels[i][j].depth = (tempAvgColor * 2) - 1; // range -1 to 1
			

		}
	}

}

void ImageData::GeneratePositionsVec()
{
	float WidthSpacing = (2.0f / ((float)m_Width - 1.0f));
	float HeightSpacing = (2.0f / ((float)m_Height - 1.0f));
	
	// size+1 because there are n/2 numbers in <-1,0) + n/2 in (0,1) 
	// and there is zero in the middle, so n/2 + n/2 + 1
	m_Position3d.resize(m_Height);
	for (unsigned int i = 0; i < m_Height; i++) // rows
	{
		m_Position3d[i].resize(m_Width);
		for (unsigned int j = 0; j < m_Width; j++) // columns
		{
			
			// Matrix already rotated \/
			m_Position3d[i][j].x = -1.0f + WidthSpacing * j;
			m_Position3d[i][j].y = m_Pixels[i][j].depth*0.5;
			m_Position3d[i][j].z = -1.0f + HeightSpacing * i;
			if (j == 0 || j == m_Width-1)
			{
				m_Position3d[i][j].y = -1*0.5-0.0001;
			}


		}
	}
}




void ImageData::GeneratePositionsPtr()
{
	m_PositionPtr = new float[(m_Width * m_Height * 3)];
	unsigned int x_pos = 0;
	unsigned int y_pos = 0;
	/*
	for (unsigned int i = 0; i < m_Height; i++) // rows
	{
		for (unsigned int j = 0; j < m_Width*3; j+=3) // columns
		{
			m_PositionPtr[i * m_Height + j + 0] = m_Position3d[i][x_pos].x;
			m_PositionPtr[i * m_Height + j + 1] = m_Position3d[i][x_pos].y;
			m_PositionPtr[i * m_Height + j + 2] = m_Position3d[i][x_pos].z;
			x_pos++;
		}
		x_pos = 0;
	}
	*/
	
	for (unsigned int i = 0; i < m_Height * m_Width * 3; i+=3)
	{
		m_PositionPtr[i + 0] = m_Position3d[y_pos][x_pos].x;
		m_PositionPtr[i + 1] = m_Position3d[y_pos][x_pos].y;
		m_PositionPtr[i + 2] = m_Position3d[y_pos][x_pos].z;
		
		if (x_pos < m_Width-1) // x_pos < 1023
			x_pos++;
		else
		{
			x_pos = 0;
			y_pos++;
		}
	}
	
}

void ImageData::GenerateColor()
{
	m_ColorPtr = new float[(m_Width * m_Height * 3)];
	unsigned int x_pos = 0;
	unsigned int y_pos = 0;
	/*
	for (unsigned int i = 0; i < m_Height; i++) // rows
	{
		for (unsigned int j = 0; j < m_Width * 3; j += 3) // columns
		{
			m_ColorPtr[i * m_Height + j + 0] = m_Pixels[i][x_pos].red;
			m_ColorPtr[i * m_Height + j + 1] = m_Pixels[i][x_pos].green;
			m_ColorPtr[i * m_Height + j + 2] = m_Pixels[i][x_pos].blue;
			x_pos++;
		}
		x_pos = 0;
	}
	*/
	
	for (unsigned int i = 0; i < (m_Height) * (m_Width) * 3; i += 3)
	{
		


		m_ColorPtr[i + 0] = m_Pixels[y_pos][x_pos].red;
		m_ColorPtr[i + 1] = m_Pixels[y_pos][x_pos].green;
		m_ColorPtr[i + 2] = m_Pixels[y_pos][x_pos].blue;

		//if (x_pos == 1000)
		//	std::cout << ".\n";

		if (x_pos < m_Width-1)
			x_pos++;
		else
		{
			x_pos = 0;
			y_pos++;
		}
	}
	
}




void ImageData::GenerateIndiecies()
{
	m_Indiecies = new unsigned int[((m_Width) * (m_Height)) * 6];

	unsigned int curr_row = 0;
	unsigned int x_pos = 0;

	/*
	for (unsigned int i = 0; i < m_Height; i++) // rows
	{
		for (unsigned int j = 0; j < (m_Width)*6; j+=6) // columns
		{
			m_Indiecies[i * m_Height + j + 0] = i * m_Height + x_pos;
			m_Indiecies[i * m_Height + j + 1] = i * m_Height + x_pos + 1;
			m_Indiecies[i * m_Height + j + 2] = (i + 1) * m_Height + x_pos + 1;

			m_Indiecies[i * m_Height + j + 3] = (i + 1) * m_Height + x_pos + 1;
			m_Indiecies[i * m_Height + j + 4] = (i + 1) * m_Height + x_pos;
			m_Indiecies[i * m_Height + j + 5] = i * m_Height + x_pos;
			x_pos++;
		}
		//x_pos = 0;
	}
	*/

	
	for (unsigned int i = 0; i < GetIndieciesSize(); i += 6)
	{

		m_Indiecies[i] = x_pos + m_Height * curr_row;
		m_Indiecies[i + 1] = x_pos + 1 + m_Height * curr_row;
		m_Indiecies[i + 2] = x_pos + 1 + m_Height * (curr_row + 1);
		m_Indiecies[i + 3] = x_pos + 1 + m_Height * (curr_row + 1);
		m_Indiecies[i + 4] = x_pos + m_Height * (curr_row + 1);
		m_Indiecies[i + 5] = x_pos + m_Height * curr_row;

		//if (x_pos != (curr_row + 1) * (m_Height - 1))
			x_pos++;
		//else
		//{
		//	curr_row = (x_pos / m_Height) + 1;
		//	x_pos++;
		//}
	}
	
}



float* ImageData::GetPositionPtr()
{
	return m_PositionPtr;
}

float* ImageData::GetColorPtr()
{
	return m_ColorPtr;
}


unsigned int* ImageData::GetIndiecies()
{
	return m_Indiecies;
}


unsigned int ImageData::GetPositionPtrSize()
{
	return ((m_Width) * (m_Height)) * 3;
}

unsigned int ImageData::GetIndieciesSize()
{
	return ((m_Width) * (m_Height)) * 6;
}
