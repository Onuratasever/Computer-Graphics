#include "Camera.h"
#include <iostream>

using namespace std;
Camera::Camera() : distance(1.0f), left(-1.0f), right(1.0f), bottom(-1.0f), top(1.0f) {}

Camera::Camera(float distance, float left, float right, float bottom, float top, int nx, int ny, Vec3 gaze, Vec3 up, Vec3 origin) 
    : distance(distance), left(left), right(right), bottom(bottom), top(top), nx(nx), ny(ny), gaze(gaze), up(up), origin(origin) 
{
    calculateCameraParameters();
}

Ray Camera::getRay(int i, int j) const 
{
    float su, sv;
    Vec3 s;

    // Write camera parameters to console
    // cout << "Camera parameters: " << endl;
    // cout << "Distance: " << distance << endl;
    // cout << "Left: " << left << endl;
    // cout << "Right: " << right << endl;
    // cout << "Bottom: " << bottom << endl;
    // cout << "Top: " << top << endl;
    // cout << "Nx: " << nx << endl;
    // cout << "Ny: " << ny << endl;
    // cout << "Gaze: " << gaze.x << " " << gaze.y << " " << gaze.z << endl;
    // cout << "Up: " << up.x << " " << up.y << " " << up.z << endl;
    // cout << "Origin: " << origin.x << " " << origin.y << " " << origin.z << endl;
    su = (right - left) * (i + 0.5f) / nx;
    sv = (top - bottom) * (j + 0.5f) / ny;

    s = q + u*su - v*sv;

    //cout << "s: " << s.x << " " << s.y << " " << s.z << endl;
    Ray ray(origin, s - origin);

    return ray;
}

float Camera::getDistance() const 
{
    return distance;
}

float Camera::getLeft() const 
{
    return left;
}

float Camera::getRight() const 
{
    return right;
}

float Camera::getBottom() const 
{
    return bottom;
}

float Camera::getTop() const 
{
    return top;
}

int Camera::getNx() const 
{
    return nx;
}

int Camera::getNy() const 
{
    return ny;
}

void Camera::calculateCameraParameters() 
{
    w = gaze.normalized() * -1.0f; // Inverted gaze direction
    u = up.cross(w).normalized(); // Orthogonal to both up and gaze
    v = w.cross(u).normalized(); // Orthogonal to both w and u

    // print u
    cout << "u: " << u.x << " " << u.y << " " << u.z << endl;
    // print v
    cout << "v: " << v.x << " " << v.y << " " << v.z << endl;
    m = origin + ((w*-1) * distance);
    // print m
    // cout << "m: " << m.x << " " << m.y << " " << m.z << endl;
    q = m + u*left  + v*top;
    // print q
    // cout << "q: " << q.x << " " << q.y << " " << q.z << endl;
}

void Camera::getGaze() 
{
    cout << "Gaze direction: " << gaze.x << " " << gaze.y << " " << gaze.z << endl;
}
void Camera::getOrigin() 
{
    cout << "Camera origin: " << origin.x << " " << origin.y << " " << origin.z << endl;
}
void Camera::getUp() 
{
    cout << "Camera up vector: " << up.x << " " << up.y << " " << up.z << endl;
}