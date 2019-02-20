#include "bitmap.h"
#include "bitmapException.h"

// Helper function for determining how many bits to shift
// based upon the order of masks (bits) given, which may be different
// across some bitmap files
uint32_t Bitmap::determineShift(const uint32_t& mask) const {
    if (mask == 0xff000000) return 24;
    if (mask == 0xff0000) return 16;
    if (mask == 0xff00) return 8;
    if (mask == 0xff) return 0;
}

// Cell shading, which renders the graphic non-photorealistic
void Bitmap::cellShade() {
    uint32_t red = 0, green = 0, blue = 0, alpha = 0;
    uint32_t colorDepth = bmpDIBHeader.colorDepth;
    vector<uint32_t> newPixelArray;

    // Discover the mask shifts for the pixel colors
    uint32_t mask1Shift = determineShift(bmpMaskHeader.mask1);
    uint32_t mask2Shift = determineShift(bmpMaskHeader.mask2);
    uint32_t mask3Shift = determineShift(bmpMaskHeader.mask3);
    uint32_t mask4Shift = determineShift(bmpMaskHeader.mask4);

    // (32 BIT) Round the pixel color values up, append the alpha byte,
    // and then store the result into the new pixel array
    if (colorDepth == RGBA) {
        for (uint32_t pixel : pixelArray) {
            red = (pixel & bmpMaskHeader.mask1) >> mask1Shift;
            green = (pixel & bmpMaskHeader.mask2) >> mask2Shift;
            blue = (pixel & bmpMaskHeader.mask3) >> mask3Shift;
            alpha = (pixel & bmpMaskHeader.mask4) >> mask4Shift;
            
            blue = roundToShade(blue);
            green = roundToShade(green);
            red = roundToShade(red);
            
            pixel = (red << mask1Shift) + (green << mask2Shift) + 
                    (blue << mask3Shift) + (alpha << mask4Shift);

            newPixelArray.push_back(pixel);
        } 
    }

    // (24 BIT) Round the pixel color values up, and then store the result
    // into the new pixel array
    if (colorDepth == RGB) {
        for (uint32_t pixel : pixelArray) {
            red = (pixel) & 0xFF;
            green = (pixel >> 8) & 0xFF;
            blue = (pixel >> 16) & 0xFF;

            red = roundToShade(red);
            green = roundToShade(green);
            blue = roundToShade(blue);

            pixel = (red) + (green << 8) + (blue << 16);

            newPixelArray.push_back(pixel);
        } 
    }

    pixelArray = newPixelArray;
}

// Helper function for cell shading
uint32_t Bitmap::roundToShade(const uint32_t& pixelVal) const {
    uint32_t range_0 = SHADE_ARRAY[0]; // Pixel color value of 0
    uint32_t range_128 = SHADE_ARRAY[1]; // Pixel color value of 128
    uint32_t range_255 = SHADE_ARRAY[2]; // Pixel color value of 255

    // Distribute color value into the three values using 4 divided ranges
    if (pixelVal < 64) return range_0;  
    if (pixelVal >= 64 && pixelVal < 128) return range_128;
    if (pixelVal >= 128 && pixelVal < 192) return range_128;
    if (pixelVal >= 192 && pixelVal < 256) return range_255;

    return pixelVal;
}

