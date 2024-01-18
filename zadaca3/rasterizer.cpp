#include <iostream>
#include <cmath>
#include "tgaimage.h"
#include "tgaimage.cpp"

using namespace std;

// compile with:
// g++ rasterizer.cpp tgaimage.cpp -o out.exe

void set_color(int x, int y, TGAImage &image, TGAColor color, bool invert = false)
{
    image.set(y, x, color);    
}

float line(float x0, float y0, float x1, float y1, float x, float y)
{
    return (y0 - y1) * x + (x1 - x0) * y + x0 * y1 - x1 * y0;
}

void line_naive(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
    for (float t = 0; t < 1; t += 0.01)
    {
        int x = x0 * (1.0f - t) + x1 * t;
        int y = x0 * (1.0f - t) + y1 * t;
        set_color(x, y, image, color);
    }
}

void line_midpoint(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color, bool invert)
{
    int y = y0;
    int d = line(x0, y0, x1, y1, x0 + 1, y0 + 0.5);
    
    int dx = (x1 - x0);
    int dy = (y1 - y0);
    int increment = 1;
    
    if (dy < 0)
    {
        // pravac ide od gore prema dolje
        dy = -dy;
        increment = -increment;
    }
    
    for (int x = x0; x <= x1; ++x)
    {
        if (invert)
        {
            set_color(x, y, image, color);
        }
        else
        {
            set_color(y, x, image, color);       
        }
        
        if (d < 0)
        {
            y = y + increment;
            d = d + dx - dy;
        }
        else
        {
            d = d - dy;
        }
    }
}

void draw_line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
    // 'transponiraj' duzinu ako je veci od 1
    bool invert = false;
    if (abs(x1 - x0) < abs(y1 - y0))
    {
        swap(x0, y0);
        swap(x1, y1);
        invert = true;
    }
    
    // zamijeni tocke ako pravac ide s desna na lijevo
    if (x1 < x0)
    {
        swap(x0, x1);
        swap(y0, y1);
    }
    
    // nacrtaj duzinu
    line_midpoint(x0, y0, x1, y1, image, color, invert);
}

// dimenzije slike
const int width  = 512;
const int height = 512;

// definirajmo boje
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0, 0, 255);
const TGAColor blue  = TGAColor(0, 0, 255, 255);

void draw_triangle_2d(int x0, int y0, int x1, int y1, int x2, int y2,TGAImage& image,const TGAColor& color){
    draw_line(x0, y0, x1, y1, image, color);
    draw_line(x1,y1,x2,y2, image, color);
    draw_line(x2,y2,x0,y0, image, color);
}

void draw_triangle_gouraud(int x0, int y0, int x1, int y1, int x2, int y2, TGAImage& image, const TGAColor& c0,const TGAColor& c1, const TGAColor& c2){
    // a(x0,y0), b(x1,y1), c(x2,y2)
    // alfa = fbc(x,y) / fbc(xa,ya)
    // beta = fac(x,y) / fac(xb,yb)
    // gama = fab(x,y) / fab(xc,yc)

    int x_min = min(x0,min(x1,x2));
    int x_max = max(x0,max(x1,x2));
    int y_min = min(y0,min(y1,y2));
    int y_max = max(y0,max(y1,y2));

    for(int y = y_min; y <= y_max; y++){
        for(int x = x_min; x <= x_max; ++x){
            float alfa = line(x1,y1, x2,y2, x,y) / line(x1,y1,x2,y2, x0,y0);
            float beta = line(x0,y0, x2,y2, x,y) / line(x0,y0,x2,y2, x1,y1);
            float gama = line(x0,y0, x1,y1, x,y) / line(x0,y0,x1,y1, x2,y2);
            //cout << alfa << " " << beta << " " << gama << endl;
            if((0 <= alfa  and alfa <= 1) and (0 <= beta  and beta <= 1) and (0 <= gama and gama <= 1)){
                char r = (char)(alfa * c0.r + beta * c1.r + gama * c2.r);
                char g = (char)(alfa * c0.g + beta * c1.g + gama * c2.g);
                char b = (char)(alfa * c0.b + beta * c1.b + gama * c2.b);
                char a = (char)(alfa * c0.a + beta * c1.a + gama * c2.a);

                TGAColor color = TGAColor(r,g,b,a);
                set_color(x,y, image, color);
            }
        }
    }

}





