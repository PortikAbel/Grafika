#pragma once

#include <GL/glew.h>
#include <QOpenGLWidget>
#include <QOpenGLTexture>
#include <QImage>
#include <QTimer>
#include "../Parametric/ParametricCurves3.h"
#include "../Core/TriangulatedMeshes3.h"
#include "../Core/Lights.h"
#include "../Core/ShaderPrograms.h"
#include "../Cyclic/CyclicCurves3.h"
#include "../Bezier/CubicCompositeCurve3.h"
#include "../Bezier/BicubicCompositeSurface3.h"

namespace cagd
{
    class GLWidget: public QOpenGLWidget
    {
        Q_OBJECT

    private:

        // variables defining the projection matrix
        double       _aspect;            // aspect ratio of the rendering window
        double       _fovy;              // field of view in direction y
        double       _z_near, _z_far;    // distance of near and far clipping planes

        // variables defining the model-view matrix
        double       _eye[3], _center[3], _up[3];

        // variables needed by transformations
        int         _angle_x, _angle_y, _angle_z;
        double      _zoom;
        double      _trans_x, _trans_y, _trans_z;

        // your other declarations

        DirectionalLight*   _dl = nullptr;
        bool                _createDl();
        void                _destroyDl();

        Spotlight*          _sl = nullptr;
        bool                _createSl();
        void                _destroySl();

        PointLight*         _pl = nullptr;
        bool                _createPl();
        void                _destroyPl();

        QOpenGLTexture*     _texture = nullptr;
        bool                _loadTextures();
        void                _destroyTextures();

        // shaders
        RowMatrix<ShaderProgram*>   _shaders;
        std::vector<bool>           _shader_selected;
        float       _rl_scale = 1.0;
        float       _rl_smoothing = 0.0;
        float       _rl_shading = 0.0;
        Color4      _toon_def_outline;

        bool    _createShaders();
        bool    _communicateWithShaders();
        void    _destroyShaders();

        // a number that can be used to switch between different test-cases
        int  _homework_id = 6;

        // DCoordinates + TemplateMatrices
        // ID = 0
        // a simple test-function that does not need dynamical memory allocation, e.g.,
        // _testingDCoordinates(), _testingTemplateMatrices(), etc.
        void _privateTestFunction();

        // models
        // ID = 1

        QTimer *_timer;
        GLfloat _angle, _angle_rad;

        TriangulatedMesh3 _space_station, _star, _sphere;

        bool _loadAllModelsFromOff();
        bool _updateAllModels();
        bool _renderPlayground();

        // parametric curves
        // ID = 2
        GLuint                       _pc_count = 6;
        RowMatrix<ParametricCurve3*> _pc;
        RowMatrix<GenericCurve3*>    _image_of_pc;
        int                          _selected_pc = 0;
        bool                         _show_tangents = false;
        bool                         _show_acceleration_vectors = false;
        int                          _div_point_count = 200;
        GLdouble                     _scale = 1.0;
        GLenum                       _usage_flag_pc = GL_STATIC_DRAW;

        bool _createAllParametricCurvesAndTheirImages();
        void _destroyAllExistingParametricCurvesAndTheirImages();
        bool _createSelectedParametricCurveImage();
        bool _renderSelectedParametricCurve();

        // parametric surfaces
        // ID = 3
        GLuint                          _ps_count = 6;
        RowMatrix<ParametricSurface3*>  _ps;
        RowMatrix<TriangulatedMesh3*>   _image_of_ps;
        int                             _selected_ps = 0;
        int                             _u_div_point_count = 200;
        int                             _v_div_point_count = 200;
        bool                            _show_texture = false;
        GLenum                          _usage_flag_ps = GL_STATIC_DRAW;

        bool _createAllParametricSurfacesAndTheirImages();
        void _destroyAllExistingParametricSurfacesAndTheirImages();
        bool _createSelectedParametricSurfaceImage();
        bool _renderSelectedParametricSurface();

        // cyclic curves
        // ID = 4

        CyclicCurve3*               _cc;
        CyclicCurve3*               _ip_cc;
        GenericCurve3*              _image_of_cc = nullptr;
        GenericCurve3*              _image_of_ip_cc = nullptr;
        ColumnMatrix<GLdouble>      _knot_vector;
        ColumnMatrix<DCoordinate3>  _data_points;
        int                         _cc_div_point_count = 200;
        int                         _selected_cp = 0;
        bool    _show_cc = true,    _show_cc_d1 = false,    _show_cc_d2 = false,
                _show_ip_cc = true, _show_ip_cc_d1 = false, _show_ip_cc_d2 = false;
        GLuint  _max_order_of_derivatives = 2;

        bool _createCyclicCurvesAndTheirImages();
        bool _updateCyclicCurveImage();
        bool _updateInterpolatingCyclicCurveImage();
        void _destroyCyclicCurvesAndTheirImages();
        bool _renderCyclicCurves();