// Grayscale, where the image's color information from RGB gets removed
void Bitmap::grayscale() { 
    uint32_t red = 0, green = 0, blue = 0, alpha = 0, gray = 0;
    uint32_t colorDepth = bmpDIBHeader.colorDepth;
    vector<uint32_t> newPixelArray;

    // Discover the mask shifts for the pixel colors
    uint32_t mask1Shift = determineShift(bmpMaskHeader.mask1);
    uint32_t mask2Shift = determineShift(bmpMaskHeader.mask2);
    uint32_t mask3Shift = determineShift(bmpMaskHeader.mask3);
    uint32_t mask4Shift = determineShift(bmpMaskHeader.mask4);

    // (32 BIT) Calculate the grayscale for each pixel, append the alpha byte, and
    // then store the pixel into the new pixel array
    if (colorDepth == RGBA) {
        for (uint32_t pixel : pixelArray) { 
            red = (pixel >> mask1Shift) & (bmpMaskHeader.mask1 >> mask1Shift);
            green = (pixel >> mask2Shift) & (bmpMaskHeader.mask2 >> mask2Shift);
            blue = (pixel >> mask3Shift) & (bmpMaskHeader.mask3 >> mask3Shift);
            alpha = (pixel >> mask4Shift) & (bmpMaskHeader.mask4 >> mask4Shift);

            gray = (red + green + blue) / 3;
            pixel = (gray << mask1Shift) + (gray << mask2Shift) + (gray << mask3Shift) + (alpha << mask4Shift);
            newPixelArray.push_back(pixel);
        } 
    }

    // (24 BIT) Calculate the grayscale for each pixel, and
    // then store the pixel into the new pixel array
    if (colorDepth == RGB) {
        for (uint32_t pixel : pixelArray) { 
            red = (pixel) & 0xFF;
            green = (pixel >> 8) & 0xFF;
            blue = (pixel >> 16) & 0xFF;

            gray = (red + green + blue) / 3;
            pixel = (gray) + (gray << 8) + (gray << 16);
            newPixelArray.push_back(pixel);
        } 
    }

    pixelArray = newPixelArray;
}

// Pixelate, which displays the bitmap such that the
// individual pixels that make up the bitmap are visible
void Bitmap::pixelate() {
    int32_t pixelWidth = bmpDIBHeader.pixelWidth, pixelHeight = bmpDIBHeader.pixelHeight;

    // Discover the mask shifts for the pixel colors
    uint32_t mask1Shift = determineShift(bmpMaskHeader.mask1);
    uint32_t mask2Shift = determineShift(bmpMaskHeader.mask2);
    uint32_t mask3Shift = determineShift(bmpMaskHeader.mask3);
    uint32_t mask4Shift = determineShift(bmpMaskHeader.mask4);

    // Iterate through the entire bitmap in sizes of 16x16 blocks
    for (int32_t row = 0; row < pixelHeight && row < pixelHeight; row += 16) {
        for (int32_t col = 0; col < pixelWidth && col < pixelWidth; col += 16) {
            uint32_t cellCountForBlock = 0;
            uint32_t redTotal = 0, greenTotal = 0, blueTotal = 0;

            // For each 16x16 block, calculate the total cells, red sum, green sum, and blue sum
            for (int32_t rowBlock = row; rowBlock < (row + 16) && rowBlock < pixelHeight; ++rowBlock) {
                for (int32_t colBlock = col; colBlock < (col + 16) && colBlock < pixelWidth; ++colBlock) {
                    ++cellCountForBlock; 

                    redTotal += red(colBlock, rowBlock);
                    greenTotal += green(colBlock, rowBlock);
                    blueTotal += blue(colBlock, rowBlock);
                } 
            } 
             
            // Calculate the average colors
            uint32_t avgRed = redTotal / cellCountForBlock;
            uint32_t avgGreen = greenTotal / cellCountForBlock;
            uint32_t avgBlue = blueTotal / cellCountForBlock;

            // Build the new pixel out of the average colors
            uint32_t newAverageColor = (bmpDIBHeader.compressionMethod == COMPRESSION_METHOD_3) 
                                       ? (avgRed << mask1Shift) + (avgGreen << mask2Shift) + (avgBlue << mask3Shift)
                                       : (avgRed) + (avgGreen << 8) + (avgBlue << 16);
            
            // Write the new pixel value into the 16x16 block just worked with 
            writeAveragedPixels(row, col, newAverageColor);
        }
    }
}

// Helper function for pixelate, which iterates through the same 16x16 block
// in pixelate(Bitmap& b), and writes the new pixel color into each pixel
void Bitmap::writeAveragedPixels(const int32_t& row, const int32_t& col, const uint32_t& newPixel) {
    int32_t pixelWidth = bmpDIBHeader.pixelWidth, pixelHeight = bmpDIBHeader.pixelHeight;
    uint32_t mask4Shift = determineShift(bmpMaskHeader.mask4);

    // Go through the 16x16 block passed in via row and col
    for (int32_t rowBlock = row; rowBlock < (row + 16) && rowBlock < pixelHeight; ++rowBlock) {
        for (int32_t colBlock = col; colBlock < (col + 16) && colBlock < pixelWidth; ++colBlock) {
            uint32_t alphaVal = alpha(colBlock, rowBlock);
            uint32_t updatedPixel = newPixel;

            // Append the alpha byte, and write the new pixel value to the entire block
            updatedPixel += (bmpDIBHeader.compressionMethod == COMPRESSION_METHOD_3) ? (alphaVal << mask4Shift) : 0;
            writePixel(colBlock, rowBlock, updatedPixel);
        }
    }
}

