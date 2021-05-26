#pragma once

#include "../Core/LinearCombination3.h"

namespace cagd {

    class CubicBezierArc3: public LinearCombination3 {
    public:
        // special constructor
        CubicBezierArc3(GLenum data_usage_flag = GL_STATIC_DRAW);

        // redeclaration and define inherited pure virtual methods
        GLboolean BlendingFunctionValues(GLdouble u, RowMatrix<GLdouble>& values) const;
        GLboolean CalculateDerivatives(GLuint max_order_of_derivatives, GLdouble u, Derivatives &d) const;
    };

}
