#include <stdexcept>
#include <iostream>
#include <cmath>
#include <cassert>

using Real = double;

class Math {
public:
    static constexpr Real EPSILON = 1e-9;
    static constexpr Real SMALL_VALUE = 1e-12;
    static constexpr Real PI = 3.141592653589793;

    static Real square(Real x) { return x * x; }

    static Real clamp(Real value, Real minVal, Real maxVal) {
        if (value < minVal) {
            return minVal;
        }

        if (value > maxVal) {
            return maxVal;
        }

        return value;
    }

    static bool isNearlyZero(Real x) { return std::abs(x) <= EPSILON; }

    static bool isNearlyEqual(Real a, Real b) { return std::abs(a - b) <= EPSILON; }
};


class Vec3 {
public:
    Real x;
    Real y;
    Real z;

    Vec3() {
        this->x = 0;
        this->y = 0;
        this->z = 0;
    }

    Vec3(Real x_, Real y_, Real z_) {
        this->x = x_;
        this->y = y_;
        this->z = z_;
    }

    Real getX() const { return x; }
    Real getY() const { return y; }
    Real getZ() const { return z; }

    Vec3 operator+(const Vec3& other) const {
        return Vec3(this->x + other.x, this->y + other.y, this->z + other.z);
    }

    Vec3 operator-(const Vec3& other) const {
        return Vec3(this->x - other.x, this->y - other.y, this->z - other.z);
    }

    Vec3 operator*(Real scalar) const {
        return Vec3(this->x * scalar, this->y * scalar, this->z * scalar);
    }

    Vec3 operator/(Real scalar) const {
        if (Math::isNearlyZero(scalar))
            throw std::runtime_error("ZeroDivision");
        return Vec3(this->x / scalar, this->y / scalar, this->z / scalar);
    }

    Vec3& operator+=(const Vec3& other) {
        this->x += other.x;
        this->y += other.y;
        this->z += other.z;
        return *this;
    }

    Vec3& operator-=(const Vec3& other) {
        this->x -= other.x;
        this->y -= other.y;
        this->z -= other.z;
        return *this;
    }

    Real length() const {
        return std::sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
    }

    Real lengthSq() const {
        return (this->x * this->x + this->y * this->y + this->z * this->z);
    }

    bool isZero() const {
        return Math::isNearlyZero(this->length());
    }

    Vec3 normalized() const {
        if (this->isZero())
            return *this;
        return *this/this->length();
    }

    void normalize() {
            if (!(this->isZero()))
                *this = *this / this->length();
    }

    static Real dot(const Vec3& a, const Vec3& b) {
        return (a.x * b.x + a.y * b.y + a.z * b.z);
    }

    static Vec3 cross(const Vec3& a, const Vec3& b) {
        return Vec3(a.y * b.z - a.z * b.y, -a.x * b.z + a.z * b.x, a.x * b.y - a.y * b.x);
    }

    static Real distance(const Vec3& a, const Vec3& b) {
        return (a - b).length();
    }

    static Real distanceSq(const Vec3& a, const Vec3& b) {
        return (a - b).lengthSq();
    }

    static Vec3 zero() {
        return Vec3(0.0, 0.0, 0.0);
    }
     void print() const {
        std::cout << "(" << x << ", " << y << ", " << z << ")";
    }
};


class Displacement {
private:
    Vec3 delta;
    Real distanceSq;

public:
    Displacement() : delta(Vec3::zero()), distanceSq(0.0) {}

    Displacement(const Vec3& delta_): delta(delta_), distanceSq(delta_.lengthSq()) {}

    const Vec3& getDelta() const {
        return delta;
    }

    Real getDistanceSquared() const {
        return distanceSq;
    }

    Real getDistance() const {
        return std::sqrt(distanceSq);
    }

    static Displacement between(const Vec3& from, const Vec3& to) {
        return Displacement(to - from);
    }
};


class PeriodicBox {
private:
    Vec3 size;

public:
    PeriodicBox() : size(0.0, 0.0, 0.0) {}

    PeriodicBox(const Vec3& boxSize) : size(boxSize) {}

    const Vec3& getSize() const {
        return size;
    }

    void setSize(const Vec3& newSize) {
        size = newSize;
    }