// Gaussian blur, which blurs an image using the Gaussian function to
// reduce noise and detail
void Bitmap::blur() { 
    int pixelWidth = bmpDIBHeader.pixelWidth, pixelHeight = bmpDIBHeader.pixelHeight;

    // Discover the mask shifts for the pixel colors
    uint32_t mask1Shift = determineShift(bmpMaskHeader.mask1);
    uint32_t mask2Shift = determineShift(bmpMaskHeader.mask2);
    uint32_t mask3Shift = determineShift(bmpMaskHeader.mask3);
    uint32_t mask4Shift = determineShift(bmpMaskHeader.mask4);

    // Read every single pixel in the image
    for (int32_t row = 0; row < pixelHeight && row < pixelHeight; row += 1) {
        for (int32_t col = 0; col < pixelWidth && col < pixelWidth; col += 1) {
            uint32_t rsum = 0, gsum = 0, bsum = 0;
            uint32_t gaussIndex = 0;

            // For every 5x5 group surround the pixel chosen as the center,
            // calculate and add to the sum of RGB
            for (int32_t rowBlock = row - 2; rowBlock < (row - 2 + 5); ++rowBlock) {
                for (int32_t colBlock = col - 2; colBlock < (col - 2 + 5); ++colBlock) {  
                    // Treat every nonexistent pixel value as 0
                    // Multiply respective values from the gaussian matrix and divide by 256
                    if (colBlock >= 0 && rowBlock >= 0 && (colBlock < pixelWidth && rowBlock < pixelHeight)) {
                        rsum += (GAUSSIAN_MATRIX[gaussIndex] * red(colBlock, rowBlock)) / (256);
                        gsum += (GAUSSIAN_MATRIX[gaussIndex] * green(colBlock, rowBlock)) / (256);
                        bsum += (GAUSSIAN_MATRIX[gaussIndex] * blue(colBlock, rowBlock)) / (256);
                        ++gaussIndex; 
                    }
                } 
            } 

            // Get the current pixel
            int32_t colBlock = col - 2, rowBlock = row - 2;

            // If the pixel chosen was within height and width bounds of the image,
            // write the new pixel into the current cell
            if (colBlock >= 0 && rowBlock >= 0 && (colBlock < pixelWidth && rowBlock < pixelHeight)) { 
                uint32_t newPixel = 0;

                // If RGBA, build new pixel, and append alpha byte
                if (bmpDIBHeader.compressionMethod == COMPRESSION_METHOD_3) {
                    newPixel = (rsum << mask1Shift) + (gsum << mask2Shift) + 
                               (bsum << mask3Shift) + (alpha(colBlock,rowBlock) << mask4Shift);
                }
               
                // If RGB, build new pixel 
                if (bmpDIBHeader.compressionMethod == COMPRESSION_METHOD_0) {
                    newPixel = (rsum) + (gsum << 8) + (bsum << 16);
                }

                writePixel(colBlock, rowBlock, newPixel);
            }
        }
    }
}

// This rotates an image by 90-degrees clockwise
void Bitmap::rot90() {
    // First flip horizontally
    fliph();

    int32_t height = bmpDIBHeader.pixelHeight, width = bmpDIBHeader.pixelWidth;
    vector<uint32_t> newPixelArray;

    // Transpose the array "matrix"
    for (int32_t col = 0; col < width; ++col) {
        for (int32_t row = 0; row < height; ++row) {
            newPixelArray.push_back(getPixel(col, row));
        }
    }

    pixelArray = newPixelArray; 
   
    // Adjust the height and width of the image,
    // since the dimensions have changed 
    bmpDIBHeader.pixelWidth = height;
    bmpDIBHeader.pixelHeight = width;
}

// Rotate the image 180-degrees clockwise
void Bitmap::rot180() {
    // Flip the image vertically
    flipv();

    // Flip the image horizontally
    fliph();
}

