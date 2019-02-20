#include "bitmapException.h"

BitmapException::BitmapException(string s) : str(s) {}

BitmapException::~BitmapException() {}

// Redefined the virtual function so that
// specific values are returned
const char* BitmapException::what() const throw() {
    return str.c_str();
}
