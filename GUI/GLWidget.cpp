#include "GLWidget.h"

#if !defined(__APPLE__)
#include <GL/glu.h>
#endif

#include <iostream>
#include <string>

using namespace std;

#include <Core/Exceptions.h>
#include "../Parametric/ParametricSurfaces3.h"
#include "../Test/TestFunctions.h"
#include "../Core/Matrices.h"
#include "../Core/Materials.h"
#include "../Core/Constants.h"

namespace cagd
{
    //------------------
    // directional light
    //------------------

    bool GLWidget::_createDl()
    {
        // creating a white directional light source
        HCoordinate3    direction   (0.0, 0.0, 1.0, 0.0);
        Color4          ambient     (0.4f, 0.4f, 0.4f, 1.0f);
        Color4          diffuse     (0.8f, 0.8f, 0.8f, 1.0f);
        Color4          specular    (1.0, 1.0, 1.0, 1.0);

        _dl = new (nothrow) DirectionalLight(GL_LIGHT0, direction, ambient, diffuse, specular);

        if (!_dl)
        {
            return false;
        }

        return true;
    }

    void GLWidget::_destroyDl()
    {
        if (_dl)
        {
            delete _dl;
            _dl = nullptr;
        }
    }

    bool GLWidget::_createPl()
    {
        // creating a white directional light source
        HCoordinate3    direction   (0.0, 0.0, 1.0, 1.0);
        Color4          ambient     (0.4f, 0.4f, 0.4f, 1.0f);
        Color4          diffuse     (0.8f, 0.8f, 0.8f, 1.0f);
        Color4          specular    (1.0, 1.0, 1.0, 1.0);
        GLfloat         constant_attenuation(1.0);
        GLfloat         linear_attenuation(0.0);
        GLfloat         quadratic_attenuation(0.0);
        HCoordinate3    spot_direction(0.0, 0.0, -1.0);

        _pl = new (nothrow) PointLight(GL_LIGHT1, direction, ambient, diffuse, specular, constant_attenuation, linear_attenuation, quadratic_attenuation);

        if (!_pl)
        {
            return false;
        }

        return true;
    }

    void GLWidget::_destroyPl()
    {
        if (_pl)
        {
            delete _pl;
            _pl = nullptr;
        }
    }

    bool GLWidget::_createSl()
    {
        HCoordinate3    direction   (0.0f, 0.0f, 1.0f, 0.1f);
        Color4          ambient     (0.4f, 0.4f, 0.4f, 1.0f);
        Color4          diffuse     (0.8f, 0.8f, 0.8f, 1.0f);
        Color4          specular    (1.0, 1.0, 1.0, 1.0);
        GLfloat         constant_attenuation(0.1f);
        GLfloat         linear_attenuation(0.1f);
        GLfloat         quadratic_attenuation(0.1f);
        HCoordinate3    spot_direction(0.0f, 0.0f, -1.0f, 1.0f);
        GLfloat         spot_cutoff(0.2f);
        GLfloat         spot_exponent(0.2f);

        _sl = new (nothrow) Spotlight(GL_LIGHT2, direction, ambient, diffuse, specular, constant_attenuation, linear_attenuation, quadratic_attenuation, spot_direction, spot_cutoff, spot_exponent);

        if (!_sl)
        {
            return false;
        }

        return true;
    }

    void GLWidget::_destroySl()
    {
        if (_sl)
        {
            delete _sl;
            _sl = nullptr;
        }
    }

    //---------
    // textures
    //---------

    bool GLWidget::_loadTextures()
    {
        _texture = new QOpenGLTexture(QImage("Textures/periodic_texture_05.jpg"));
        _texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
        _texture->setMagnificationFilter(QOpenGLTexture::Linear);

        return true;
    }

    void GLWidget::_destroyTextures()
    {
        if (_texture)
        {
            delete _texture;
            _texture = nullptr;
        }
    }

    //--------
    // shaders
    //--------

    bool GLWidget::_createShaders()
    {
        _shaders.ResizeColumns(4);

        RowMatrix<string> vertex_shaders, fragment_shaders;

        vertex_shaders.ResizeColumns(4);
        fragment_shaders.ResizeColumns(4);

        vertex_shaders[0] = "Shaders/directional_light.vert";
        vertex_shaders[1] = "Shaders/reflection_lines.vert";
        vertex_shaders[2] = "Shaders/toon.vert";
        vertex_shaders[3] = "Shaders/two_sided_lighting.vert";

        fragment_shaders[0] = "Shaders/directional_light.frag";
        fragment_shaders[1] = "Shaders/reflection_lines.frag";
        fragment_shaders[2] = "Shaders/toon.frag";
        fragment_shaders[3] = "Shaders/two_sided_lighting.frag";

        for (GLuint i = 0; i < 4; i++)
        {
            _shaders[i] = new (nothrow) ShaderProgram();

            if (!_shaders[i])
            {
                return false;
            }

            try {
                if (!_shaders[i]->InstallShaders(vertex_shaders[i], fragment_shaders[i], GL_TRUE))
                {
                    return false;
                }
            }
            catch (Exception &e)
            {
                cerr << e << endl;
                return false;
            }

            _shader_selected.push_back(false);
        }

        return true;
    }

    bool GLWidget::_communicateWithShaders()
    {
        _shaders[1]->Enable();
        if (!_shaders[1]->SetUniformVariable1f("scale_factor", _rl_scale))
        {
            _shaders[1]->Disable();
            return false;
        }
        if (!_shaders[1]->SetUniformVariable1f("smoothing", _rl_smoothing))
        {
            _shaders[1]->Disable();
            return false;
        }
        if (!_shaders[1]->SetUniformVariable1f("shading", _rl_shading))
        {
            _shaders[1]->Disable();
            return false;
        }
        _shaders[1]->Disable();

        _shaders[2]->Enable();
        if (!_shaders[2]->SetUniformVariable4fv("default_outline_color", 1, &_toon_def_outline[0]))
        {
            _shaders[2]->Disable();
            return false;
        }
        _shaders[2]->Disable();

        return true;
    }

    void GLWidget::_destroyShaders()
    {
        for (GLuint i = 0; i < 4; i++)
        {
            if (_shaders[i])
            {
                delete _shaders[i];
                _shaders[i] = nullptr;
            }
        }
    }

    //-----------------------
    // draw a simple triangle
    //-----------------------

    void GLWidget::_privateTestFunction()
    {
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_LINES);
            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(1.1f, 0.0f, 0.0f);

            glColor3f(0.0, 1.0, 0.0);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(0.0f, 1.1f, 0.0f);

            glColor3f(0.0f, 0.0f, 1.0f);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(0.0f, 0.0f, 1.1f);
        glEnd();

        glBegin(GL_TRIANGLES);
            // attributes
            glColor3f(1.0f, 0.0f, 0.0f);
            // associated with position
            glVertex3f(1.0f, 0.0f, 0.0f);

            // attributes
            glColor3f(0.0, 1.0, 0.0);
            // associated with position
            glVertex3f(0.0, 1.0, 0.0);

