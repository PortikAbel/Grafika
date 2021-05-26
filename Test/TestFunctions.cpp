#include <cmath>
#include "TestFunctions.h"
#include "../Core/Constants.h"

using namespace cagd;
using namespace std;

GLdouble spiral_on_cone::u_min = -TWO_PI;
GLdouble spiral_on_cone::u_max = +TWO_PI;

DCoordinate3 spiral_on_cone::d0(GLdouble u)
{
    return DCoordinate3(u * cos(u),
                        u * sin(u),
                        u);
}

DCoordinate3 spiral_on_cone::d1(GLdouble u)
{
    GLdouble c = cos(u), s = sin(u);
    return DCoordinate3(c - u * s,
                        s + u * c,
                        1.0);
}

DCoordinate3 spiral_on_cone::d2(GLdouble u)
{
    GLdouble c = cos(u), s = sin(u);
    return DCoordinate3(-2.0 * s - u * c,
                        2.0 * c - u * s,
                        0.0);
}

GLdouble torus_knot::u_min = 0.0;
GLdouble torus_knot::u_max = 3 * TWO_PI;

DCoordinate3 torus_knot::d0(GLdouble u)
{
    GLdouble c = cos(u), s = sin(u), c23 = cos(2.0/3.0 * u);
    return DCoordinate3( (c23 + 2.0) * c,
                         (c23 + 2.0) * s,
                         sin(2.0/3.0 * u)
                       );
}

DCoordinate3 torus_knot::d1(GLdouble u)
{
    GLdouble c = cos(u), s = sin(u), c23 = cos(2.0/3.0 * u), s23 = sin(2.0/3.0 * u);
    return DCoordinate3(-(c23 + 2.0) * s - 2.0/3.0 * s23 * c,
                         (c23 + 2.0) * c - 2.0 / 3.0 * s23 * s,
                         2.0/3.0 * c23
                       );
}

DCoordinate3 torus_knot::d2(GLdouble u)
{
    GLdouble c = cos(u), s = sin(u), c23 = cos(2.0/3.0 * u), s23 = sin(2.0/3.0 * u);
    return DCoordinate3( (12.0 * s23 * s - (13.0 * c23 + 18.0) * c) / 9.0,
                         ((13.0 * c23 + 18.0) * s + 12.0 * s23 * c) / -9.0,
                         -4.0/9.0 * s23
                       );
}

GLdouble spiral_on_sphere::u_min = -PI;
GLdouble spiral_on_sphere::u_max = PI;

DCoordinate3 spiral_on_sphere::d0(GLdouble u)
{
    return DCoordinate3( sin(u) * cos(2.0*u),
                         sin(u) * sin(2.0*u),
                         cos(u));
}

DCoordinate3 spiral_on_sphere::d1(GLdouble u)
{
    return DCoordinate3( cos(u)*cos(2.0*u) - 2.0*sin(u)*sin(2.0*u),
                         cos(u)*sin(2.0*u) + 2.0*sin(u)*cos(2.0*u),
                         -sin(u));
}

DCoordinate3 spiral_on_sphere::d2(GLdouble u)
{
    return DCoordinate3( -4.0*cos(u)*sin(2.0*u) - 5*sin(u)*cos(2*u),
                         4.0*cos(u)*cos(2.0*u) - 5*sin(u)*sin(2*u),
                         -cos(u));
}

GLdouble viviani::u_min = -PI;
GLdouble viviani::u_max = PI;

DCoordinate3 viviani::d0(GLdouble u)
{
    GLdouble s = sin(u), c = cos(u);
    return DCoordinate3( s * c,
                         s*s,
                         c);
}

DCoordinate3 viviani::d1(GLdouble u)
{
    GLdouble s = sin(u), c = cos(u);
    return DCoordinate3( c*c - s*s,
                         2.0 * s * c,
                         -s);
}

DCoordinate3 viviani::d2(GLdouble u)
{
    GLdouble s = sin(u), c = cos(u);
    return DCoordinate3( -4.0 * c * s,
                         2.0 * (c*c - s*s),
                         -c);
}

GLdouble hypocycloid::u_min = -PI;
GLdouble hypocycloid::u_max = PI;

