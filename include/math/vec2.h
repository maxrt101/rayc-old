#ifndef _RAYC_VIDEO_VEC2_H_
#define _RAYC_VIDEO_VEC2_H_ 1

#include <string>
#include <cmath>

namespace rayc {

template <typename T>
struct Vec2 {
  T x;
  T y;

  inline Vec2() : x(0), y(0) {}
  inline Vec2(T x, T y) : x(x), y(y) {}

  inline Vec2<T>& operator=(const Vec2<T>& rhs) {
    x = rhs.x;
    y = rhs.y;
    return *this;
  }

  inline T dot(const Vec2<T>& rhs) const { return x * rhs.x + y * rhs.y; }
  inline T cross(const Vec2<T>& rhs) const { return x * rhs.y - y * rhs.x; }

  inline Vec2<T>  operator +(const Vec2<T>& rhs) const { return {x + rhs.x, y + rhs.y}; }
  inline Vec2<T>  operator +(const T& rhs) const { return {x + rhs, y + rhs}; }
  inline Vec2<T>  operator -(const Vec2<T>& rhs) const { return {x - rhs.x, y - rhs.y}; }
  inline Vec2<T>  operator -(const T& rhs) const { return {x - rhs, y - rhs}; }
  inline Vec2<T>  operator *(const Vec2<T>& rhs) const { return {x * rhs.x, y * rhs.y}; }
  inline Vec2<T>  operator *(const T& rhs) const { return {x * rhs, y * rhs}; }
  inline Vec2<T>  operator /(const Vec2<T>& rhs) const { return {x / rhs.x, y / rhs.y}; }
  inline Vec2<T>  operator /(const T& rhs) const { return {x / rhs, y / rhs}; }
  inline Vec2<T>& operator+=(const Vec2<T>& rhs) { x += rhs.x; y += rhs.y; return *this; }
  inline Vec2<T>& operator+=(const T& rhs) { x += rhs; y = rhs; return *this; }
  inline Vec2<T>& operator-=(const Vec2<T>& rhs) { x -= rhs.x; y -= rhs.y; return *this; }
  inline Vec2<T>& operator-=(const T& rhs) { x -= rhs; y -= rhs; return *this; }
  inline Vec2<T>& operator*=(const Vec2<T>& rhs) { x *= rhs.x; y *= rhs.y; return *this; }
  inline Vec2<T>& operator*=(const T& rhs) { x *= rhs; y *= rhs; return *this; }
  inline Vec2<T>& operator/=(const Vec2<T>& rhs) { x /= rhs.x; y /= rhs.y; return *this; }
  inline Vec2<T>& operator/=(const T& rhs) { x /= rhs; y /= rhs; return *this; }
  inline Vec2<T>  operator-() const { return {-x, -y}; }
  inline bool     operator==(const Vec2<T>& rhs) const { return x == rhs.x && y == rhs.y; }
  inline bool     operator!=(const Vec2<T>& rhs) const { return x != rhs.x || y != rhs.y; }

  inline std::string toString() const { return "(" + std::to_string(x) + ", " + std::to_string(y) + ")"; }
  inline operator Vec2<int>() const { return Vec2<int>((int)x, (int)y); }
  inline operator Vec2<unsigned int>() const { return Vec2<unsigned int>((unsigned int)x, (unsigned int)y); }
  inline operator Vec2<float>() const { return Vec2<float>((float)x, (float)y); }
  inline operator Vec2<double>() const { return Vec2<double>((double)x, (double)y); }
};

typedef Vec2<int> Vec2i;
typedef Vec2<unsigned int> Vec2u;
typedef Vec2<float> Vec2f;
typedef Vec2<double> Vec2d;

} /* namespace rayc */

#endif /* _RAYC_VIDEO_VEC2_H_ */