            // attributes
            glColor3f(0.0f, 0.0f, 1.0f);
            // associated with position
            glVertex3f(0.0f, 0.0f, 1.0f);
        glEnd();
    }

    //----------------------
    // methods of playground
    //----------------------

    bool GLWidget::_loadAllModelsFromOff()
    {
        return _space_station.LoadFromOFF("Models/space_station.off", GL_TRUE)
                && _star.LoadFromOFF("Models/star.off", GL_TRUE)
                && _sphere.LoadFromOFF("Models/sphere.off", GL_TRUE);
    }

    bool GLWidget::_updateAllModels()
    {
        return _space_station.UpdateVertexBufferObjects(GL_DYNAMIC_DRAW)
                && _star.UpdateVertexBufferObjects(GL_DYNAMIC_DRAW)
                && _sphere.UpdateVertexBufferObjects(GL_DYNAMIC_DRAW);
    }

    bool GLWidget::_renderPlayground()
    {
        if (!_dl)
        {
            return false;
        }

        bool shader_selected = false;
        for (GLuint i = 0; i < 4; i++)
        {
            if (_shader_selected[i])
                _shaders[i]->Enable();
            shader_selected |= _shader_selected[i];
        }
        if (!shader_selected)
        {
            glEnable(GL_LIGHTING);
            glEnable(GL_NORMALIZE);
            _dl->Enable();
        }

        MatFBBrass.Apply();
        _star.Render();

        for (uint i = 1; i <= 15; i++)
        {
            glPushMatrix();

            switch (i % 3) {
            case 0:
                MatFBEmerald.Apply();
                break;
            case 1:
                MatFBTurquoise.Apply();
                break;
            case 2:
                MatFBRuby.Apply();
                break;
            }

            glRotated((i*27.0 - _angle) * (i%4 + 1), 0.0, 0.0, 1.0);

            glTranslated (1.0 + i * 0.3, 0.0, 0.0);

            glScaled(0.3, 0.3, 0.3);

            _sphere.Render();

            glRotated(180.0-_angle, 1.0, 0.0, 0.0);
            glTranslated(0.0, 1.0, 0.0);
            glRotated(-_angle, 0.0, 0.0, 1.0);

            if (i % 5 % 2 == 0)
            {
                glScaled(0.7, 0.7, 0.7);
                MatFBSilver.Apply();
                _space_station.Render();
            }

            glPopMatrix();
        }

        if (shader_selected)
        {
            for (GLuint i = 0; i < 4; i++)
            {
                if (_shader_selected[i])
                    _shaders[i]->Disable();
            }
        }
        else
        {
            glDisable(GL_LIGHTING);
            glDisable(GL_NORMALIZE);
            _dl->Disable();
        }

        return true;
    }

    //-----------------------------
    // methods of parametric curves
    //-----------------------------

    bool GLWidget::_createAllParametricCurvesAndTheirImages()
    {
        _pc.ResizeColumns(_pc_count);

        // create and store your geometry in display lists or vertex buffer objects
        // ...
        RowMatrix<ParametricCurve3::Derivative> derivative(3);

        derivative[0] = spiral_on_cone::d0;
        derivative[1] = spiral_on_cone::d1;
        derivative[2] = spiral_on_cone::d2;

        _pc[0] = nullptr;
        _pc[0] = new (nothrow) ParametricCurve3(derivative, spiral_on_cone::u_min, spiral_on_cone::u_max);

        if (!_pc[0])
        {
            throw Exception("Could not generate the spiral on cone parametric curve!");
        }

        derivative[0] = torus_knot::d0;
        derivative[1] = torus_knot::d1;
        derivative[2] = torus_knot::d2;

        _pc[1] = nullptr;
        _pc[1] = new (nothrow) ParametricCurve3(derivative, torus_knot::u_min, torus_knot::u_max);

        if (!_pc[1])
        {
            throw Exception("Could not generate the torus knot parametric curve!");
        }

        derivative[0] = spiral_on_sphere::d0;
        derivative[1] = spiral_on_sphere::d1;
        derivative[2] = spiral_on_sphere::d2;

        _pc[2] = nullptr;
        _pc[2] = new (nothrow) ParametricCurve3(derivative, spiral_on_sphere::u_min, spiral_on_sphere::u_max);

        if (!_pc[2])
        {
            throw Exception("Could not generate the spiral on sphere parametric curve!");
        }

        derivative[0] = viviani::d0;
        derivative[1] = viviani::d1;
        derivative[2] = viviani::d2;

        _pc[3] = nullptr;
        _pc[3] = new (nothrow) ParametricCurve3(derivative, viviani::u_min, viviani::u_max);

        if (!_pc[3])
        {
            throw Exception("Could not generate the viviani parametric curve!");
        }

        derivative[0] = hypocycloid::d0;
        derivative[1] = hypocycloid::d1;
        derivative[2] = hypocycloid::d2;

        _pc[4] = nullptr;
        _pc[4] = new (nothrow) ParametricCurve3(derivative, hypocycloid::u_min, hypocycloid::u_max);

        if (!_pc[4])
        {
            throw Exception("Could not generate the hypocycloid parametric curve!");
        }

        derivative[0] = epitrochoid::d0;
        derivative[1] = epitrochoid::d1;
        derivative[2] = epitrochoid::d2;

        _pc[5] = nullptr;
        _pc[5] = new (nothrow) ParametricCurve3(derivative, epitrochoid::u_min, epitrochoid::u_max);

        if (!_pc[5])
        {
            throw Exception("Could not generate the epitrochoid parametric curve!");
        }

        _image_of_pc.ResizeColumns(_pc_count);

        for (GLuint i = 0; i < _pc_count; i++)
        {
            _image_of_pc[i] = _pc[i]->GenerateImage(_div_point_count, _usage_flag_pc);

            if (!_image_of_pc[i] || !_image_of_pc[i]->UpdateVertexBufferObjects(_scale, _usage_flag_pc))
            {
                _destroyAllExistingParametricCurvesAndTheirImages();
                return false;
            }
        }
        return true;
    }

    void GLWidget::_destroyAllExistingParametricCurvesAndTheirImages()
    {
        for (GLuint i = 0; i < _pc_count; i++)
        {
            if (_pc[i])
            {
                delete _pc[i];
                _pc[i] = nullptr;
            }
            if (_image_of_pc[i])
            {
                delete _image_of_pc[i];
                _image_of_pc[i] = nullptr;
            }
        }
    }

    bool GLWidget::_createSelectedParametricCurveImage()
    {
        if (_image_of_pc[_selected_pc])
        {
            delete _image_of_pc[_selected_pc];
        }

        _image_of_pc[_selected_pc] = _pc[_selected_pc]->GenerateImage(_div_point_count, _usage_flag_pc);

        if (!_image_of_pc[_selected_pc] || !_image_of_pc[_selected_pc]->UpdateVertexBufferObjects(_scale, _usage_flag_pc))
        {
            return false;
        }

        return true;
    }

    bool GLWidget::_renderSelectedParametricCurve()
    {
        if (!_image_of_pc[_selected_pc])
        {
            return false;
        }

        glColor3f(1.0f, 0.0f, 0.0f);
        _image_of_pc[_selected_pc]->RenderDerivatives(0, GL_LINE_STRIP);

        if (_show_tangents)
        {
            glPointSize(5.0f);

                glColor3f(0.0f, 0.5f, 0.0f);
                _image_of_pc[_selected_pc]->RenderDerivatives(1, GL_LINES);
                _image_of_pc[_selected_pc]->RenderDerivatives(1, GL_POINTS);

            glPointSize(1.0f);
        }

        if (_show_acceleration_vectors)
        {
            glPointSize(5.0f);

                glColor3f(0.1f, 0.5f, 0.9f);
                _image_of_pc[_selected_pc]->RenderDerivatives(2, GL_LINES);
                _image_of_pc[_selected_pc]->RenderDerivatives(2, GL_POINTS);

            glPointSize(1.0f);
        }

        return true;
    }

    //-------------------------------
    // methods of parametric surfaces
    //-------------------------------

    bool GLWidget::_createAllParametricSurfacesAndTheirImages()
    {
        _ps.ResizeColumns(_ps_count);

        TriangularMatrix<ParametricSurface3::PartialDerivative> derivative(2);

        derivative(0, 0) = torus::d00;
        derivative(1, 0) = torus::d10;
        derivative(1, 1) = torus::d01;

        _ps[0] = new (nothrow) ParametricSurface3(
                    derivative,
                    torus::u_min, torus::u_max,
                    torus::v_min, torus::v_max
                    );

        if (!_ps[0])
        {
            throw Exception("Could not generate torus parametric surface");
        }

        derivative(0, 0) = dupin_cyclide::d00;
        derivative(1, 0) = dupin_cyclide::d10;
        derivative(1, 1) = dupin_cyclide::d01;

        _ps[1] = new (nothrow) ParametricSurface3(
                    derivative,
                    dupin_cyclide::u_min, dupin_cyclide::u_max,
                    dupin_cyclide::v_min, dupin_cyclide::v_max
                    );

        if (!_ps[1])
        {
            throw Exception("Could not generate torus parametric surface");
        }

        derivative(0, 0) = sphere::d00;
        derivative(1, 0) = sphere::d10;
        derivative(1, 1) = sphere::d01;

        _ps[2] = new (nothrow) ParametricSurface3(
                    derivative,
                    sphere::u_min, sphere::u_max,
                    sphere::v_min, sphere::v_max
                    );

        if (!_ps[2])
        {
            throw Exception("Could not generate torus parametric surface");
        }

        derivative(0, 0) = cylinder::d00;
        derivative(1, 0) = cylinder::d10;
        derivative(1, 1) = cylinder::d01;

        _ps[3] = new (nothrow) ParametricSurface3(
                    derivative,
                    cylinder::u_min, cylinder::u_max,
                    cylinder::v_min, cylinder::v_max
                    );

        if (!_ps[3])
        {
            throw Exception("Could not generate torus parametric surface");
        }

        derivative(0, 0) = cone::d00;
        derivative(1, 0) = cone::d10;
        derivative(1, 1) = cone::d01;

        _ps[4] = new (nothrow) ParametricSurface3(
                    derivative,
                    cone::u_min, cone::u_max,
                    cone::v_min, cone::v_max
                    );

        if (!_ps[4])
        {
            throw Exception("Could not generate torus parametric surface");
        }

        derivative(0, 0) = seashell::d00;
        derivative(1, 0) = seashell::d10;
        derivative(1, 1) = seashell::d01;

        _ps[5] = new (nothrow) ParametricSurface3(
                    derivative,
                    seashell::u_min, seashell::u_max,
                    seashell::v_min, seashell::v_max
                    );

        if (!_ps[5])
        {
            throw Exception("Could not generate torus parametric surface");
        }

        _image_of_ps.ResizeColumns(_ps_count);

        for (GLuint i = 0; i < _ps_count; i++)
        {
            _image_of_ps[i] = _ps[i]->GenerateImage(_u_div_point_count, _v_div_point_count, _usage_flag_ps);

            if (!_image_of_ps[i] || !_image_of_ps[i]->UpdateVertexBufferObjects(_usage_flag_ps))
            {
                _destroyAllExistingParametricSurfacesAndTheirImages();
                return false;
            }
        }

        return true;
    }

    void GLWidget::_destroyAllExistingParametricSurfacesAndTheirImages()
    {
        for (GLuint i = 0; i < _ps_count; i++)
        {
            if (_ps[i])
            {
                delete _ps[i];
                _ps[i] = nullptr;
            }
            if (_image_of_ps[i])
            {
                delete _image_of_ps[i];
                _image_of_ps[i] = nullptr;
            }
        }
    }

    bool GLWidget::_createSelectedParametricSurfaceImage()
    {
        if (_image_of_ps[_selected_ps])
        {
            delete _image_of_ps[_selected_ps];
        }

        _image_of_ps[_selected_ps] = _ps[_selected_ps]->GenerateImage(_u_div_point_count, _v_div_point_count, _usage_flag_ps);

        if (!_image_of_ps[_selected_ps])
        {
            return false;
        }

        if (!_image_of_ps[_selected_ps]->UpdateVertexBufferObjects(_usage_flag_ps))
        {
            return false;
        }

        return true;
    }

    bool GLWidget::_renderSelectedParametricSurface()
    {
        if (!_dl || !_image_of_ps[_selected_ps])
        {
            return false;
        }

        bool ret_val;
        if (_show_texture)
        {
            glEnable(GL_TEXTURE_2D);
            _texture->bind();
            ret_val = _image_of_ps[_selected_ps]->Render();
            _texture->release();
            glDisable(GL_TEXTURE_2D);
        }
        else
        {
            glEnable(GL_LIGHTING);
            glEnable(GL_NORMALIZE);
            _dl->Enable();
            MatFBTurquoise.Apply();
            ret_val = _image_of_ps[_selected_ps]->Render();
            _dl->Disable();
            glDisable(GL_NORMALIZE);
            glDisable(GL_LIGHTING);
        }

        return ret_val;
    }

    //-------------------------
    // methods of cyclic curves
    //-------------------------

    bool GLWidget::_createCyclicCurvesAndTheirImages()
    {
        GLuint control_point_count = 3;
        _cc = new (nothrow) CyclicCurve3(control_point_count, GL_STATIC_DRAW);
        _ip_cc = new (nothrow) CyclicCurve3(control_point_count, GL_STATIC_DRAW);

        if(!_cc || !_ip_cc)
        {
            return false;
        }

        GLdouble    u_step = TWO_PI / (2 * control_point_count + 1),
                    u = 0.0;

        _knot_vector.ResizeRows(2 * control_point_count + 1);
        _data_points.ResizeRows(2 * control_point_count + 1);

        for (GLuint i = 0; i <= 2 * control_point_count; i++)
        {
            _knot_vector[i] = u;
            DCoordinate3& data_vector = (*_cc)[i];
            data_vector[0] = sin(u);
            data_vector[1] = cos(u);
            _data_points[i] = data_vector;
            u += u_step;
        }

        emit cc_control_point_x_changed(_data_points[_selected_cp].x());
        emit cc_control_point_y_changed(_data_points[_selected_cp].y());
        emit cc_control_point_z_changed(_data_points[_selected_cp].z());

        return _updateCyclicCurveImage() && _updateInterpolatingCyclicCurveImage();
    }

    bool GLWidget::_updateCyclicCurveImage()
    {
        // updating VBO
        if (!_cc->UpdateVertexBufferObjectsOfData())
        {
            _destroyCyclicCurvesAndTheirImages();
            return false;
        }

        // generating image
        if (_image_of_cc)
        {
            delete _image_of_cc;
        }

        _image_of_cc = _cc->GenerateImage(_max_order_of_derivatives, _cc_div_point_count, GL_STATIC_DRAW);

        if (!_image_of_cc || !_image_of_cc->UpdateVertexBufferObjects())
        {
            return false;
        }
        return true;
    }

    bool GLWidget::_updateInterpolatingCyclicCurveImage()
    {
        // updating VBO
        if (!_ip_cc->UpdateDataForInterpolation(_knot_vector, _data_points))
        {
            _destroyCyclicCurvesAndTheirImages();
            return false;
        }
        if (!_ip_cc->UpdateVertexBufferObjectsOfData())
        {
            _destroyCyclicCurvesAndTheirImages();
            return false;
        }

        // generating image
        if (_image_of_ip_cc)
        {
            delete _image_of_ip_cc;
        }

        _image_of_ip_cc = _ip_cc->GenerateImage(_max_order_of_derivatives, _cc_div_point_count, GL_STATIC_DRAW);

        if (!_image_of_ip_cc || !_image_of_ip_cc->UpdateVertexBufferObjects())
        {
            return false;
        }

        return true;
    }

    void GLWidget::_destroyCyclicCurvesAndTheirImages()
    {
        if (_cc)
        {
            delete _cc;
            _cc = nullptr;
        }
        if (_image_of_cc)
        {
            delete _image_of_cc;
            _image_of_cc = nullptr;
        }
        if (_ip_cc)
        {
            delete _ip_cc;
            _ip_cc = nullptr;
        }
        if (_image_of_ip_cc)
        {
            delete _image_of_ip_cc;
            _image_of_ip_cc = nullptr;
        }
    }

    bool GLWidget::_renderCyclicCurves()
    {
        if (!_cc || !_ip_cc || !_image_of_cc || !_image_of_ip_cc)
        {
            return false;
        }

        if (_show_cc)
        {
            glColor3f(1.0f, 0.0f, 0.0f);
            if (!_cc->RenderData(GL_LINE_LOOP))
            {
                return false;
            }
            _image_of_cc->RenderDerivatives(0, GL_LINE_LOOP);

            glPointSize(10.0f);
            glBegin(GL_POINTS);
                glVertex3dv(&(*_cc)[_selected_cp][0]);
            glEnd();
            glPointSize(1.0f);
        }

        glPointSize(5.0f);
        if (_show_cc_d1)
        {
            glColor3f(0.0f, 1.0f, 0.0f);
            _image_of_cc->RenderDerivatives(1, GL_LINES);
            _image_of_cc->RenderDerivatives(1, GL_POINTS);
        }
        if (_show_cc_d2)
        {
            glColor3f(0.0f, 0.0f, 1.0f);
            _image_of_cc->RenderDerivatives(2, GL_LINES);
            _image_of_cc->RenderDerivatives(2, GL_POINTS);
        }
        glPointSize(1.0f);


        if (_show_ip_cc)
        {
            glColor3f(1.0f, 0.0f, 1.0f);
            if (!_ip_cc->RenderData(GL_LINE_LOOP))
            {
                return false;
            }
            _image_of_ip_cc->RenderDerivatives(0, GL_LINE_LOOP);


            glPointSize(10.0f);
            glBegin(GL_POINTS);
                glVertex3dv(&_data_points[_selected_cp][0]);
            glEnd();
            glPointSize(1.0f);
        }

        glPointSize(5.0f);
        if (_show_ip_cc_d1)
        {
            glColor3f(1.0f, 1.0f, 0.0f);
            _image_of_ip_cc->RenderDerivatives(1, GL_LINES);
            _image_of_ip_cc->RenderDerivatives(1, GL_POINTS);
        }
        if (_show_ip_cc_d2)
        {
            glColor3f(0.0f, 1.0f, 1.0f);
            _image_of_ip_cc->RenderDerivatives(2, GL_LINES);
            _image_of_ip_cc->RenderDerivatives(2, GL_POINTS);
        }
        glPointSize(1.0f);

        return true;
    }

    //-----------------------------
    // methods of cubic bezier arcs
    //-----------------------------

    bool GLWidget::_createCubicCompositeCurve()
    {
        _compositeCurve = new CubicCompositeCurve3(1);
        if (!_compositeCurve)
        {
            return false;
        }

        return true;
    }

    void GLWidget::_destroyCubicCompositeCurve()
    {
        if (_compositeCurve)
        {
            delete _compositeCurve;
            _compositeCurve = nullptr;
        }
    }

    bool GLWidget::_renderCubicCompositeCurve()
    {
        if (!_compositeCurve)
        {
            return false;
        }

        if (!_compositeCurve->RenderHighlightedArcs(_selectedCurve1, _selectedCurve2))
        {
            return false;
        }

        if (!_compositeCurve->RenderAllArcs())
        {
            return false;
        }

        if (_showCurveData && !_compositeCurve->RenderAllData(_selectedCurve1, _selectedCurvePoint))
        {
            return false;
        }

        if (_showFirstOrderCurveDerivatives && !_compositeCurve->RenderAllFirstOrderDerivatives())
        {
            return false;
        }

        if (_showSecondOrderCurveDerivatives && !_compositeCurve->RenderAllSecondOrderDerivatives())
        {
            return false;
        }
        return true;
    }

    //----------------------------------
    // methods of bicubic bezier patches
    //----------------------------------

    bool GLWidget::_createBicubicCompositeSurface()
    {
        _compositeSurface = new BicubicCompositeSurface3(1);
        if (!_compositeSurface)
        {
            return false;
        }

        DCoordinate3 selectedPoint;
        _compositeSurface->GetDataPointValues(_selectedPatch1, _selectedPointRow, _selectedPointCol, selectedPoint);
        emit patch_control_point_x_changed(selectedPoint.x());
        emit patch_control_point_y_changed(selectedPoint.y());
        emit patch_control_point_z_changed(selectedPoint.z());

        return true;
    }

    void GLWidget::_destroyBicubicCompositeSurface()
    {
        if (_compositeSurface)
        {
            delete _compositeCurve;
            _compositeCurve = nullptr;
        }
    }

    bool GLWidget::_renderBicubicCompositeSurface()
    {
        if (!_dl || !_pl || !_sl)
        {
            return false;
        }

        if (_showPatchData && !_compositeSurface->RenderAllPatchesData(_selectedPatch1, _selectedPointRow, _selectedPointCol))
        {
            return false;
        }

        if (_shader)
        {
            _shaders[_selected_shader] -> Enable();
        }
        else
        {
            if (_directional_light)
            {
                _dl->Enable();
            }
            if (_point_like_light)
            {
                _pl->Enable();
            }
            if (_reflector_light)
            {
                _sl->Enable();
            }
        }

        if (!_compositeSurface)
        {
            return false;
        }

        if (!_compositeSurface->RenderHighlightedPatches(_selectedPatch1, _selectedPatch2))
        {
            return false;
        }

        if (!_compositeSurface->RenderAllPatches())
        {
            return false;
        }

        if (_showIsoLinesU && !_compositeSurface->RenderAllPatchesIsoU())
        {
            return false;
        }

        if (_showIsoLinesV && !_compositeSurface->RenderAllPatchesIsoV())
        {
            return false;
        }

        if (_showIsoLinesD1U && !_compositeSurface->RenderAllPatchesIsoUd1())
        {
            return false;
        }

        if (_showIsoLinesD1V && !_compositeSurface->RenderAllPatchesIsoVd1())
        {
            return false;
        }
        if (_showNormalVectors && !_compositeSurface->RenderAllPatchesNormal()){
            return false;
        }

        if (_shader)
        {
            _shaders[_selected_shader] -> Disable();
        }
        else
        {
            if (_directional_light)
            {
                _dl->Disable();
            }
            if (_point_like_light)
            {
                _pl->Disable();
            }
            if (_reflector_light)
            {
                _sl->Disable();
            }
        }
        return true;
    }

    //----------------------------
    // private slots for animation
    //----------------------------

    void GLWidget::_animate()
    {
        GLfloat *vertex = _star.MapVertexBuffer(GL_READ_WRITE);
        GLfloat *normal = _star.MapNormalBuffer(GL_READ_ONLY);

        _angle_rad += DEG_TO_RADIAN / 5.0;
        if (_angle_rad >= TWO_PI) _angle_rad -= TWO_PI;

        _angle += 0.2;
        if (_angle >= 360) _angle -= 360.0;

        GLfloat scale = sin(_angle_rad) / 3000.0;
        for (GLuint i = 0; i < _star.VertexCount(); ++i)
        {
            for (GLuint coordinate = 0; coordinate < 3; ++coordinate, ++vertex, ++normal)
                *vertex += scale * (*normal);
        }

        _star.UnmapVertexBuffer();
        _star.UnmapNormalBuffer();

        update();
    }

    //--------------------------------
    // special and default constructor
    //--------------------------------
    GLWidget::GLWidget(QWidget *parent): QOpenGLWidget(parent)
    {
        _timer = new QTimer(this);
        _timer->setInterval(0);

        connect(_timer, SIGNAL(timeout()), this, SLOT(_animate()));
    }

    GLWidget::~GLWidget()
    {
        _destroyAllExistingParametricCurvesAndTheirImages();
        _destroyAllExistingParametricSurfacesAndTheirImages();
        _destroyCyclicCurvesAndTheirImages();
        _destroyCubicCompositeCurve();
        _destroyBicubicCompositeSurface();
        _destroyDl();
        _destroySl();
        _destroyPl();
        _destroyTextures();
        _destroyShaders();
    }

    //--------------------------------------------------------------------------------------
    // this virtual function is called once before the first call to paintGL() or resizeGL()
    //--------------------------------------------------------------------------------------
    void GLWidget::initializeGL()
    {        
        // creating a perspective projection matrix
        glMatrixMode(GL_PROJECTION);

        glLoadIdentity();

        _aspect = static_cast<double>(width()) / static_cast<double>(height());
        _z_near = 1.0;
        _z_far  = 100.0;
        _fovy   = 45.0;

        gluPerspective(_fovy, _aspect, _z_near, _z_far);

        // setting the model view matrix
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        _eye[0] = _eye[1] = 0.0; _eye[2] = 6.0;
        _center[0] = _center[1] = _center[2] = 0.0;
        _up[0] = _up[2] = 0.0; _up[1] = 1.0;

        gluLookAt(_eye[0], _eye[1], _eye[2], _center[0], _center[1], _center[2], _up[0], _up[1], _up[2]);

        // enabling the depth test
        glEnable(GL_DEPTH_TEST);

        // setting the background color
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        // initial values of transformation parameters
        _angle_x = _angle_y = _angle_z = 0.0;
        _trans_x = _trans_y = _trans_z = 0.0;
        _zoom = 1.0;

        // ...

        try
        {
            // initializing the OpenGL Extension Wrangler library
            GLenum error = glewInit();

            if (error != GLEW_OK)
            {
                throw Exception("Could not initialize the OpenGL Extension Wrangler Library!");
            }

            if (!glewIsSupported("GL_VERSION_2_0"))
            {
                throw Exception("Your graphics card is not compatible with OpenGL 2.0+! "
                                "Try to update your driver or buy a new graphics adapter!");
            }

            if (!_loadAllModelsFromOff())
            {
                throw Exception("Could not load all models from OFF!");
            }

            if (!_updateAllModels())
            {
                throw Exception("Could not update all models!");
            }

            if (!_createAllParametricCurvesAndTheirImages())
            {
                throw Exception("Could not create all parametric curves!");
            }

            if (!_createAllParametricSurfacesAndTheirImages())
            {
                throw Exception("Could not create all parametric surfaces!");
            }

            if (!_createCyclicCurvesAndTheirImages())
            {
                throw Exception("Could not create cyclic curves");
            }

            if (!_createCubicCompositeCurve())
            {
                throw Exception("Could not create cubic bezier arc");
            }

            if (!_createBicubicCompositeSurface())
            {
                throw Exception("Could not create bicubic bezier patches");
            }

            if (!_createShaders())
            {
                throw Exception("Could not create all shaders");
            }

            if (!_createDl())
            {
                throw Exception("Could not create the dirctional light object");
            }

            if (!_createPl())
            {
                throw Exception("Could not create the point light object");
            }

            if (!_createSl())
            {
                throw Exception("Could not create the spotlight object");
            }

            if (!_loadTextures())
            {
                throw Exception("Could not load textures");
            }

            // animation timer
            _angle = _angle_rad = 0.0;
            _timer->start();
        }
        catch (Exception &e)
        {
            cout << e << endl;
        }

        glEnable(GL_POINT_SMOOTH);
        glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

        glEnable(GL_LINE_SMOOTH);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

        glEnable(GL_POLYGON_SMOOTH);
        glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

        glEnable(GL_DEPTH_TEST);
    }

    //-----------------------
    // the rendering function
    //-----------------------
    void GLWidget::paintGL()
    {
        // clears the color and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // stores/duplicates the original model view matrix
        glPushMatrix();

            // applying transformations
            glRotatef(_angle_x, 1.0, 0.0, 0.0);
            glRotatef(_angle_y, 0.0, 1.0, 0.0);
            glRotatef(_angle_z, 0.0, 0.0, 1.0);
            glTranslated(_trans_x, _trans_y, _trans_z);
            glScaled(_zoom, _zoom, _zoom);
            glColor3f(1.0, 1.0, 1.0);

            switch (_homework_id)
            {
            case 0:
                _privateTestFunction();
                break;

            case 1:
                _communicateWithShaders();
                _renderPlayground();
                break;

            case 2:
                _renderSelectedParametricCurve();
                break;

            case 3:
                _renderSelectedParametricSurface();
                break;

            case 4:
                _renderCyclicCurves();
                break;
            case 5:
                _renderCubicCompositeCurve();
                break;
            case 6:
                _communicateWithShaders();
                _renderBicubicCompositeSurface();
                break;
            }

        // pops the current matrix stack, replacing the current matrix with the one below it on the stack,
        // i.e., the original model view matrix is restored
        glPopMatrix();
    }

    //----------------------------------------------------------------------------
    // when the main window is resized one needs to redefine the projection matrix
    //----------------------------------------------------------------------------
    void GLWidget::resizeGL(int w, int h)
    {
        // setting the new size of the rendering context
        glViewport(0, 0, w, h);

        // redefining the projection matrix
        glMatrixMode(GL_PROJECTION);

        glLoadIdentity();

        _aspect = static_cast<double>(w) / static_cast<double>(h);

        gluPerspective(_fovy, _aspect, _z_near, _z_far);

        // switching back to the model view matrix
        glMatrixMode(GL_MODELVIEW);

        update();
    }

    std::ostream& GLWidget::saveCurves(std::ostream & stream)
    {
        return stream << *_compositeCurve;
    }

    std::istream& GLWidget::loadCurves(std::istream & stream)
    {
        return stream >> *_compositeCurve;
        update();
    }

    std::ostream& GLWidget::saveSurfaces(std::ostream & stream)
    {
        return stream << *_compositeSurface;
    }

    std::istream& GLWidget::loadSurfaces(std::istream & stream)
    {
        return stream >> *_compositeSurface;
        update();
    }

    //-----------------------------------
    // implementation of the public slots
    //-----------------------------------

    void GLWidget::set_angle_x(int value)
    {
        if (_angle_x != value)
        {
            _angle_x = value;
            update();
        }
    }

    void GLWidget::set_angle_y(int value)
    {
        if (_angle_y != value)
        {
            _angle_y = value;
            update();
        }
    }

    void GLWidget::set_angle_z(int value)
    {
        if (_angle_z != value)
        {
            _angle_z = value;
            update();
        }
    }

    void GLWidget::set_zoom_factor(double value)
    {
        if (_zoom != value)
        {
            _zoom = value;
            update();
        }
    }

    void GLWidget::set_trans_x(double value)
    {
        if (_trans_x != value)
        {
            _trans_x = value;
            update();
        }
    }

    void GLWidget::set_trans_y(double value)
    {
        if (_trans_y != value)
        {
            _trans_y = value;
            update();
        }
    }

    void GLWidget::set_trans_z(double value)
    {
        if (_trans_z != value)
        {
            _trans_z = value;
            update();
        }
    }

    // ---------------------------------------------------
    // slots of shaderers
    // ---------------------------------------------------

    void GLWidget::set_shader_light(int index)
    {
        if (index == 0)
        {
            _shader = true;
            _light = false;
        }
        else
        {
            _shader = false;
            _light = true;
        }
        update();
    }

    void GLWidget::set_dl_shader_selected(bool value)
    {
        if (_shader_selected[0] != value)
        {
            _shader_selected[0] = value;
            update();
        }
    }
    void GLWidget::set_rl_shader_selected(bool value)
    {
        if (_shader_selected[1] != value)
        {
            _shader_selected[1] = value;
            update();
        }
    }
    void GLWidget::set_toon_shader_selected(bool value)
    {
        if (_shader_selected[2] != value)
        {
            _shader_selected[2] = value;
            update();
        }
    }
    void GLWidget::set_two_sl_shader_selected(bool value)
    {
        if (_shader_selected[3] != value)
        {
            _shader_selected[3] = value;
            update();
        }
    }

    void GLWidget::set_rl_scale(double value)
    {
        _rl_scale = value;
        update();
    }
    void GLWidget::set_rl_smoothing(double value)
    {
        _rl_smoothing = value;
        update();
    }
    void GLWidget::set_rl_shading(double value)
    {
        _rl_shading = value;
        update();
    }
    void GLWidget::set_toon_def_outline_r(double value)
    {
        _toon_def_outline[0] = value;
        update();
    }
    void GLWidget::set_toon_def_outline_g(double value)
    {
        _toon_def_outline[1] = value;
        update();
    }
    void GLWidget::set_toon_def_outline_b(double value)
    {
        _toon_def_outline[2] = value;
        update();
    }
    void GLWidget::set_toon_def_outline_a(double value)
    {
        _toon_def_outline[3] = value;
        update();
    }

    void GLWidget::setID(int id)
    {
        if (_homework_id != id)
        {
            if (_homework_id == 1)
            {
                _timer->stop();
            }
            _homework_id = id;
            if (_homework_id == 1)
            {
                _timer->start();
            }
            update();
        }
    }

    void GLWidget::resetTransformations()
    {
        _angle_x = _angle_y = _angle_z = 0.0;
        emit setAngle(0);

        _trans_x = _trans_y = _trans_z = 0.0;
        emit setTranslate(0.0);

        _zoom = 1.0;
        emit setZoom(1.0);

        update();
    }

    // ---------------------------------------------------
    // slots of parametric curves
    // ---------------------------------------------------

    void GLWidget::setParametricCurveIndex(int index)
    {
        if (_selected_pc != index)
        {
            _selected_pc = index;
            update();
        }
    }

    void GLWidget::setVisibilityOfTangents(bool visibility)
    {
        if (_show_tangents != visibility)
        {
            _show_tangents = visibility;
            update();
        }
    }

    void GLWidget::setVisibilityOfAccelerationVectors(bool visibility)
    {
        if (_show_acceleration_vectors != visibility)
        {
            _show_acceleration_vectors = visibility;
            update();
        }
    }

    void GLWidget::setDerivativeScale(double scale)
    {
        _scale = scale;
        _createSelectedParametricCurveImage();
        update();
    }

    void GLWidget::setDivPointCount(int div_point_count)
    {
        if (_div_point_count != div_point_count)
        {
            _div_point_count = div_point_count;
            _createSelectedParametricCurveImage();
            update();
        }
    }

    void GLWidget::resetPcAttributes()
    {
        _show_tangents = false;
        _show_acceleration_vectors = false;
        emit derivativeVisibilitySignal(false);
        setDerivativeScale(1.0);
        emit derivativeScaleSignal(1.0);
        setDivPointCount(200);
        emit divisionPointCountSignal(200);
    }

    // ---------------------------------------------------
    // slots of parametric surfaces
    // ---------------------------------------------------

    void GLWidget::setParametricSurfaceIndex(int index)
    {
        if (_selected_ps != index)
        {
            _selected_ps = index;
            update();
        }
    }

    void GLWidget::setDivPointCountU(int u_div_point_count)
    {
        if (_u_div_point_count != u_div_point_count)
        {
            _u_div_point_count = u_div_point_count;
            _createSelectedParametricSurfaceImage();
            update();
        }
    }

    void GLWidget::setDivPointCountV(int v_div_point_count)
    {
        if (_v_div_point_count != v_div_point_count)
        {
            _v_div_point_count = v_div_point_count;
            _createSelectedParametricSurfaceImage();
            update();
        }
    }

    void GLWidget::setShowTexture(bool show_texture)
    {
        if (_show_texture != show_texture)
        {
            _show_texture = show_texture;
            update();
        }
    }

    // ---------------------------------------------------
    // slots of cyclic curves
    // ---------------------------------------------------

    void GLWidget::setSelectedControlPoint(int index)
    {
        _selected_cp = index;
        emit cc_control_point_x_changed(_data_points[_selected_cp].x());
        emit cc_control_point_y_changed(_data_points[_selected_cp].y());
        emit cc_control_point_z_changed(_data_points[_selected_cp].z());
    }

    void GLWidget::cc_cp_set_x(double x)
    {
        _data_points[_selected_cp].x() = x;
        (*_cc)[_selected_cp].x() = x;
        _updateCyclicCurveImage();
        _updateInterpolatingCyclicCurveImage();
        update();
    }
    void GLWidget::cc_cp_set_y(double y)
    {
        _data_points[_selected_cp].y() = y;
        (*_cc)[_selected_cp].y() = y;
        _updateCyclicCurveImage();
        _updateInterpolatingCyclicCurveImage();
        update();
    }
    void GLWidget::cc_cp_set_z(double z)
    {
        _data_points[_selected_cp].z() = z;
        (*_cc)[_selected_cp].z() = z;
        _updateCyclicCurveImage();
        _updateInterpolatingCyclicCurveImage();
        update();
    }

    void GLWidget::setCcVisibility(bool visibility)
    {
        _show_cc = visibility;
        update();
    }
    void GLWidget::setCcD1Visibility(bool visibility)
    {
        _show_cc_d1 = visibility;
        update();
    }
    void GLWidget::setCcD2Visibility(bool visibility)
    {
        _show_cc_d2 = visibility;
        update();
    }
    void GLWidget::setIpCcVisibility(bool visibility)
    {
        _show_ip_cc = visibility;
        update();
    }
    void GLWidget::setIpCcD1Visibility(bool visibility)
    {
        _show_ip_cc_d1 = visibility;
        update();
    }
    void GLWidget::setIpCcD2Visibility(bool visibility)
    {
        _show_ip_cc_d2 = visibility;
        update();
    }

    // ---------------------------------------------------
    // slots of composite curve
    // ---------------------------------------------------


    void GLWidget::set_selected_curve1(int index)
    {
        int ind = _compositeCurve->GetArcCount();
        if (index < ind)
        {
            _selectedCurve1 = index;
            emit selected_arc_color(_compositeCurve->GetColorInd(_selectedCurve1));
            emit selected_cp_arc(0);
            set_selected_cp_arc(0);
        } else {
            _selectedCurve1 = ind-1;
            emit selected_curve1(ind-1);
        }
    }

    void GLWidget::set_selected_curve2(int index)
    {
        int ind = _compositeCurve->GetArcCount();
        if (index < ind)
        {
            _selectedCurve2 = index;
        } else {
            _selectedCurve2 = ind-1;
            emit selected_curve2(ind-1);
        }
    }

    void GLWidget::set_selected_cp_arc(int index)
    {
        _selectedCurvePoint = index;
        DCoordinate3 selectedPoint;
        _compositeCurve->GetDataPointValues(_selectedCurve1, _selectedCurvePoint, selectedPoint);
        emit arc_control_point_x_changed(selectedPoint.x());
        emit arc_control_point_y_changed(selectedPoint.y());
        emit arc_control_point_z_changed(selectedPoint.z());
    }

    void GLWidget::arc_cp_set_x(double x)
    {
        DCoordinate3 pointToUpdate;
        _compositeCurve->GetDataPointValues(_selectedCurve1, _selectedCurvePoint, pointToUpdate);
        pointToUpdate.x() = x;
        _compositeCurve->UpdateArc(_selectedCurve1, _selectedCurvePoint, pointToUpdate);
        update();
    }

    void GLWidget::arc_cp_set_y(double y)
    {
        DCoordinate3 pointToUpdate;
        _compositeCurve->GetDataPointValues(_selectedCurve1, _selectedCurvePoint, pointToUpdate);
        pointToUpdate.y() = y;
        _compositeCurve->UpdateArc(_selectedCurve1, _selectedCurvePoint, pointToUpdate);
        update();
    }

    void GLWidget::arc_cp_set_z(double z)
    {
        DCoordinate3 pointToUpdate;
        _compositeCurve->GetDataPointValues(_selectedCurve1, _selectedCurvePoint, pointToUpdate);
        pointToUpdate.z() = z;
        _compositeCurve->UpdateArc(_selectedCurve1, _selectedCurvePoint, pointToUpdate);
        update();
    }

    void GLWidget::set_arc_d1_visibility(bool value)
    {
        _showFirstOrderCurveDerivatives = value;
        update();
    }
    void GLWidget::set_arc_d2_visibility(bool value)
    {
        _showSecondOrderCurveDerivatives = value;
        update();
    }
    void GLWidget::set_arc_data_visibility(bool value)
    {
        _showCurveData = value;
        update();
    }

    void GLWidget::new_arc()
    {
        _compositeCurve->InsertNewArc();
        update();
    }

    void GLWidget::cont_arc()
    {
        if (_compositeCurve->ContinueExistingArc(_selectedCurve1, _arc_dir_1))
        {
            update();
        }
    }

    void GLWidget::join_arcs()
    {
        if (_compositeCurve->JoinExistingArcs(_selectedCurve1, _arc_dir_1, _selectedCurve2, _arc_dir_2))
        {
            update();
        }
    }

    void GLWidget::merge_arcs()
    {
        if (_compositeCurve->MergeExistingArcs(_selectedCurve1, _arc_dir_1, _selectedCurve2, _arc_dir_2))
        {
            update();
        }
    }

    void GLWidget::set_arc_dir_1(int dir)
    {
        if (_arc_dir_1 != dir)
        {
            _arc_dir_1 = dir == 0 ? CubicCompositeCurve3::Direction::LEFT : CubicCompositeCurve3::Direction::RIGHT;
        }
    }

    void GLWidget::set_arc_dir_2(int dir)
    {
        if (_arc_dir_2 != dir)
        {
            _arc_dir_2 = dir == 0 ? CubicCompositeCurve3::Direction::LEFT : CubicCompositeCurve3::Direction::RIGHT;
        }
    }

    void GLWidget::set_arc_color(int ind)
    {
        _compositeCurve->ChangeColor(_selectedCurve1, ind);
        update();
    }

    // ---------------------------------------------------
    // slots of composite surface
    // ---------------------------------------------------

    void GLWidget::set_selected_patch1(int index)
    {
        int ind = _compositeSurface->GetPatchCount();

        if (index < ind)
        {
            _selectedPatch1 = index;
            emit selected_cp_row(0);
            emit selected_cp_column(0);
            set_selected_cp_patch_row(0);
            set_selected_cp_patch_column(0);
        } else {
            _selectedPatch1 = ind-1;
            emit selected_patch1(ind-1);
        }
    }

    void GLWidget::set_selected_patch2(int index)
    {
        int ind = _compositeSurface->GetPatchCount();

        if (index < ind)
        {
            _selectedPatch2 = index;
        } else {
            _selectedPatch2 = ind-1;
            emit selected_patch2(ind-1);
        }
    }

    void GLWidget::set_selected_cp_patch_row(int index)
    {
        _selectedPointRow = index;
        DCoordinate3 selectedPoint;
        _compositeSurface->GetDataPointValues(_selectedPatch1, _selectedPointRow, _selectedPointCol, selectedPoint);
        emit patch_control_point_x_changed(selectedPoint.x());
        emit patch_control_point_y_changed(selectedPoint.y());
        emit patch_control_point_z_changed(selectedPoint.z());
    }

    void GLWidget::set_selected_cp_patch_column(int index)
    {
        _selectedPointCol = index;
        DCoordinate3 selectedPoint;
        _compositeSurface->GetDataPointValues(_selectedPatch1, _selectedPointRow, _selectedPointCol, selectedPoint);
        emit patch_control_point_x_changed(selectedPoint.x());
        emit patch_control_point_y_changed(selectedPoint.y());
        emit patch_control_point_z_changed(selectedPoint.z());
    }

    void GLWidget::patch_cp_set_x(double x)
    {
        DCoordinate3 selectedPoint;
        _compositeSurface->GetDataPointValues(_selectedPatch1, _selectedPointRow, _selectedPointCol, selectedPoint);
        selectedPoint.x() = x;
        _compositeSurface->UpdatePatch(_selectedPatch1, _selectedPointRow, _selectedPointCol, selectedPoint);
        update();
    }

    void GLWidget::patch_cp_set_y(double y)
    {
        DCoordinate3 selectedPoint;
        _compositeSurface->GetDataPointValues(_selectedPatch1, _selectedPointRow, _selectedPointCol, selectedPoint);
        selectedPoint.y() = y;
        _compositeSurface->UpdatePatch(_selectedPatch1, _selectedPointRow, _selectedPointCol, selectedPoint);
        update();
    }

    void GLWidget::patch_cp_set_z(double z)
    {
        DCoordinate3 selectedPoint;
        _compositeSurface->GetDataPointValues(_selectedPatch1, _selectedPointRow, _selectedPointCol, selectedPoint);
        selectedPoint.z() = z;
        _compositeSurface->UpdatePatch(_selectedPatch1, _selectedPointRow, _selectedPointCol, selectedPoint);
        update();
    }

    void GLWidget::setIsoLineUVisibility(bool visibility)
    {
        _showIsoLinesU = visibility;
        update();
    }
    void GLWidget::setIsoLineVVisibility(bool visibility)
    {
        _showIsoLinesV = visibility;
        update();
    }
    void GLWidget::setIsoLineD1UVisibility(bool visibility)
    {
        _showIsoLinesD1U = visibility;
        update();
    }
    void GLWidget::setIsoLineD1VVisibility(bool visibility)
    {
        _showIsoLinesD1V = visibility;
        update();
    }
    void GLWidget::setNormalsVisibility(bool visibility)
    {
        _showNormalVectors = visibility;
        update();
    }
    void GLWidget::setPatchDataVisibility(bool visibility)
    {
        _showPatchData = visibility;
        update();
    }

    void GLWidget::setShader(bool shader)
    {
        _shader = shader;
        update();
    }

    void GLWidget::setLight(bool light)
    {
        _light = light;
        update();
    }

    void GLWidget::setShaderType(int selected_shader)
    {
        _selected_shader = selected_shader;
        update();
    }

    void GLWidget::setDirectionalLight(bool directional_light)
    {
        _directional_light = directional_light;
        update();
    }

    void GLWidget::setPointLikeLight(bool point_like_light)
    {
        _point_like_light = point_like_light;
        update();
    }

    void GLWidget::setReflectorLight(bool reflector_light)
    {
        _reflector_light = reflector_light;
        update();
    }

    void GLWidget::new_patch()
    {
        _compositeSurface->InsertNewPatch();
        update();
    }

    void GLWidget::cont_patch()
    {
        if (_compositeSurface->ContinueExistingPatch(_selectedCurve1, _patch_dir_1))
        {
            update();
        }
    }

    void GLWidget::join_patches()
    {
        if (_compositeSurface->JoinExistingPatches(_selectedCurve1, _patch_dir_1, _selectedCurve2, _patch_dir_2))
        {
            update();
        }
    }

    void GLWidget::merge_patches()
    {
        if (_compositeSurface->MergeExistingPatches(_selectedCurve1, _patch_dir_1, _selectedCurve2, _patch_dir_2))
        {
            update();
        }
    }

    void GLWidget::set_patch_dir_1(int dir)
    {
        if (_patch_dir_1 != dir)
        {
            switch (dir)
            {
            case 0: _patch_dir_1 = BicubicCompositeSurface3::Direction::N;
            case 1: _patch_dir_1 = BicubicCompositeSurface3::Direction::NW;
            case 2: _patch_dir_1 = BicubicCompositeSurface3::Direction::W;
            case 3: _patch_dir_1 = BicubicCompositeSurface3::Direction::SW;
            case 4: _patch_dir_1 = BicubicCompositeSurface3::Direction::S;
            case 5: _patch_dir_1 = BicubicCompositeSurface3::Direction::SE;
            case 6: _patch_dir_1 = BicubicCompositeSurface3::Direction::E;
            case 7: _patch_dir_1 = BicubicCompositeSurface3::Direction::NE;
            }
        }
    }

    void GLWidget::set_patch_dir_2(int dir)
    {
        if (_patch_dir_2 != dir)
        {
            switch (dir)
            {
            case 0: _patch_dir_2 = BicubicCompositeSurface3::Direction::N;
            case 1: _patch_dir_2 = BicubicCompositeSurface3::Direction::NW;
            case 2: _patch_dir_2 = BicubicCompositeSurface3::Direction::W;
            case 3: _patch_dir_2 = BicubicCompositeSurface3::Direction::SW;
            case 4: _patch_dir_2 = BicubicCompositeSurface3::Direction::S;
            case 5: _patch_dir_2 = BicubicCompositeSurface3::Direction::SE;
            case 6: _patch_dir_2 = BicubicCompositeSurface3::Direction::E;
            case 7: _patch_dir_2 = BicubicCompositeSurface3::Direction::NE;
            }
        }
    }
}