// Rotate the image 270 degrees
void Bitmap::rot270() {  
    // Flip the image vertically
    flipv();
    
    int32_t height = bmpDIBHeader.pixelHeight, width = bmpDIBHeader.pixelWidth;
    vector<uint32_t> newPixelArray;

    // Transpose the array "matrix"
    for (int32_t col = 0; col < width; ++col) {
        for (int32_t row = 0; row < height; ++row) {
            newPixelArray.push_back(getPixel(col, row));
        }
    }

    pixelArray = newPixelArray;  

    // Update the image width and height because
    // the dimensions have changed
    bmpDIBHeader.pixelWidth = height;
    bmpDIBHeader.pixelHeight = width;
}

// Flip the image horizontally
void Bitmap::fliph() {
    int32_t height = bmpDIBHeader.pixelHeight, width = bmpDIBHeader.pixelWidth;

    // Reverse the array row by row on the image
    for (int32_t i = 0, currWidth = 0; i < height; ++i, currWidth += width) {
        reverse(pixelArray.begin() + currWidth, pixelArray.begin() + currWidth + width);
    }
}

// Flip the image vertically
void Bitmap::flipv() {
    // Reverse the entire array 
    reverse(pixelArray.begin(), pixelArray.end());

    // Flip horizontally
    fliph();
}

// Flip the image across the diagonal
// from top left corner to bottom right corner
void Bitmap::flipd1() {
    // Rotate 90-degrees clockwise
    rot90();

    // Flip the image vertically
    flipv();
}

// Flip the image across the diagonal
// from the top right corner to bottom left corner
void Bitmap::flipd2() {
    // Rotate 90-degrees clockwise
    rot90();

    // Flip the image horizontally
    fliph();
}

// Scale up the image by duplicating every pixel row and column-wise (2x2)
void Bitmap::scaleUp() { 
    int32_t pixelHeight = bmpDIBHeader.pixelHeight, pixelWidth = bmpDIBHeader.pixelWidth;
    vector<uint32_t> newPixelArray;

    // For every pixel in each row, iterate through the columns twice
    for (int32_t row = 0; row < pixelHeight; ++row) {
        // Duplicate the pixel twice for the first iteration
        for (int col = 0; col < pixelWidth; ++col) {
            newPixelArray.push_back(getPixel(col, row));
            newPixelArray.push_back(getPixel(col, row));
        }

        // Duplicate the pixel twice for the second iteration
        for (int32_t col = 0; col < pixelWidth; ++col) {
            newPixelArray.push_back(getPixel(col, row));
            newPixelArray.push_back(getPixel(col, row));
        }
    }

    pixelArray = newPixelArray;

    int32_t newWidth = 0, newHeight = 0;

    // Adjust image width and height, since dimensions have changed
    newWidth = bmpDIBHeader.pixelWidth *= 2;
    newHeight = bmpDIBHeader.pixelHeight *= 2;

    // Adjust the raw bitmap size regardless of 24 BIT or 32 BIT
    bmpDIBHeader.sizeRawBitmapData = newWidth * newHeight * (RGBA / 8); 
}

// For scaling down, remove every other row and column
void Bitmap::scaleDown() {
    int32_t pixelHeight = bmpDIBHeader.pixelHeight, pixelWidth = bmpDIBHeader.pixelWidth;
    vector<uint32_t> newPixelArray; 

    // Do not shrink past 1x1 pixel, otherwise error occurs
    if (pixelHeight == 1 || pixelWidth == 1) {
        return;
    }

    // Iterate through a reduced version of the image
    for (int32_t row = 0; row < pixelHeight; row += 2) {
        for (int32_t col = 0; col < pixelWidth; col += 2) {
            newPixelArray.push_back(getPixel(col, row));
        }
    }

    pixelArray = newPixelArray;

    int32_t newWidth = 0, newHeight = 0;
    // Adjust image width and height, since dimensions have changed
    newWidth = bmpDIBHeader.pixelWidth /= 2;
    newHeight = bmpDIBHeader.pixelHeight /= 2;

    // Adjust the raw bitmap size regardless of 32 BIT or 24 BIT bitmap file
    bmpDIBHeader.sizeRawBitmapData = newHeight * newWidth * (RGBA / 8); 
}

