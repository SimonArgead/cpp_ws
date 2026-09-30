#include <cmath>
#include <iostream>

class Vector3D{
public:
    double x, y, z; // define that we have x, y, z coordinates in our 3D vector

    Vector3D(double x, double y, double z) : x(x), y(y), z(z) {}

    // Operator "+" overload
    Vector3D operator+(const Vector3D& other) const{
        return Vector3D(x + other.x, y + other.y, z + other.z);
    } 

    // Operator "-" overload
    Vector3D operator-(const Vector3D& other) const{
        return Vector3D(x - other.x, y - other.y, z - other.z);
    } 

    // Operator "==" overload
    Vector3D operator==(const Vector3D& other) const{
        return Vector3D(x == other.x && y == other.y && z == other.z);
    } 
};

int main() {
    Vector3D a(1, 2, 3);
    Vector3D b(4, 5, 6);
    Vector3D c = a + b;
    std::cout << c << std::endl;
}