    Real minimalImage1D(Real dx, Real boxLength) const {
        if (boxLength <= 0.0) {
            return dx;
        }

        Real half = 0.5 * boxLength;

        if (dx > half) {
            dx -= boxLength;
        } else if (dx < -half) {
            dx += boxLength;
        }

        return dx;
    }

    Vec3 applyMinimalImage(const Vec3& delta) const {
        return Vec3(
            minimalImage1D(delta.getX(), size.getX()),
            minimalImage1D(delta.getY(), size.getY()),
            minimalImage1D(delta.getZ(), size.getZ()));
    }

    Real wrapCoordinate(Real value, Real boxLength) const {
        if (boxLength <= 0.0) {
            return value;
        }

        value = std::fmod(value, boxLength);
        if (value < 0.0) {
            value += boxLength;
        }

        return value;
    }

    Vec3 wrapPosition(const Vec3& position) const {
        return Vec3(
            wrapCoordinate(position.getX(), size.getX()),
            wrapCoordinate(position.getY(), size.getY()),
            wrapCoordinate(position.getZ(), size.getZ()));
    }

    Displacement periodicDisplacement(const Vec3& from, const Vec3& to) const {
        Vec3 rawDelta = to - from;
        Vec3 correctedDelta = applyMinimalImage(rawDelta);
        return Displacement(correctedDelta);
    }
};


class Mat4 {
private:
    Real m[16]; // index = col * 4 + row

public:
    Mat4() {
        for (int i = 0; i < 16; i++) {
            m[i] = 0.0;
        }
    }

    Real& at(int row, int col) {
        return m[col * 4 + row];
    }

    const Real& at(int row, int col) const {
        return m[col * 4 + row];
    }

    const Real* data() const {
        return m;
    }

    Real* data() {
        return m;
    }

    static Mat4 identity() {
        Mat4 result;

        result.at(0, 0) = 1.0;
        result.at(1, 1) = 1.0;
        result.at(2, 2) = 1.0;
        result.at(3, 3) = 1.0;

        return result;
    }

    static Mat4 translation(const Vec3& t) {
        Mat4 result = identity();

        result.at(0, 3) = t.x;
        result.at(1, 3) = t.y;
        result.at(2, 3) = t.z;

        return result;
    }

    static Mat4 scale(const Vec3& s) {
        Mat4 result = identity();

        result.at(0, 0) = s.x;
        result.at(1, 1) = s.y;
        result.at(2, 2) = s.z;

        return result;
    }

    static Mat4 rotationX(Real angle) {
        Mat4 result = identity();
        Real c = std::cos(angle);
        Real s = std::sin(angle);

        result.at(1, 1) = c;
        result.at(1, 2) = -s;
        result.at(2, 1) = s;
        result.at(2, 2) = c;

        return result;
    }

    static Mat4 rotationY(Real angle) {
        Mat4 result = identity();
        Real c = std::cos(angle);
        Real s = std::sin(angle);

        result.at(0, 0) = c;
        result.at(0, 2) = s;
        result.at(2, 0) = -s;
        result.at(2, 2) = c;

        return result;
    }

    static Mat4 rotationZ(Real angle) {
        Mat4 result = identity();
        Real c = std::cos(angle);
        Real s = std::sin(angle);

        result.at(0, 0) = c;
        result.at(0, 1) = -s;
        result.at(1, 0) = s;
        result.at(1, 1) = c;

        return result;
    }

    static Mat4 perspective(Real fov, Real aspect, Real nearPlane, Real farPlane) {
        if (Math::isNearlyZero(aspect)) {
            throw std::runtime_error("ZeroDivision (aspect)!");
        }

        if (Math::isNearlyZero(std::tan(fov * 0.5))) {
            throw std::runtime_error("ZeroDivision (fov)!");
        }

        if (Math::isNearlyEqual(nearPlane, farPlane)) {
            throw std::runtime_error("ZeroDivision (near = far)!");
        }

        Real tanHalfFov = std::tan(fov * 0.5);
        Mat4 result;

        result.at(0, 0) = 1.0 / (aspect * tanHalfFov);
        result.at(1, 1) = 1.0 / tanHalfFov;
        result.at(2, 2) = -(farPlane + nearPlane) / (farPlane - nearPlane);
        result.at(2, 3) = -(2.0 * farPlane * nearPlane) / (farPlane - nearPlane);
        result.at(3, 2) = -1.0;

        return result;
    }