// Read the first bitmap file header (14 bytes total)
void Bitmap::readBitmapFileHeader(istream& in, Bitmap& b) {
    // Read in the identifier for the type of bitmap (always "BM")
    char tag[2];
    in >> tag[0] >> tag[1];

    // Check if bitmap type is valid
    if (strncmp(tag, "BM", 2) != 0) {
        throw BitmapException("Error: bitmap tag type isn't BM");
    } else {
        // Store values into header
        b.bmpFileHeader.tag1 = tag[0];
        b.bmpFileHeader.tag2 = tag[1];
    }

    // Read in the size
    uint32_t size = 0;
    in.read((char*) &size, 4);
    b.bmpFileHeader.sizeOfBMP = size;

    // Store garbage data to be written later in ostream
    uint32_t index = 0, garbageVal = 0;
    for (; index < FILE_HEADER_GARBAGE; ++index) {
        in.read((char*) &garbageVal, 1);
        b.bmpFileHeader.garbage[index] = garbageVal;
    }

    // Readin the offset to the start of the pixel array data
    uint32_t offset = 0;
    in.read((char*) &offset, 4); 
    b.bmpFileHeader.offsetToPixelArray = offset;
}

// Read the DIB (Device-Independent Bitmap) header
void Bitmap::readBitmapDIBHeader(istream& in, Bitmap& b) {
    // Read in the size of the second header
    uint32_t size = 0;  
    in.read((char*) &size, 4);
    b.bmpDIBHeader.sizeOfDIBHeader = size;
 
    // Read in the width and height in pixels (signed) 
    int32_t width = 0, height = 0;
    in.read((char*) &width, 4);
    in.read((char*) &height, 4);
    b.bmpDIBHeader.pixelWidth = width;
    b.bmpDIBHeader.pixelHeight = height;
 
    // Read in the color plane and color depth of the image 
    uint16_t numColorPlanes = 0, colorDepth = 0;
    in.read((char*) &numColorPlanes, 2);
    in.read((char*) &colorDepth, 2);
    if (numColorPlanes != 1) {
        throw BitmapException("Error: number of color planes in bitmap isn't 1");
    }
    if (colorDepth != RGB && colorDepth != RGBA) {
        throw BitmapException("Error: bitmap color depth isn't 24 or 32");
    }
    b.bmpDIBHeader.numColorPlanes = numColorPlanes;
    b.bmpDIBHeader.colorDepth = colorDepth;

    // Read in the compression method and size of the raw bitmap data
    uint32_t compressionMethod = 0, sizeRawBitmapData = 0;
    in.read((char*) &compressionMethod, 4);
    in.read((char*) &sizeRawBitmapData, 4);
    if (compressionMethod != COMPRESSION_METHOD_0 && compressionMethod != COMPRESSION_METHOD_3) {
        throw BitmapException("Error: Bitmap file compression is not 0 or 3");
    }
    b.bmpDIBHeader.compressionMethod = compressionMethod;
    b.bmpDIBHeader.sizeRawBitmapData = sizeRawBitmapData;
  
    // Read in the horizontal and vertical resolutions (2835x2835) 
    uint32_t horizontalRes = 0, verticalRes = 0; 
    in.read((char*) &horizontalRes, 4);
    in.read((char*) &verticalRes, 4);
    b.bmpDIBHeader.horizontalRes = horizontalRes;
    b.bmpDIBHeader.verticalRes = verticalRes;

    // Read in the colors in the color palate and the important colors
    uint32_t colorsPalate = 0, importantColors = 0;
    in.read((char*) &colorsPalate, 4);
    in.read((char*) &importantColors, 4);
    b.bmpDIBHeader.colorsPalate = colorsPalate;
    b.bmpDIBHeader.importantColors = importantColors;
}

// Read in the bitmap mask header (if compression method is 3)
void Bitmap::readBitmapMaskHeader(istream& in, Bitmap& b) {
    uint32_t mask1 = 0, mask2 = 0, mask3 = 0, mask4 = 0;
    uint32_t orderOfMasks = 0;

    // Order of masks is always RGBA, however,
    // the shifts may be different for them across
    // different bitmap files
    in.read((char*) &mask1, 4);
    in.read((char*) &mask2, 4);
    in.read((char*) &mask3, 4);
    in.read((char*) &mask4, 4);
    in.read((char*) &orderOfMasks, 4);

    b.bmpMaskHeader.mask1 = mask1;
    b.bmpMaskHeader.mask2 = mask2;
    b.bmpMaskHeader.mask3 = mask3;
    b.bmpMaskHeader.mask4 = mask4;
    b.bmpMaskHeader.orderOfMasks = orderOfMasks;

    // Skip 64 bytes of color space information, which can be ignored
    // Will be using this to pad anyways to keep consistency between files
    uint32_t ignoredColorSpaceInfo = 0, index = 0;
    for (; index < 64; index += 4) {
        in.read((char*) &ignoredColorSpaceInfo, 4);
    }
}


