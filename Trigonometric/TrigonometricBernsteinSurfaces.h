#pragma once

#include "../Core/Matrices.h"
#include "../Core/RealSquareMatrices.h"
#include "../Core/TensorProductSurfaces3.h"

namespace cagd {
    class TrigonometricBernsteinSurface3: public TensorProductSurface3
    {
    protected:
        // shape parameters in directions u and v, respectively
        GLdouble                _alpha, _beta;

        // orders of basis functions in directions u and v, respectively
        GLuint                  _n, _m;

        // normalizing coefficients in directions u and v, respectively
        RowMatrix<GLdouble>     _u_c, _v_c;

        // binomial coefficients
        TriangularMatrix<GLdouble> _bc;

        // auxiliar protected methods
        GLvoid                  _CalculateBinomialCoefficients(GLuint order, TriangularMatrix<GLdouble> &bc);
        GLboolean               _CalculateNormalizingCoefficients(GLuint order, GLdouble alpha, RowMatrix<GLdouble> &c);

    public:
        // special constructor
        TrigonometricBernsteinSurface3(GLdouble alpha, GLuint n, GLdouble beta, GLuint m);

        // inherited pure virtual abstract methods that must be declared and defined
        GLboolean UBlendingFunctionValues(GLdouble u, RowMatrix<GLdouble>& values) const;
        GLboolean VBlendingFunctionValues(GLdouble v, RowMatrix<GLdouble>& values) const;
        GLboolean CalculatePartialDerivatives(GLuint maximum_order_of_partial_derivatives, GLdouble u, GLdouble v, PartialDerivatives& pd) const;

        //...
    };
}
