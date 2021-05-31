#pragma once

#include <iostream>
#include <GL/glew.h>

namespace cagd
{
    class Color4;

    std::ostream& operator <<(std::ostream& lhs, const Color4& rhs);
    std::istream& operator >>(std::istream& lhs, Color4& rhs);

    class Color4
    {
        friend std::ostream& operator <<(std::ostream& lhs, const Color4& rhs);
        friend std::istream& operator >>(std::istream& lhs, Color4& rhs);

    protected:
        GLfloat _data[4]; // (r, g, b, a)

    public:
        Color4()
        {
            _data[0] = _data[1] = _data[2] = 0.0;
            _data[3] = 1.0;
        }

        Color4(GLfloat r, GLfloat g, GLfloat b, GLfloat a = 1.0f)
        {
            _data[0] = r;
            _data[1] = g;
            _data[2] = b;
            _data[3] = a;
        }

        // homework: get components by value
        GLfloat operator [](GLuint rhs) const;
        GLfloat r() const;
        GLfloat g() const;
        GLfloat b() const;
        GLfloat a() const;

        // homework: get components by reference
        GLfloat& operator [](GLuint rhs);
        GLfloat& r();
        GLfloat& g();
        GLfloat& b();
        GLfloat& a();
    };

    // homework: get components by value
    inline GLfloat Color4::operator[](GLuint rhs) const
    {
        return _data[rhs];
    }
    inline GLfloat Color4::r() const
    {
        return _data[0];
    }
    inline GLfloat Color4::g() const
    {
        return _data[1];
    }
    inline GLfloat Color4::b() const
    {
        return _data[2];
    }
    inline GLfloat Color4::a() const
    {
        return _data[3];
    }

    // homework: get components by reference
    inline GLfloat& Color4::operator[](GLuint rhs)
    {
        return _data[rhs];
    }
    inline GLfloat& Color4::r()
    {
        return _data[0];
    }
    inline GLfloat& Color4::g()
    {
        return _data[1];
    }
    inline GLfloat& Color4::b()
    {
        return _data[2];
    }
    inline GLfloat& Color4::a()
    {
        return _data[3];
    }

    inline std::ostream& operator <<(std::ostream& lhs, const Color4& rhs)
    {
        return lhs << rhs._data[0]
                << " " << rhs._data[1]
                << " " << rhs._data[2]
                << " " << rhs._data[3];
    }
    inline std::istream& operator >>(std::istream& lhs, Color4& rhs)
    {
        return lhs >> rhs._data[0]
                >> rhs._data[1]
                >> rhs._data[2]
                >> rhs._data[3];
    }

    inline Color4 darkRed = Color4(0.4f,0.0f,0.0f);
    inline Color4 yellow = Color4(1.0f,1.0f,0.4f);
    inline Color4 darkGreen = Color4(0.0f,0.4f,0.0f);
    inline Color4 green = Color4(0.6f,1.0f,0.6f);
    inline Color4 blue = Color4(0.6f,1.0f,1.0f);
    inline Color4 darkBlue = Color4(0.0f,0.0f,0.4f);
    inline Color4 purple = Color4(0.8f,0.6f,1.0f);
    inline Color4 pink = Color4(1.0f,0.4f,0.7f);
    inline Color4 orange = Color4(1.0f,0.5f,0.0f);
    inline Color4 grey = Color4(0.6f,0.6f,0.6f);
}