// Read in the bitmap pixel array data
void Bitmap::readBitmapPixelArray(istream& in, Bitmap& b) {  
    uint32_t colorDepth = bmpDIBHeader.colorDepth, pixelWidth = bmpDIBHeader.pixelWidth;

    // Read strategy for RGBA bitmaps (32 BIT)
    if (colorDepth == RGBA) {
        uint32_t temp = 0;

        // Read data in groups of 4
        for (uint32_t index = 0; index < bmpDIBHeader.sizeRawBitmapData; index += 4) {
            in.read((char*) &temp, 4);
            pixelArray.push_back(temp);
        }
    }

    // Read strategy for RGB bitmaps (24 BIT)
    if (colorDepth == RGB) {
        uint32_t temp = 0, pixelWidth = bmpDIBHeader.pixelWidth;
        uint8_t byte1, byte2, byte3;

        // Padding difference used later to get to next multiple of 4 bytes
        uint32_t paddingDiff = (pixelWidth * 3) % 4;
        // Current number of bytes per pixels
        uint32_t totalBytes = pixelWidth * 3;
        // Get to the next multiple of 4 after the total bytes and subtract that by the total bytes
        uint32_t paddingBytes = (totalBytes + 4 - paddingDiff) - totalBytes;
        // Managing the padding per row of the image
        uint32_t ignoreBytes = 0, pixelCount = 0;

        // Read data in groups of 4
        for (uint32_t i = 0; i < bmpDIBHeader.sizeRawBitmapData; i += 3) {
            if (totalBytes % 4 != 0) {
                if (pixelCount != pixelWidth) {
                    in.read((char*) &byte1, 1);
                    in.read((char*) &byte2, 1);
                    in.read((char*) &byte3, 1);

                    temp = byte1 + (byte2 << 8) + (byte3 << 16);
                    pixelArray.push_back(temp);
                    ++pixelCount;
                } else if (pixelCount == pixelWidth) { // Store padding once width is width is reached
                    in.read((char*) &ignoreBytes, paddingBytes);
                    pixelCount = 0; // Reset pixel count to continually meet width for padding
                }
            } else if (totalBytes % 4 == 0) { // Proceed without padding
                in.read((char*) &byte1, 1);
                in.read((char*) &byte2, 1);
                in.read((char*) &byte3, 1);

                temp = byte1 + (byte2 << 8) + (byte3 << 16);
                pixelArray.push_back(temp);
            }
        }
    }
}

// Overloading extraction operator to read in bitmap file
istream& operator>>(istream& in, Bitmap& b) {
    b.readBitmapFileHeader(in, b);
    b.readBitmapDIBHeader(in, b);
   
    // Only read bitmap mask header if compression method is provided as 3
    if (b.bmpDIBHeader.compressionMethod == COMPRESSION_METHOD_3) {
        b.readBitmapMaskHeader(in, b);
    }

    b.readBitmapPixelArray(in, b);

    return in;
}

// Write the bitmap file header
void Bitmap::writeBitmapFileHeader(ostream& out, const Bitmap& b) const {
    out.write((char*) &b.bmpFileHeader.tag1, 1);
    out.write((char*) &b.bmpFileHeader.tag2, 1);
    out.write((char*) &b.bmpFileHeader.sizeOfBMP, 4);
    out.write((char*) &b.bmpFileHeader.garbage, 4);
    out.write((char*) &b.bmpFileHeader.offsetToPixelArray, 4);
}