    static Mat4 lookAt(const Vec3& eye, const Vec3& target, const Vec3& up) {
        Vec3 forward = (target - eye).normalized();
        Vec3 right = Vec3::cross(forward, up).normalized();
        Vec3 trueUp = Vec3::cross(right, forward);
        Mat4 result = identity();

        result.at(0, 0) = right.x;
        result.at(1, 0) = right.y;
        result.at(2, 0) = right.z;

        result.at(0, 1) = trueUp.x;
        result.at(1, 1) = trueUp.y;
        result.at(2, 1) = trueUp.z;

        result.at(0, 2) = -forward.x;
        result.at(1, 2) = -forward.y;
        result.at(2, 2) = -forward.z;

        result.at(0, 3) = -Vec3::dot(right, eye);
        result.at(1, 3) = -Vec3::dot(trueUp, eye);
        result.at(2, 3) = Vec3::dot(forward, eye);

        return result;
    }

    Mat4 operator*(const Mat4& other) const {
        Mat4 result;

        for (int row = 0; row < 4; row++) {
            for (int col = 0; col < 4; col++) {
                Real sum = 0.0;

                for (int k = 0; k < 4; k++) {
                    sum += this->at(row, k) * other.at(k, col);
                }

                result.at(row, col) = sum;
            }
        }

        return result;
    }

    static Mat4 makeModelMatrix(const Vec3& position, const Vec3& rotation, const Vec3& scaleVec) {
        return Mat4::translation(position) * Mat4::rotationZ(rotation.z) * 
            Mat4::rotationY(rotation.y) * Mat4::rotationX(rotation.x) * Mat4::scale(scaleVec);
    }

    void print() const {
        for (int row = 0; row < 4; row++) {
            for (int col = 0; col < 4; col++) {
                std::cout << at(row, col) << " ";
            }
            std::cout << "\n";
        }
    }
};


class AABB {
private:
    Vec3 minPoint;
    Vec3 maxPoint;

public:
    AABB() : minPoint(Vec3::zero()), maxPoint(Vec3::zero()) {}

    AABB(const Vec3& minP, const Vec3& maxP) : minPoint(minP), maxPoint(maxP) {}

    Vec3 getMin() const { return minPoint; }
    Vec3 getMax() const { return maxPoint; }

    Vec3 size() const {
        return maxPoint - minPoint;
    }

    Vec3 center() const {
        return (minPoint + maxPoint) * 0.5;
    }

    bool contains(const Vec3& p) const {
        return p.getX() >= minPoint.getX() && p.getX() <= maxPoint.getX() &&
               p.getY() >= minPoint.getY() && p.getY() <= maxPoint.getY() &&
               p.getZ() >= minPoint.getZ() && p.getZ() <= maxPoint.getZ();
    }
};



int main() {
    Vec3 a(1.0, 2.0, 3.0);
    Vec3 b(4.0, 6.0, 3.0);

    std::cout << "a = ";
    a.print();
    std::cout << "\n";

    std::cout << "b = ";
    b.print();
    std::cout << "\n";

    Vec3 delta = b - a;
    std::cout << "delta = ";
    delta.print();
    std::cout << "\n";

    std::cout << "distanceSq = " << Vec3::distanceSq(a, b) << "\n";
    std::cout << "distance = " << Vec3::distance(a, b) << "\n";

    PeriodicBox box(Vec3(10.0, 10.0, 10.0));
    Vec3 p1(0.5, 0.5, 0.5);
    Vec3 p2(9.8, 0.5, 0.5);

    Displacement pd = box.periodicDisplacement(p1, p2);

    std::cout << "periodic delta = ";
    pd.getDelta().print();
    std::cout << "\n";

    std::cout << "periodic distanceSq = " << pd.getDistanceSquared() << "\n";

    Mat4 model = Mat4::makeModelMatrix(Vec3(1.0, 2.0, 3.0), Vec3(0.1, 0.2, 0.3), Vec3(1.0, 1.0, 1.0));

    std::cout << "model matrix:\n";
    model.print();

    return 0;
}
