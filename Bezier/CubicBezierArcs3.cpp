#include "CubicBezierArcs3.h"

namespace cagd {
    CubicBezierArc3::CubicBezierArc3(GLenum data_usage_flag):
        LinearCombination3(0.0, 1.0, 4, data_usage_flag)
    {
    }

    GLboolean CubicBezierArc3::BlendingFunctionValues(GLdouble u, RowMatrix<GLdouble> &values) const
    {
        if (u < _u_min || u > _u_max)
            return GL_FALSE;

        values.ResizeColumns(4);

        GLdouble u2 = u * u, u3 = u2 * u;

        values[0] = 1 - 3 * u + 3 * u2 - u3;
        values[1] = 3 * u - 6 * u2 + 3 * u3;
        values[2] = 3 * u2 - 3 * u3;
        values[3] = u3;

        return GL_TRUE;
    }

    GLboolean CubicBezierArc3::CalculateDerivatives(GLuint max_order_of_derivatives, GLdouble u, Derivatives &d) const
    {
        if (u < _u_min || u > _u_max || max_order_of_derivatives > 2)
        {
            return GL_FALSE;
        }

        d.ResizeRows(max_order_of_derivatives + 1);
        d.LoadNullVectors();

        GLdouble u2 = u * u, u3 = u2 * u;

        d[0] += _data[0] * (1 - 3 * u + 3 * u2 - u3);
        d[0] += _data[1] * (3 * u - 6 * u2 + 3 * u3);
        d[0] += _data[2] * (3 * u2 - 3 * u3);
        d[0] += _data[3] * u3;

        if (max_order_of_derivatives > 0)
        {
            d[1] += _data[0] * (-3 + 6 * u - 3 * u2);
            d[1] += _data[1] * (3 - 12 * u + 9 * u2);
            d[1] += _data[2] * (6 * u - 9 * u2);
            d[1] += _data[3] * 3 * u2;
        }

        if (max_order_of_derivatives > 1)
        {
            d[2] += _data[0] * (6 - 6 * u);
            d[2] += _data[1] * (-12 + 18 * u);
            d[2] += _data[2] * (6 - 18 * u);
            d[2] += _data[3] * 6 * u;
        }

        return GL_TRUE;
    }

    GLboolean CubicBezierArc3::GetData(GLuint index, DCoordinate3 &data) const
    {
        data = _data[index];

        return GL_TRUE;
    }

}