// Write the DIB header
void Bitmap::writeBitmapDIBHeader(ostream& out, const Bitmap& b) const {
    out.write((char*) &b.bmpDIBHeader.sizeOfDIBHeader, 4);
    out.write((char*) &b.bmpDIBHeader.pixelWidth, 4);
    out.write((char*) &b.bmpDIBHeader.pixelHeight, 4);
    out.write((char*) &b.bmpDIBHeader.numColorPlanes, 2);
    out.write((char*) &b.bmpDIBHeader.colorDepth, 2);
    out.write((char*) &b.bmpDIBHeader.compressionMethod, 4);
    out.write((char*) &b.bmpDIBHeader.sizeRawBitmapData, 4);
    out.write((char*) &b.bmpDIBHeader.horizontalRes, 4);
    out.write((char*) &b.bmpDIBHeader.verticalRes, 4);
    out.write((char*) &b.bmpDIBHeader.colorsPalate, 4);
    out.write((char*) &b.bmpDIBHeader.importantColors, 4);
}

// Write the mask header
void Bitmap::writeBitmapMaskHeader(ostream& out, const Bitmap& b) const {
    out.write((char*) &b.bmpMaskHeader.mask1, 4);
    out.write((char*) &b.bmpMaskHeader.mask2, 4);
    out.write((char*) &b.bmpMaskHeader.mask3, 4);
    out.write((char*) &b.bmpMaskHeader.mask4, 4);
    out.write((char*) &b.bmpMaskHeader.orderOfMasks, 4);
}

// Write the pixel array data into the bitmap (modified or unmodified)
void Bitmap::writeBitmapPixelArray(ostream& out, const Bitmap& b) const {
    uint32_t colorDepth = bmpDIBHeader.colorDepth;

    // Store all of the data in groups of 4 for RGBA (32 BIT)
    if (colorDepth == RGBA) {
        for (uint32_t i : pixelArray) {
            out.write((char*) &i, 4);
        }
    }

    // Store all of the data in groups of 3 for RGBA (24 BIT)
    // May or may not include padding bytes
    if (colorDepth == RGB) {
        uint32_t pixelWidth = bmpDIBHeader.pixelWidth;

        // Get padding difference
        uint32_t paddingDiff = (pixelWidth * 3) % 4;
        // Get total bytes currently for the width
        uint32_t totalBytes = pixelWidth * 3;
        // Get to the next multiple of 4 after the total bytes and subtract that by the total bytes
        uint32_t paddingBytes = (totalBytes + 4 - paddingDiff) - totalBytes;
        // Keep track of every row to write the padding bytes to
        uint32_t pixelCount = 0, paddingVal = 0;

        for (uint32_t i : pixelArray) {
            out.write((char*) &i, 3);
            ++pixelCount;

            if (paddingDiff != 0 && pixelCount == pixelWidth) {
                out.write((char*) &paddingVal, paddingBytes);
                pixelCount = 0;
            }

        }
    }
}

// Overloading the insertion operator to write out the bitmap file
ostream& operator<<(ostream& out, const Bitmap& b) {
    b.writeBitmapFileHeader(out, b);
    b.writeBitmapDIBHeader(out, b);
   
    // Do not write bitmap mask header if compression method is 3
    if (b.bmpDIBHeader.compressionMethod == COMPRESSION_METHOD_3) {
        b.writeBitmapMaskHeader(out, b);

        // Add padding (color space info) for the ignored 64 bytes
        // to keep consistency between input and output bitmap files
        uint32_t index = 0;
        uint32_t temp = 0;
        for (; index < 64; ++index) {
            out.write((char*) &temp, 1);
        }
    }

    b.writeBitmapPixelArray(out, b);

    return out;
}

// Write the pixel at a given cell
void Bitmap::writePixel(const uint32_t& x, const uint32_t& y, const uint32_t& newPixel) {
    pixelArray[y * bmpDIBHeader.pixelWidth + x] = newPixel;
}

// Retrieve the pixel at a given cell
uint32_t Bitmap::getPixel(const uint32_t& x, const uint32_t& y) const {
    return pixelArray.at(y * bmpDIBHeader.pixelWidth + x);
}

// Retrieve the alpha value of a pixel in a given cell
uint32_t Bitmap::alpha(const uint32_t& x, const uint32_t& y) const {
     uint32_t pixel = pixelArray.at(y * bmpDIBHeader.pixelWidth + x);
     uint32_t mask4Shift = determineShift(bmpMaskHeader.mask4);

     if (bmpDIBHeader.compressionMethod == COMPRESSION_METHOD_3) {
        return (pixel >> mask4Shift) & (bmpMaskHeader.mask4 >> mask4Shift);
     }
     
     return pixel;
}

