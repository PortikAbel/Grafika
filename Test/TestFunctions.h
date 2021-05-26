#pragma once

#include "../Core/DCoordinates3.h"

namespace cagd
{
    namespace spiral_on_cone
    {
        extern GLdouble u_min, u_max;

        DCoordinate3 d0(GLdouble);
        DCoordinate3 d1(GLdouble);
        DCoordinate3 d2(GLdouble);
    }

    namespace torus_knot {
        extern GLdouble u_min, u_max;

        DCoordinate3 d0(GLdouble);
        DCoordinate3 d1(GLdouble);
        DCoordinate3 d2(GLdouble);
    }

    namespace spiral_on_sphere {
        extern GLdouble u_min, u_max;

        DCoordinate3 d0(GLdouble);
        DCoordinate3 d1(GLdouble);
        DCoordinate3 d2(GLdouble);
    }

    namespace viviani {
        extern GLdouble u_min, u_max;

        DCoordinate3 d0(GLdouble);
        DCoordinate3 d1(GLdouble);
        DCoordinate3 d2(GLdouble);
    }

    namespace hypocycloid {
        extern GLdouble u_min, u_max;

        DCoordinate3 d0(GLdouble);
        DCoordinate3 d1(GLdouble);
        DCoordinate3 d2(GLdouble);
    }

    namespace epitrochoid {
        extern GLdouble u_min, u_max;

        DCoordinate3 d0(GLdouble);
        DCoordinate3 d1(GLdouble);
        DCoordinate3 d2(GLdouble);
    }

    namespace torus {
        extern GLdouble R, r;

        extern GLdouble u_min, u_max, v_min, v_max;

        DCoordinate3 d00(GLdouble, GLdouble);
        DCoordinate3 d10(GLdouble, GLdouble);
        DCoordinate3 d01(GLdouble, GLdouble);
    }

    namespace dupin_cyclide {
        extern GLdouble a, b, c, d;

        extern GLdouble u_min, u_max, v_min, v_max;

        DCoordinate3 d00(GLdouble, GLdouble);
        DCoordinate3 d10(GLdouble, GLdouble);
        DCoordinate3 d01(GLdouble, GLdouble);
    }

    namespace sphere {
        extern GLdouble R;

        extern GLdouble u_min, u_max, v_min, v_max;

        DCoordinate3 d00(GLdouble, GLdouble);
        DCoordinate3 d10(GLdouble, GLdouble);
        DCoordinate3 d01(GLdouble, GLdouble);
    }

    namespace cylinder {
        extern GLdouble R;

        extern GLdouble u_min, u_max, v_min, v_max;

        DCoordinate3 d00(GLdouble, GLdouble);
        DCoordinate3 d10(GLdouble, GLdouble);
        DCoordinate3 d01(GLdouble, GLdouble);
    }

    namespace cone {
        extern GLdouble R;

        extern GLdouble u_min, u_max, v_min, v_max;

        DCoordinate3 d00(GLdouble, GLdouble);
        DCoordinate3 d10(GLdouble, GLdouble);
        DCoordinate3 d01(GLdouble, GLdouble);
    }

    namespace seashell {
        extern GLdouble c;

        extern GLdouble u_min, u_max, v_min, v_max;

        DCoordinate3 d00(GLdouble, GLdouble);
        DCoordinate3 d10(GLdouble, GLdouble);
        DCoordinate3 d01(GLdouble, GLdouble);
    }
}
