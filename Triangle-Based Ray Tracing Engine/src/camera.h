#ifndef CAMERA_H
#define CAMERA_H
#include "Ray.h"
#include "Vec3.h"

class Camera
{
    public:
        Camera();
        Camera(float distance, float left, float right, float bottom, float top, int nx, int ny, Vec3 gaze, Vec3 up, Vec3 origin = Vec3(0.0, 0.0, 0.0));
        Ray getRay(int i, int j) const;
        float getDistance() const;
        float getLeft() const;
        float getRight() const;
        float getBottom() const;
        float getTop() const;
        int getNx() const;
        int getNy() const;
        void getGaze();
        void getOrigin();
        void getUp();

    private:
        float distance, left, right, bottom, top;
        int nx, ny;
        Vec3 origin, gaze, up;
        Vec3 u, v, w, m, q;
        
        void calculateCameraParameters();
};

#endif // CAMERA_H