// Retrieve the red color value of a pixel in a given cell
uint32_t Bitmap::red(const uint32_t& x, const uint32_t& y) const {
     uint32_t pixel = pixelArray.at(y * bmpDIBHeader.pixelWidth + x);
     uint32_t mask1Shift = determineShift(bmpMaskHeader.mask1);

     if (bmpDIBHeader.compressionMethod == 3) {
        return (pixel >> mask1Shift) & (bmpMaskHeader.mask1 >> mask1Shift);
     }

     if (bmpDIBHeader.compressionMethod == 0) {
        return (pixel) & 0xFF;
     }

     return pixel;
}

// Retrieve the green color value of a pixel in a given cell
uint32_t Bitmap::green(const uint32_t& x, const uint32_t& y) const {
     uint32_t pixel = pixelArray.at(y * bmpDIBHeader.pixelWidth + x);
     uint32_t mask2Shift = determineShift(bmpMaskHeader.mask2);

     if (bmpDIBHeader.compressionMethod == COMPRESSION_METHOD_3) {
        return (pixel >> mask2Shift) & (bmpMaskHeader.mask2 >> mask2Shift);
     }

     if (bmpDIBHeader.compressionMethod == COMPRESSION_METHOD_0) {
        return (pixel >> 8) & 0xFF;
     }

     return pixel;
}

// Retrieve the blue color value of a pixel in a given cell
uint32_t Bitmap::blue(const uint32_t& x, const uint32_t& y) const {
     uint32_t pixel = pixelArray.at(y * bmpDIBHeader.pixelWidth + x);
     uint32_t mask3Shift = determineShift(bmpMaskHeader.mask3);

     if (bmpDIBHeader.compressionMethod == COMPRESSION_METHOD_3) {
        return (pixel >> mask3Shift) & (bmpMaskHeader.mask3 >> mask3Shift);
     }

     if (bmpDIBHeader.compressionMethod == COMPRESSION_METHOD_0) {
        return (pixel >> 16) & 0xFF;
     }

     return pixel;
}


void Bitmap::displayBMPFileHeader() const {
    cout << "Tag Type of BMP File: " << bmpFileHeader.tag1 << bmpFileHeader.tag2 << '\n';
    cout << "Size of BMP File: " << bmpFileHeader.sizeOfBMP << '\n';
    cout << "Garbage bytes: " << "0x00 0x00 0x00 0x00" << '\n';
    cout << "Offset to Pixel Data: " << bmpFileHeader.offsetToPixelArray << '\n';
}

void Bitmap::displayBMPDIBHeader() const {
    cout << "Size of DIB Header: " << bmpDIBHeader.sizeOfDIBHeader << '\n';
    cout << "Width px: " << bmpDIBHeader.pixelWidth << '\n';
    cout << "Height px: " << bmpDIBHeader.pixelHeight << '\n';
    cout << "Number of Color Planes: " << bmpDIBHeader.numColorPlanes << '\n';
    cout << "Color Depth: " << bmpDIBHeader.colorDepth << '\n';
    cout << "Compression Method: " << bmpDIBHeader.compressionMethod << '\n';
    cout << "Size of Raw Bitmap Data: " << bmpDIBHeader.sizeRawBitmapData << '\n';
    cout << "Horizontal Resolution: " << bmpDIBHeader.horizontalRes << '\n';
    cout << "Vertical Resolution: " << bmpDIBHeader.verticalRes << '\n';
    cout << "Colors Palate: " << bmpDIBHeader.colorsPalate << '\n';
    cout << "Important Colors: " << bmpDIBHeader.importantColors << '\n';
}

void Bitmap::displayBMPMaskHeader() const {
    cout << "Mask 1: " << hex << bmpMaskHeader.mask1 << '\n';
    cout << "Mask 2: " << hex << bmpMaskHeader.mask2 << '\n';
    cout << "Mask 3: " << hex << bmpMaskHeader.mask3 << '\n';
    cout << "Mask 4: " << hex << bmpMaskHeader.mask4 << '\n';
    cout << "Order of Masks: " << hex << bmpMaskHeader.orderOfMasks << '\n';
}
