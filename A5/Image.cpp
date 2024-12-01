// Termm--Fall 2024

#include "Image.hpp"

#include <iostream>
#include <cstring>
#include <lodepng/lodepng.h>

const uint Image::m_colorComponents = 3; // Red, blue, green

//---------------------------------------------------------------------------------------
Image::Image()
  : m_width(0),
    m_height(0),
    m_data(0)
{
}

//---------------------------------------------------------------------------------------
Image::Image(
		uint width,
		uint height
)
  : m_width(width),
    m_height(height)
{
	size_t numElements = m_width * m_height * m_colorComponents;
	m_data = new double[numElements];
	memset(m_data, 0, numElements*sizeof(double));
}

//---------------------------------------------------------------------------------------
Image::Image(const Image & other)
  : m_width(other.m_width),
    m_height(other.m_height),
    m_data(other.m_data ? new double[m_width * m_height * m_colorComponents] : 0)
{
  if (m_data) {
    std::memcpy(m_data, other.m_data,
                m_width * m_height * m_colorComponents * sizeof(double));
  }
}

//---------------------------------------------------------------------------------------
Image::~Image()
{
  delete [] m_data;
}

//---------------------------------------------------------------------------------------
Image & Image::operator=(const Image& other)
{
  delete [] m_data;
  
  m_width = other.m_width;
  m_height = other.m_height;
  m_data = (other.m_data ? new double[m_width * m_height * m_colorComponents] : 0);

  if (m_data) {
    std::memcpy(m_data,
                other.m_data,
                m_width * m_height * m_colorComponents * sizeof(double)
    );
  }
  
  return *this;
}

//---------------------------------------------------------------------------------------
uint Image::width() const
{
  return m_width;
}

//---------------------------------------------------------------------------------------
uint Image::height() const
{
  return m_height;
}

//---------------------------------------------------------------------------------------
double Image::operator()(uint x, uint y, uint i) const
{
  return m_data[m_colorComponents * (m_width * y + x) + i];
}

//---------------------------------------------------------------------------------------
double & Image::operator()(uint x, uint y, uint i)
{
  return m_data[m_colorComponents * (m_width * y + x) + i];
}

//---------------------------------------------------------------------------------------
static double clamp(double x, double a, double b)
{
	return x < a ? a : (x > b ? b : x);
}

bool Image::loadPng(const std::string &filename) 
{
    std::vector<unsigned char> image;
    unsigned width, height;

    // Decode the PNG file
    unsigned error = lodepng::decode(image, width, height, filename);

    if (error) {
        std::cerr << "PNG decoder error " << error << ": " << lodepng_error_text(error)
                  << std::endl;
        return false;
    }

    // Update image dimensions
    m_width = width;
    m_height = height;

    // Free existing data if any
    delete[] m_data;
    m_data = new double[m_width * m_height * m_colorComponents];

    // Convert RGBA data from bytes [0-255] to doubles [0.0-1.0], but only store RGB
    for (unsigned y = 0; y < m_height; ++y) {
        for (unsigned x = 0; x < m_width; ++x) {
            for (unsigned c = 0; c < m_colorComponents; ++c) {
                // Calculate indices for source and destination
                size_t src_idx = 4 * (y * m_width + x) + c;  // Source is RGBA (4 components)
                size_t dst_idx = m_colorComponents * (y * m_width + x) + c;  // Dest is RGB (3 components)
                
                // Normalize from [0-255] to [0.0-1.0]
                if (src_idx < image.size()) {
                    m_data[dst_idx] = static_cast<double>(image[src_idx]) / 255.0;
                } else {
                    m_data[dst_idx] = 0.0;  // Safety fallback
                }
            }
        }
    }

    std::cout << "Loaded texture: " << filename << " (" << m_width << "x" << m_height 
              << "), components: " << m_colorComponents << std::endl;

    return true;
}

//---------------------------------------------------------------------------------------
bool Image::savePng(const std::string & filename) const
{
	std::vector<unsigned char> image;

	image.resize(m_width * m_height * m_colorComponents);

	double color;
	for (uint y(0); y < m_height; y++) {
		for (uint x(0); x < m_width; x++) {
			for (uint i(0); i < m_colorComponents; ++i) {
				color = m_data[m_colorComponents * (m_width * y + x) + i];
				color = clamp(color, 0.0, 1.0);
				image[m_colorComponents * (m_width * y + x) + i] = (unsigned char)(255 * color);
			}
		}
	}

	// Encode the image
	unsigned error = lodepng::encode(filename, image, m_width, m_height, LCT_RGB);

	if(error) {
		std::cerr << "encoder error " << error << ": " << lodepng_error_text(error)
				<< std::endl;
	}

	return true;
}

//---------------------------------------------------------------------------------------
const double * Image::data() const
{
  return m_data;
}

//---------------------------------------------------------------------------------------
double * Image::data()
{
  return m_data;
}
