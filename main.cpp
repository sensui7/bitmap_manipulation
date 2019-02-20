#include <iostream>
#include <fstream>
#include <string>
#include "bitmap.h"
#include "bitmapException.h"

int main(int argc, char** argv) {
    if(argc != 4) {
        cout << "usage:\n"
             << "bitmap option inputfile.bmp outputfile.bmp\n"
             << "options:\n"
             << "  -i identity\n"
             << "  -c cell shade\n"
             << "  -g gray scale\n"
             << "  -p pixelate\n"
             << "  -b blur\n"
             << "  -r90 rotate 90\n"
             << "  -r180 rotate 180\n"
             << "  -r270 rotate 270\n"
             << "  -v flip vertically\n"
             << "  -h flip horizontally\n"
             << "  -d1 flip diagonally 1\n"
             << "  -d2 flip diagonally 2\n"
             << "  -grow scale the image by 2\n"
             << "  -shrink scale the image by .5" << endl;

        return 0;
    }

    try {
        string flag(argv[1]);
        string infile(argv[2]);
        string outfile(argv[3]);

        ifstream in;
        Bitmap image;
        ofstream out;

        in.open(infile, ios::binary);
        in >> image;
        in.close();

        if(flag == "-c")
        {
            image.cellShade();
        }
        if(flag == "-g")
        {
            image.grayscale();
        }
        if(flag == "-p")
        {
            image.pixelate();
        }
        if(flag == "-b")
        {
            image.blur();
        }
        if(flag == "-r90")
        {
            image.rot90();
        }
        if(flag == "-r180")
        {
            image.rot180();
        }
        if(flag == "-r270")
        {
            image.rot270();
        }
        if(flag == "-v")
        {
            image.flipv();
        }
        if(flag == "-h")
        {
            image.fliph();
        }
        if(flag == "-d1")
        {
            image.flipd1();
        }
        if(flag == "-d2")
        {
            image.flipd2();
        }
        if(flag == "-grow")
        {
            image.scaleUp();
        }
        if(flag == "-shrink")
        {
            image.scaleDown();
        }

        out.open(outfile, ios::binary);
        out << image;
        out.close();
    }
    catch(BitmapException& caught) {
        cout << caught.what() << endl;
    }

    return 0;
}
