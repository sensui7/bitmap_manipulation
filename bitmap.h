// Only used STL for completing this assignment
#include <iostream>
#include <algorithm>
#include <cstring>
#include <vector>

const uint32_t FILE_HEADER_GARBAGE = 4;
const uint32_t RGB = 24;
const uint32_t RGBA = 32;
const uint32_t COMPRESSION_METHOD_0 = 0;
const uint32_t COMPRESSION_METHOD_3 = 3;

const uint32_t SHADE_ARRAY[] = { 0, 128, 255 };
const uint32_t GAUSSIAN_MATRIX[] = { 1,  4,  6,  4, 1,
                                     4, 16, 24, 16, 4,
                                     6, 24, 36, 24, 6,
                                     4, 16, 24, 16, 4,
                                     1,  4,  6,  4, 1 };

// First header of bitmap file: 14 bytes total
struct bitmapFileHeader {
    char tag1;                               // 1 byte
    char tag2;                               // 1 byte
    uint32_t sizeOfBMP;                      // 4 bytes
    char garbage[FILE_HEADER_GARBAGE];       // 4 bytes
    uint32_t offsetToPixelArray;             // 4 bytes
};

// Second header of bitmap file: 40 bytes total
struct bitmapDIBHeader {
    uint32_t sizeOfDIBHeader;                // 4 bytes
    int32_t pixelWidth;                      // 4 bytes (signed)
    int32_t pixelHeight;                     // 4 bytes (signed)
    uint16_t numColorPlanes;                 // 2 bytes
    uint16_t colorDepth;                     // 2 bytes
    uint32_t compressionMethod;              // 4 bytes
    uint32_t sizeRawBitmapData;              // 4 bytes
    uint32_t horizontalRes;                  // 4 bytes
    uint32_t verticalRes;                    // 4 bytes
    uint32_t colorsPalate;                   // 4 bytes
    uint32_t importantColors;                // 4 bytes
};

// Third header of bitmap file: 24 bytes total
// (excluding 64 bytes of color space information)
struct bitmapMaskHeader {
    uint32_t mask1;                          // 4 bytes "R"
    uint32_t mask2;                          // 4 bytes "G"
    uint32_t mask3;                          // 4 bytes "B"
    uint32_t mask4;                          // 4 bytes "A"
    uint32_t orderOfMasks;                   // 4 bytes
};

using namespace std;

class Bitmap {
private:
    friend istream& operator>>(istream& in, Bitmap& b);
    friend ostream& operator<<(ostream& out, const Bitmap& b);

    bitmapFileHeader bmpFileHeader;
    bitmapDIBHeader bmpDIBHeader;
    bitmapMaskHeader bmpMaskHeader;
    vector<uint32_t> pixelArray;

public:
    //Bitmap();
    //Bitmap(const Bitmap&);
    //Bitmap operator=(const Bitmap&);
    //Bitmap(Bitmap&&);
    //~Bitmap();

    // Functions to read in bitmap file
    void readBitmapFileHeader(istream& in, Bitmap& b);
    void readBitmapDIBHeader(istream& in, Bitmap& b);
    void readBitmapMaskHeader(istream& in, Bitmap& b);
    void readBitmapPixelArray(istream& in, Bitmap& b);
   
    // Functions to write out bitmap file 
    void writeBitmapFileHeader(ostream& out, const Bitmap& b) const; 
    void writeBitmapDIBHeader(ostream& out, const Bitmap& b) const;
    void writeBitmapMaskHeader(ostream& out, const Bitmap& b) const;
    void writeBitmapPixelArray(ostream& out, const Bitmap& b) const;

    // Helper function to write 16x16 block for pixelate
    void writeAveragedPixels(const int32_t& row, const int32_t& col, const uint32_t& newPixel);

    // Helper function to write pixel at specified cell
    void writePixel(const uint32_t& x, const uint32_t& y, const uint32_t& newPixel);

    // Helper function to get pixel at specified cell
    uint32_t getPixel(const uint32_t& x, const uint32_t& y) const;

    // Helper functions to retreive color at specified cell
    uint32_t alpha(const uint32_t& x, const uint32_t& y) const;
    uint32_t red(const uint32_t& x, const uint32_t& y) const;
    uint32_t green(const uint32_t& x, const uint32_t& y) const;
    uint32_t blue(const uint32_t& x, const uint32_t& y) const;

    // Helper functions for the image manipulations 
    uint32_t determineShift(const uint32_t& mask) const;
    uint32_t roundToShade(const uint32_t& pixelVal) const;

    // Basic image manipulation functions
    void cellShade();
    void grayscale();
    void pixelate();
    void blur();

    // Advanced image manipulation functions
    void rot90();
    void rot180();
    void rot270();
    void flipv();
    void fliph();
    void flipd1();
    void flipd2();
    void scaleUp();
    void scaleDown();

    // Functions used to debug header file and data content of bitmap
    void displayBMPFileHeader() const;
    void displayBMPDIBHeader() const;
    void displayBMPMaskHeader() const;
};