        GenericCurve3 *ptr = nullptr;

        // Bezier curves
        // ID = 5

        CubicCompositeCurve3*   _compositeCurve;
        GLuint      _selectedCurve      = 0;
        GLuint      _selectedCurvePoint = 0;
        bool    _showFirstOrderCurveDerivatives = false;
        bool    _showSecondOrderCurveDerivatives = false;

        bool _createCubicCompositeCurve();
        void _destroyCubicCompositeCurve();
        bool _renderCubicCompositeCurve();

        // Bezier patches
        // ID = 6

        BicubicCompositeSurface3*   _compositeSurface;
        GLuint      _selectedPatch  = 0;
        GLuint      _selectedPointRow   = 0,    _selectedPointCol = 0;
        bool        _showIsoLinesU      = false;
        bool        _showIsoLinesV      = false;
        bool        _showIsoLinesD1U    = false;
        bool        _showIsoLinesD1V    = false;
        bool        _shader             = false;
        bool        _light              = true;
        int         _selected_shader    = 0;
        bool        _directional_light  = false;
        bool        _reflector_light    = false;
        bool        _point_like_light   = false;


        bool _createBicubicCompositeSurface();
        void _destroyBicubicCompositeSurface();
        bool _renderBicubicCompositeSurface();

    private slots:
        void _animate();

    public:
        // special and default constructor
        // the format specifies the properties of the rendering window
        GLWidget(QWidget* parent = 0);

        // redeclared virtual functions
        void initializeGL();
        void paintGL();
        void resizeGL(int w, int h);

        // file handling methods
        std::ostream& saveCurves(std::ostream&);
        std::istream& loadCurves(std::istream&);
        std::ostream& saveSurfaces(std::ostream&);
        std::istream& loadSurfaces(std::istream&);

        virtual ~GLWidget();

    public slots:
        // public event handling methods/slots
        void set_angle_x(int value);
        void set_angle_y(int value);
        void set_angle_z(int value);

        void set_zoom_factor(double value);

        void set_trans_x(double value);
        void set_trans_y(double value);
        void set_trans_z(double value);

        void resetTransformations();

        void set_dl_shader_selected(bool value);
        void set_rl_shader_selected(bool value);
        void set_toon_shader_selected(bool value);
        void set_two_sl_shader_selected(bool value);
        void set_rl_scale(double value);
        void set_rl_smoothing(double value);
        void set_rl_shading(double value);
        void set_toon_def_outline_r(double value);
        void set_toon_def_outline_g(double value);
        void set_toon_def_outline_b(double value);
        void set_toon_def_outline_a(double value);

        void setID(int id);

        void setParametricCurveIndex(int index);
        void setVisibilityOfTangents(bool visibility);
        void setVisibilityOfAccelerationVectors(bool visibility);
        void setDerivativeScale(double scale);
        void setDivPointCount(int div_point_count);

        void resetPcAttributes();

        void setParametricSurfaceIndex(int index);
        void setDivPointCountU(int u_div_point_count);
        void setDivPointCountV(int v_div_point_count);
        void setShowTexture(bool show_texture);

        void setSelectedControlPoint(int);
        void cc_cp_set_x(double);
        void cc_cp_set_y(double);
        void cc_cp_set_z(double);
        void setCcVisibility(bool);
        void setCcD1Visibility(bool);
        void setCcD2Visibility(bool);
        void setIpCcVisibility(bool);
        void setIpCcD1Visibility(bool);
        void setIpCcD2Visibility(bool);

        void set_selected_cp_arc(int);
        void arc_cp_set_x(double);
        void arc_cp_set_y(double);
        void arc_cp_set_z(double);
        void set_arc_d1_visibility(bool);
        void set_arc_d2_visibility(bool);

        //Patch
        void set_selected_cp_patch_row(int);
        void set_selected_cp_patch_column(int);
        void patch_cp_set_x(double);
        void patch_cp_set_y(double);
        void patch_cp_set_z(double);

        void setIsoLineUVisibility(bool);
        void setIsoLineVVisibility(bool);
        void setIsoLineD1UVisibility(bool);
        void setIsoLineD1VVisibility(bool);
        void shaderOrLight(int);
        void setShaderType(int);
        void setDirectionalLight(bool);
        void setReflectorLight(bool);
        void setPointLikeLight(bool);

    signals:
        void setAngle(int);
        void setTranslate(double);
        void setZoom(double);

        void derivativeVisibilitySignal(bool);
        void derivativeScaleSignal(double);
        void divisionPointCountSignal(int);

        void cc_control_point_x_changed(double);
        void cc_control_point_y_changed(double);
        void cc_control_point_z_changed(double);

        void arc_control_point_x_changed(double);
        void arc_control_point_y_changed(double);
        void arc_control_point_z_changed(double);

        void patch_control_point_x_changed(double);
        void patch_control_point_y_changed(double);
        void patch_control_point_z_changed(double);
    };
}