DCoordinate3 hypocycloid::d0(GLdouble u)
{
    return DCoordinate3( 8.0 * cos(u) + cos(8.0 * u),
                         8.0 * sin(u) + sin(8.0 * u)
                       );
}

DCoordinate3 hypocycloid::d1(GLdouble u)
{
    return DCoordinate3( -8.0 * (sin(u) + sin(8.0 * u)),
                         8.0 * (cos(u) + cos(8.0 * u))
                       );
}

DCoordinate3 hypocycloid::d2(GLdouble u)
{
    return DCoordinate3( -8.0 * (cos(u) + 8.0 * cos(8.0 * u)),
                         -8.0 * (sin(u) + 8.0 * sin(8.0 * u))
                       );
}

GLdouble epitrochoid::u_min = -PI;
GLdouble epitrochoid::u_max = PI;

DCoordinate3 epitrochoid::d0(GLdouble u)
{
    return DCoordinate3( 4.0 * cos(u) - 0.5 * cos(4.0 * u),
                         4.0 * sin(u) - 0.5 * sin(4.0 * u)
                       );
}

DCoordinate3 epitrochoid::d1(GLdouble u)
{
    return DCoordinate3( -4.0 * (sin(u) - 0.5 * sin(4.0 * u)),
                         4.0 * (cos(u) - 0.5 * cos(4.0 * u))
                       );
}

DCoordinate3 epitrochoid::d2(GLdouble u)
{
    return DCoordinate3( 4.0 * (2.0 * cos(4.0 * u) - cos(u)),
                         4.0 * (2.0 * sin(4.0 * u) - sin(u))
                       );
}


GLdouble torus::R = 1.0;
GLdouble torus::r = 0.5;

GLdouble torus::u_min = 0.0;
GLdouble torus::u_max = TWO_PI;
GLdouble torus::v_min = 0.0;
GLdouble torus::v_max = TWO_PI;

DCoordinate3 torus::d00(GLdouble u, GLdouble v)
{
    return DCoordinate3((R + r * cos(v)) * cos(u),
                        (R + r * cos(v)) * sin(u),
                        r * sin(v)
                        );
}

DCoordinate3 torus::d10(GLdouble u, GLdouble v)
{
    return DCoordinate3(-(R + r * cos(v)) * sin(u),
                        (R + r * cos(v)) * cos(u),
                        0.0
                        );
}

DCoordinate3 torus::d01(GLdouble u, GLdouble v)
{
    return DCoordinate3(-r * sin(v) * cos(u),
                        -r * sin(v) * sin(u),
                        r * cos(v)
                        );
}

GLdouble dupin_cyclide::a = 1.0;
GLdouble dupin_cyclide::b = 0.98;
GLdouble dupin_cyclide::c = 0.199;
GLdouble dupin_cyclide::d = 0.3;

GLdouble dupin_cyclide::u_min = 0.0;
GLdouble dupin_cyclide::u_max = TWO_PI;
GLdouble dupin_cyclide::v_min = 0.0;
GLdouble dupin_cyclide::v_max = TWO_PI;

DCoordinate3 dupin_cyclide::d00(GLdouble u, GLdouble v)
{
    GLdouble cu = cos(u), cv = cos(v), denom = (a - c * cu * cv);
    return DCoordinate3((d * (c - a * cu * cv) + b*b * cu) / denom,
                        b * sin(u) * (a - d * cv) / denom,
                        b * sin(v) * (c * cu - d) / denom
                        );
}

DCoordinate3 dupin_cyclide::d10(GLdouble u, GLdouble v)
{
    GLdouble cu = cos(u), cv = cos(v), su = sin(u), sv = sin(v), denom = (a - c * cu * cv);
    denom *= denom;
    return DCoordinate3(- ((c*c - a*a) * d * cv + a * b*b) * su / denom,
                        b * (d * cv - a) * (c * cv * su*su + c * cv * cu*cu - a * cu) / denom,
                        b * c * (d * cv - a) * sv * su / denom
                        );
}

DCoordinate3 dupin_cyclide::d01(GLdouble u, GLdouble v)
{
    GLdouble cu = cos(u), cv = cos(v), su = sin(u), sv = sin(v), denom = (a - c * cu * cv);
    denom *= denom;
    return DCoordinate3(- cu * (b*b * c * cu + (c*c - a*a) * d) * sv / denom,
                        - a * b * (c * cu - d) * su * sv / denom,
                        - b * (c * cu - d) * (c * cu * sv*sv + c * cu * cv*cv - a * cv) / denom
                        );
}

