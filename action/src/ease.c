#include <math/math.h>
#include <action/ease.h>

float ease_linear(const float total, const float remain)
{
        float t;
        if(remain >= EPSILON) {
                t = (total - remain) / total;
        } else {
                t = 1.0f;
        }
        return t;
}

float ease_quadratic_in(const float total, const float remain)
{
        float t;
        if(remain >= EPSILON) {
                t = (total - remain) / total;
                t = t * t;
        } else {
                t = 1.0f;
        }
        return t;
}

float ease_quadratic_out(const float total, const float remain)
{
        float t;
        if(remain >= EPSILON) {
                t = (total - remain) / total;
                t = t * (2 - t);
        } else {
                t = 1.0f;
        }
        return t;
}

float ease_quadratic_in_out(const float total, const float remain)
{
        float t;
        if(remain >= EPSILON) {
                t = total - remain;
                t /= total / 2;
                if(t < 1.0f) {
                        t = t * t * 0.5f;
                } else {
                        t = t - 1;
                        t = -0.5f * (t * (t - 2) - 1);
                }
        } else {
                t = 1.0f;
        }
        return t;
}

float ease_cubic_in(const float total, const float remain)
{
        float t;
        if(remain >= EPSILON) {
                t = (total - remain) / total;
                t = t * t * t;
        } else {
                t = 1.0f;
        }
        return t;
}

float ease_cubic_out(const float total, const float remain)
{
        float t;
        if(remain >= EPSILON) {
                t = (total - remain) / total;
                t = t - 1;
                t = (t * t * t + 1);
        } else {
                t = 1.0f;
        }
        return t;
}

float ease_cubic_in_out(const float total, const float remain)
{
        float t;
        if(remain >= EPSILON) {
                t = total - remain;
                t /= total / 2;
                if(t < 1.0f) {
                        t = t * t * t * 0.5;
                } else {
                        t = t - 2;
                        t = (t * t * t + 2) / 2;
                }
        } else {
                t = 1.0f;
        }
        return t;
}

float ease_quartic_in(const float total, const float remain)
{
        float t;
        if(remain >= EPSILON) {
                t = (total - remain) / total;
                t = t * t * t * t;
        } else {
                t = 1.0f;
        }
        return t;
}

float ease_quartic_out(const float total, const float remain)
{
        float t;
        if(remain >= EPSILON) {
                t = (total - remain) / total;
                t = t - 1;
                t = -(t * t * t * t - 1);
        } else {
                t = 1.0f;
        }
        return t;
}

float ease_quartic_in_out(const float total, const float remain)
{
        float t;
        if(remain >= EPSILON) {
                t = total - remain;
                t /= total / 2;
                if(t < 1.0f) {
                        t = t * t * t * t * 0.5f;
                } else {
                        t = t - 2;
                        t = -0.5f * (t * t * t * t - 2);
                }
        } else {
                t = 1.0f;
        }
        return t;
}

float ease_quintic_in(const float total, const float remain)
{
        float t;
        if(remain >= EPSILON) {
                t = (total - remain) / total;
                t = t * t * t * t * t;
        } else {
                t = 1.0f;
        }
        return t;
}

float ease_quintic_out(const float total, const float remain)
{
        float t;
        if(remain >= EPSILON) {
                t = (total - remain) / total;
                t = t - 1;
                t = (t * t * t * t * t + 1);
        } else {
                t = 1.0f;
        }
        return t;
}

float ease_quintic_in_out(const float total, const float remain)
{
        float t;
        if(remain >= EPSILON) {
                t = total - remain;
                t /= total / 2;
                if(t < 1.0f) {
                        t = t * t * t * t * t * 0.5f;
                } else {
                        t = t - 2;
                        t = (t * t * t * t * t + 2) * 0.5f;
                }
        } else {
                t = 1.0f;
        }
        return t;
}

float ease_sinusoidal_in(const float total, const float remain)
{
        float t;
        if(remain >= EPSILON) {
                t = (total - remain) / total;
                t = -cos(t * M_PI * 0.5f) + 1;
        } else {
                t = 1.0f;
        }
        return t;
}

float ease_sinusoidal_out(const float total, const float remain)
{
        float t;
        if(remain >= EPSILON) {
                t = (total - remain) / total;
                t = sin(t * M_PI * 0.5f);
        } else {
                t = 1.0f;
        }
        return t;
}

float ease_sinusoidal_in_out(const float total, const float remain)
{
        float t;
        if(remain >= EPSILON) {
                t = (total - remain) / total;
                t = -0.5f * (cos(t * M_PI) - 1);
        } else {
                t = 1.0f;
        }
        return t;
}

float ease_exponential_in(const float total, const float remain)
{
        float t;
        if(remain >= EPSILON) {
                t = (total - remain) / total;
                t = (float)pow(2.0, 10.0 * (t - 1));
        } else {
                t = 1.0f;
        }
        return t;
}

float ease_exponential_out(const float total, const float remain)
{
        float t;
        if(remain >= EPSILON) {
                t = (total - remain) / total;
                t = -(float)pow(2.0, -10.0 * t) + 1;
        } else {
                t = 1.0f;
        }
        return t;
}

float ease_exponential_in_out(const float total, const float remain)
{
        float t;
        if(remain >= EPSILON) {
                t = total - remain;
                t /= total / 2;
                if(t < 1.0f) {
                        t = 0.5f * (float)pow(2.0, 10.0 * (t - 1));
                } else {
                        t = t - 1;
                        t = 0.5f * (-(float)pow(2.0, -10.0 * t) + 2);
                }
        } else {
                t = 1.0f;
        }
        return t;
}

float ease_circular_in(const float total, const float remain)
{
        float t;
        if(remain >= EPSILON) {
                t = (total - remain) / total;
                t = -((float)sqrt(1.0 - t * t) - 1);
        } else {
                t = 1.0f;
        }
        return t;
}

float ease_circular_out(const float total, const float remain)
{
        float t;
        if(remain >= EPSILON) {
                t = (total - remain) / total;
                t = t - 1;
                t = (float)sqrt(1.0 - t * t);
        } else {
                t = 1.0f;
        }
        return t;
}

float ease_circular_in_out(const float total, const float remain)
{
        float t;
        if(remain >= EPSILON) {
                t = total - remain;
                t /= total / 2;
                if(t < 1.0f) {
                        t = -0.5f * ((float)sqrt(1 - t * t) - 1);
                } else {
                        t = t - 2;
                        t = 0.5f * ((float)sqrt(t - t * t) + 1);
                }
        } else {
                t = 1.0f;
        }
        return t;
}
