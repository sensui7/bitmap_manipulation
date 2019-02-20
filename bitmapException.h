#include <string>
#include <exception>

using namespace std;

// Inherit from std::exception to acquire accessible conversion
// so that the catch block in main.cpp could function correctly
// once an exception is thrown
class BitmapException : public exception {
    public:
        BitmapException(string s);
        ~BitmapException();

        // Override virtual function
        const char* what() const throw();
        
    private:
        string str;
};
