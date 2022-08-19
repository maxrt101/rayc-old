#ifndef _RAYC_VIDEO_VEC3_H_
#define _RAYC_VIDEO_VEC3_H_ 1

namespace rayc {

template <typename T>
struct Vec3 {
  T x;
  T y;
  T z;

  inline Vec3() : x(0), y(0), z(0) {}
  inline Vec3(T x, T y, T z) : x(x), y(y), z(z) {}

  inline Vec3<T>& operator=(const Vec3<T>& rhs) {
    x = rhs.x;
    y = rhs.y;
    z = rhs.z;
    return *this;
  }

  inline bool operator==(const Vec3<T>& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z; }
  inline bool operator!=(const Vec3<T>& rhs) const { return x != rhs.x || y != rhs.y || z != rhs.z; }
};

typedef Vec3<int> Vec3i;
typedef Vec3<unsigned int> Vec3u;
typedef Vec3<float> Vec3f;
typedef Vec3<double> Vec3d;

} /* namespace rayc */

#endif /* _RAYC_VIDEO_VEC3_H_ */