class Vec4f {
public:
    float x, y, z, w;

    Vec4f(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 1.0f)
            : x(x), y(y), z(z), w(w) {}


    Vec4f operator*(const float scalar) const {
        return Vec4f(x * scalar, y * scalar, z * scalar, w * scalar);
    }



    Vec4f operator*(const Vec4f& other) const {
        return Vec4f(x * other.x, y * other.y, z * other.z, w * other.w);
    }





    float operator[](int index) const {
        if (index == 0)
            return x;
        if (index == 1)
            return y;
        if (index == 2)
            return z;
        if (index == 3)
            return w;
        return 0.0f;
    }

    Vec4f& operator/=(const float scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        w /= scalar;
        return *this;
    }


};


class Matrix{
public:
    float m[4][4];

    Matrix() {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                m[i][j] = 0.0f;
    }


    Matrix perspective(int width, int height, int d) {
        Matrix mat;

        mat.m[0][0] = d;
        mat.m[1][1] = d;
        mat.m[2][2] = 1.0f;
        mat.m[3][2] = -1.0f / d;
        mat.m[2][3] = 1.0f;
        mat.m[3][3] = 0.0f;

        return mat;
    }


    Matrix operator*(const Matrix& other) const {
        Matrix result;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                for (int k = 0; k < 4; ++k)
                    result.m[i][j] += m[i][k] * other.m[k][j];
        return result;
    }


    Vec4f operator*(const Vec4f& v) const {
        return Vec4f(
                m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w,
                m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w,
                m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w,
                m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w
        );
    }
};


void draw_triangle( int x0, int y0, int z0,
                    int x1, int y1, int z1,
                    int x2, int y2, int z2,
                    TGAImage& image, const TGAColor& c)
{

    float d = 1.0f;
    Matrix projection_matrix;
    projection_matrix = projection_matrix.perspective(width, height, d);

    Vec4f v0(x0, y0, z0, 1.0f);
    Vec4f v1(x1, y1, z1, 1.0f);
    Vec4f v2(x2, y2, z2, 1.0f);


    v0 = projection_matrix * v0;
    v1 = projection_matrix * v1;
    v2 = projection_matrix * v2;


    v0 /= v0[3];
    v1 /= v1[3];
    v2 /= v2[3];


    int sx0 = (int)((v0[0] + 1.0f) * 0.5f * width);
    int sy0 = (int)((1.0f - (v0[1] + 1.0f) * 0.5f) * height);

    int sx1 = (int)((v1[0] + 1.0f) * 0.5f * width);
    int sy1 = (int)((1.0f - (v1[1] + 1.0f) * 0.5f) * height);

    int sx2 = (int)((v2[0] + 1.0f) * 0.5f * width);
    int sy2 = (int)((1.0f - (v2[1] + 1.0f) * 0.5f) * height);

    draw_line(sx0, sy0, sx1, sy1,image, c);
    draw_line(sx1, sy1, sx2, sy2,image, c);
    draw_line(sx2, sy2, sx0, sy0,image, c);
}


int main()
{
    TGAImage image(width, height, TGAImage::RGB);

    draw_triangle_2d(60, 500, 20, 150,150,100,image,red);
    draw_triangle_gouraud(10, 30, 350, 150,200,300,image,blue,red,white);

    draw_triangle(-48, -10, 82, 29, -15, 44, 13, 34,114, image, blue);

    image.write_tga_file("lines.tga");
}