GLdouble sphere::R = 2.0;

GLdouble sphere::u_min = 0.0;
GLdouble sphere::u_max = TWO_PI;
GLdouble sphere::v_min = EPS;
GLdouble sphere::v_max = PI - EPS;

DCoordinate3 sphere::d00(GLdouble u, GLdouble v)
{
    return DCoordinate3(R * cos(u) * sin(v),
                        R * sin(u) * sin(v),
                        R * cos(v)
                        );
}

DCoordinate3 sphere::d01(GLdouble u, GLdouble v)
{
    return DCoordinate3(- R * sin(u) * sin(v),
                        R * cos(u) * sin(v),
                        0.0
                        );
}

DCoordinate3 sphere::d10(GLdouble u, GLdouble v)
{
    return DCoordinate3(R * cos(u) * cos(v),
                        R * sin(u) * cos(v),
                        - R * sin(v)
                        );
}

GLdouble cylinder::R = 1.8;

GLdouble cylinder::u_min = 0.0;
GLdouble cylinder::u_max = TWO_PI;
GLdouble cylinder::v_min = -2.0;
GLdouble cylinder::v_max = 2.0;

DCoordinate3 cylinder::d00(GLdouble u, GLdouble v)
{
    return DCoordinate3(R * sin(u), v, R * cos(u));
}

DCoordinate3 cylinder::d10(GLdouble u, GLdouble v)
{
    return DCoordinate3(R * cos(u), 0.0, -R * sin(u));
}

DCoordinate3 cylinder::d01(GLdouble u, GLdouble v)
{
    return DCoordinate3(0.0, 1.0, 0.0);
}

GLdouble cone::R = 0.5;

GLdouble cone::u_min = 0.0;
GLdouble cone::u_max = TWO_PI;
GLdouble cone::v_min = -2.0;
GLdouble cone::v_max = 2.0;

DCoordinate3 cone::d00(GLdouble u, GLdouble v)
{
    return DCoordinate3(v, R * v * sin(u), R * v * cos(u));
}

DCoordinate3 cone::d10(GLdouble u, GLdouble v)
{
    return DCoordinate3(0.0, R * v * cos(u), -R * v * sin(u));
}

DCoordinate3 cone::d01(GLdouble u, GLdouble v)
{
    return DCoordinate3(1.0, R * sin(u), R * cos(u));
}


GLdouble seashell::c = 5.0/4.0;

GLdouble seashell::u_min = 0.0;
GLdouble seashell::u_max = TWO_PI;
GLdouble seashell::v_min = -TWO_PI;
GLdouble seashell::v_max = TWO_PI;

DCoordinate3 seashell::d00(GLdouble u, GLdouble v)
{
    return DCoordinate3(c * (1.0 - v / TWO_PI) * sin(2.0*v) * (1 + cos(u)) + sin(2.0*v),
                        c * (1.0 - v / TWO_PI) * cos(2.0*v) * (1 + cos(u)) + cos(2.0*v),
                        c * v + c * (1.0 - v / TWO_PI) * sin(u)
                        );
}

DCoordinate3 seashell::d10(GLdouble u, GLdouble v)
{
    return DCoordinate3(- c * (1.0 - v / TWO_PI) * sin(2.0*v) * sin(u),
                        - c * (1.0 - v / TWO_PI) * cos(2.0*v) * sin(u),
                        c * (1.0 - v / TWO_PI) * cos(u)
                        );
}

DCoordinate3 seashell::d01(GLdouble u, GLdouble v)
{
    return DCoordinate3(c * (1 + cos(u)) * (sin(2.0*v) / TWO_PI + (1.0 - v / TWO_PI) * 2.0 * cos(2.0*v)) + 2.0 * cos(2.0*v),
                        c * (1 + cos(u)) * (cos(2.0*v) / TWO_PI - (1.0 - v / TWO_PI) * 2.0 * sin(2.0*v)) - 2.0 * sin(2.0*v),
                        (1.0 - c * sin(u)) / TWO_PI
                        );
}
