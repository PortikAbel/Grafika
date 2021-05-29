#include "MainWindow.h"
#include "../Core/DCoordinates3.h"

namespace cagd
{
    MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
    {
        setupUi(this);

    /*

      the structure of the main window's central widget

     *---------------------------------------------------*
     |                 central widget                    |
     |                                                   |
     |  *---------------------------*-----------------*  |
     |  |     rendering context     |   scroll area   |  |
     |  |       OpenGL widget       | *-------------* |  |
     |  |                           | | side widget | |  |
     |  |                           | |             | |  |
     |  |                           | |             | |  |
     |  |                           | *-------------* |  |
     |  *---------------------------*-----------------*  |
     |                                                   |
     *---------------------------------------------------*

    */
        _side_widget = new SideWidget(this);

        _scroll_area = new QScrollArea(this);
        _scroll_area->setWidget(_side_widget);
        _scroll_area->setSizePolicy(_side_widget->sizePolicy());
        _scroll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

        _gl_widget = new GLWidget(this);

        centralWidget()->setLayout(new QHBoxLayout());
        centralWidget()->layout()->addWidget(_gl_widget);
        centralWidget()->layout()->addWidget(_scroll_area);

        // creating a signal slot mechanism between the rendering context and the side widget


        // transformations
        connect(_side_widget->rotate_x_slider, SIGNAL(valueChanged(int)), _gl_widget, SLOT(set_angle_x(int)));
        connect(_side_widget->rotate_y_slider, SIGNAL(valueChanged(int)), _gl_widget, SLOT(set_angle_y(int)));
        connect(_side_widget->rotate_z_slider, SIGNAL(valueChanged(int)), _gl_widget, SLOT(set_angle_z(int)));

        connect(_side_widget->zoom_factor_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_zoom_factor(double)));

        connect(_side_widget->trans_x_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_trans_x(double)));
        connect(_side_widget->trans_y_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_trans_y(double)));
        connect(_side_widget->trans_z_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_trans_z(double)));

        connect(_side_widget->resetButton, SIGNAL(pressed()), _gl_widget, SLOT(resetTransformations()));

        connect(_gl_widget, SIGNAL(setAngle(int)), _side_widget->rotate_x_slider, SLOT(setValue(int)));
        connect(_gl_widget, SIGNAL(setAngle(int)), _side_widget->rotate_y_slider, SLOT(setValue(int)));
        connect(_gl_widget, SIGNAL(setAngle(int)), _side_widget->rotate_z_slider, SLOT(setValue(int)));

        connect(_gl_widget, SIGNAL(setZoom(double)), _side_widget->zoom_factor_spin_box, SLOT(setValue(double)));

        connect(_gl_widget, SIGNAL(setTranslate(double)), _side_widget->trans_x_spin_box, SLOT(setValue(double)));
        connect(_gl_widget, SIGNAL(setTranslate(double)), _side_widget->trans_y_spin_box, SLOT(setValue(double)));
        connect(_gl_widget, SIGNAL(setTranslate(double)), _side_widget->trans_z_spin_box, SLOT(setValue(double)));

        // shaders
        //connect(_side_widget->dl_shader_check_box, SIGNAL(toggled(bool)), _gl_widget, SLOT(set_dl_shader_selected(bool)));
        //connect(_side_widget->rl_shader_check_box, SIGNAL(toggled(bool)), _gl_widget, SLOT(set_rl_shader_selected(bool)));
        //connect(_side_widget->toon_shader_check_box, SIGNAL(toggled(bool)), _gl_widget, SLOT(set_toon_shader_selected(bool)));
        //connect(_side_widget->two_sl_shader_check_box, SIGNAL(toggled(bool)), _gl_widget, SLOT(set_two_sl_shader_selected(bool)));

        connect(_side_widget->rl_scale_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_rl_scale(double)));
        connect(_side_widget->rl_smoothing_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_rl_smoothing(double)));
        connect(_side_widget->rl_shading_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_rl_shading(double)));

        connect(_side_widget->toon_outline_r_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_toon_def_outline_r(double)));
        connect(_side_widget->toon_outline_g_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_toon_def_outline_g(double)));
        connect(_side_widget->toon_outline_b_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_toon_def_outline_b(double)));
        connect(_side_widget->toon_outline_a_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_toon_def_outline_a(double)));

        // parametric curves
        connect(_side_widget->curveComboBox, SIGNAL(currentIndexChanged(int)), _gl_widget, SLOT(setParametricCurveIndex(int)));
        connect(_side_widget->show_tangents_check_box, SIGNAL(toggled(bool)), _gl_widget, SLOT(setVisibilityOfTangents(bool)));
        connect(_side_widget->show_acceleration_vectors_check_box, SIGNAL(toggled(bool)), _gl_widget, SLOT(setVisibilityOfAccelerationVectors(bool)));
        connect(_side_widget->derivative_scale_double_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(setDerivativeScale(double)));
        connect(_side_widget->division_point_count_spin_box, SIGNAL(valueChanged(int)), _gl_widget, SLOT(setDivPointCount(int)));
        connect(_side_widget->resetButton_2, SIGNAL(pressed(void)), _gl_widget, SLOT(resetPcAttributes(void)));

        connect(_gl_widget, SIGNAL(derivativeVisibilitySignal(bool)), _side_widget->show_tangents_check_box, SLOT(setChecked(bool)));
        connect(_gl_widget, SIGNAL(derivativeVisibilitySignal(bool)), _side_widget->show_acceleration_vectors_check_box, SLOT(setChecked(bool)));
        connect(_gl_widget, SIGNAL(derivativeScaleSignal(double)), _side_widget->derivative_scale_double_spin_box, SLOT(setValue(double)));
        connect(_gl_widget, SIGNAL(divisionPointCountSignal(int)), _side_widget->division_point_count_spin_box, SLOT(setValue(int)));

        // parametric surfaces
        connect(_side_widget->surfaceComboBox, SIGNAL(currentIndexChanged(int)), _gl_widget, SLOT(setParametricSurfaceIndex(int)));
        connect(_side_widget->uDivPointCountSpinBox, SIGNAL(valueChanged(int)), _gl_widget, SLOT(setDivPointCountU(int)));
        connect(_side_widget->vDivPointCountSpinBox, SIGNAL(valueChanged(int)), _gl_widget, SLOT(setDivPointCountV(int)));
        connect(_side_widget->showTextureCheckBox, SIGNAL(toggled(bool)), _gl_widget, SLOT(setShowTexture(bool)));

        // cyclic curves
        connect(_side_widget->cc_cp_index_spin_box, SIGNAL(valueChanged(int)), _gl_widget, SLOT(setSelectedControlPoint(int)));
        connect(_side_widget->cc_trans_x_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(cc_cp_set_x(double)));
        connect(_side_widget->cc_trans_y_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(cc_cp_set_y(double)));
        connect(_side_widget->cc_trans_z_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(cc_cp_set_z(double)));
        connect(_side_widget->show_cc_check_box, SIGNAL(toggled(bool)), _gl_widget, SLOT(setCcVisibility(bool)));
        connect(_side_widget->show_cc_d1_check_box, SIGNAL(toggled(bool)), _gl_widget, SLOT(setCcD1Visibility(bool)));
        connect(_side_widget->show_cc_d2_check_box, SIGNAL(toggled(bool)), _gl_widget, SLOT(setCcD2Visibility(bool)));
        connect(_side_widget->show_ip_cc_check_box, SIGNAL(toggled(bool)), _gl_widget, SLOT(setIpCcVisibility(bool)));
        connect(_side_widget->show_ip_cc_d1_check_box, SIGNAL(toggled(bool)), _gl_widget, SLOT(setIpCcD1Visibility(bool)));
        connect(_side_widget->show_ip_cc_d2_check_box, SIGNAL(toggled(bool)), _gl_widget, SLOT(setIpCcD2Visibility(bool)));

        connect(_gl_widget, SIGNAL(cc_control_point_x_changed(double)), _side_widget->cc_trans_x_spin_box, SLOT(setValue(double)));
        connect(_gl_widget, SIGNAL(cc_control_point_y_changed(double)), _side_widget->cc_trans_y_spin_box, SLOT(setValue(double)));
        connect(_gl_widget, SIGNAL(cc_control_point_z_changed(double)), _side_widget->cc_trans_z_spin_box, SLOT(setValue(double)));

        // cubic bezier arc
        connect(_side_widget->arc_cp_index_spin_box, SIGNAL(valueChanged(int)), _gl_widget, SLOT(set_selected_cp_arc(int)));
        connect(_side_widget->arc_cp_x_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(arc_cp_set_x(double)));
        connect(_side_widget->arc_cp_y_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(arc_cp_set_y(double)));
        connect(_side_widget->arc_cp_z_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(arc_cp_set_z(double)));
        connect(_side_widget->arc_d1_check_box, SIGNAL(toggled(bool)), _gl_widget, SLOT(set_arc_d1_visibility(bool)));
        connect(_side_widget->arc_d2_check_box, SIGNAL(toggled(bool)), _gl_widget, SLOT(set_arc_d2_visibility(bool)));

        connect(_gl_widget, SIGNAL(arc_control_point_x_changed(double)), _side_widget->arc_cp_x_spin_box, SLOT(setValue(double)));
        connect(_gl_widget, SIGNAL(arc_control_point_y_changed(double)), _side_widget->arc_cp_y_spin_box, SLOT(setValue(double)));
        connect(_gl_widget, SIGNAL(arc_control_point_z_changed(double)), _side_widget->arc_cp_z_spin_box, SLOT(setValue(double)));

        // bicubic bezier patches
        connect(_side_widget->show_iso_u_check_box, SIGNAL(toggled(bool)), _gl_widget, SLOT(setBbPatchVisibility(bool)));
        connect(_side_widget->show_iso_v_check_box, SIGNAL(toggled(bool)), _gl_widget, SLOT(setIpBbPatchVisibility(bool)));
        connect(_side_widget->show_iso_d1_u_check_box, SIGNAL(toggled(bool)), _gl_widget, SLOT(setBbPatchD1UVisibility(bool)));
        connect(_side_widget->show_iso_d1_v_check_box, SIGNAL(toggled(bool)), _gl_widget, SLOT(setBbPatchD1VVisibility(bool)));
<<<<<<< HEAD
        //connect(_side_widget->shaderRadioButton, SIGNAL(toggled(bool)), _gl_widget, SLOT(setShader(bool)));
        //connect(_side_widget->lightRadioButton, SIGNAL(toggled(bool)), _gl_widget, SLOT(setLight(bool)));
=======
>>>>>>> c5f8844f41c97631922fcf107297892fc229c1b9
        connect(_side_widget->shaderComboBox, SIGNAL(currentIndexChanged(int)), _gl_widget, SLOT(setShaderType(int)));
        connect(_side_widget->directedCheckBox, SIGNAL(toggled(bool)), _gl_widget, SLOT(setDirectionalLight(bool)));
        connect(_side_widget->pointLikeCheckBox, SIGNAL(toggled(bool)), _gl_widget, SLOT(setReflectorLight(bool)));
        connect(_side_widget->reflectorCheckBox, SIGNAL(toggled(bool)), _gl_widget, SLOT(setPointLikeLight(bool)));
        connect(_side_widget->tabWidget, SIGNAL(tabBarClicked(int)), _gl_widget, SLOT(shaderOrLight(int)));

<<<<<<< HEAD
        connect(_side_widget->cp_row_spinBox, SIGNAL(valueChanged(int)), _gl_widget, SLOT(set_selected_cp_patch_row(int)));
        connect(_side_widget->cp_col_spinBox, SIGNAL(valueChanged(int)), _gl_widget, SLOT(set_selected_cp_patch_column(int)));
        connect(_side_widget->patch_cp_x_spinBox, SIGNAL(valueChanged(double)), _gl_widget, SLOT(patch_cp_set_x(double)));
        connect(_side_widget->patch_cp_y_spinBox, SIGNAL(valueChanged(double)), _gl_widget, SLOT(patch_cp_set_y(double)));
        connect(_side_widget->patch_cp_z_spinBox, SIGNAL(valueChanged(double)), _gl_widget, SLOT(patch_cp_set_z(double)));

        connect(_gl_widget, SIGNAL(patch_control_point_x_changed(double)), _side_widget->patch_cp_x_spinBox, SLOT(setValue(double)));
        connect(_gl_widget, SIGNAL(patch_control_point_y_changed(double)), _side_widget->patch_cp_y_spinBox, SLOT(setValue(double)));
        connect(_gl_widget, SIGNAL(patch_control_point_z_changed(double)), _side_widget->patch_cp_z_spinBox, SLOT(setValue(double)));

=======
        connect(_side_widget->bp_rl_scale_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_rl_scale(double)));
        connect(_side_widget->bp_rl_smoothing_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_rl_smoothing(double)));
        connect(_side_widget->bp_rl_shading_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_rl_shading(double)));

        connect(_side_widget->bp_toon_outline_r_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_toon_def_outline_r(double)));
        connect(_side_widget->bp_toon_outline_g_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_toon_def_outline_g(double)));
        connect(_side_widget->bp_toon_outline_b_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_toon_def_outline_b(double)));
        connect(_side_widget->bp_toon_outline_a_spin_box, SIGNAL(valueChanged(double)), _gl_widget, SLOT(set_toon_def_outline_a(double)));
>>>>>>> c5f8844f41c97631922fcf107297892fc229c1b9
        // change scene
        connect(_side_widget->toolBox, SIGNAL(currentChanged(int)), _gl_widget, SLOT(setID(int)));
    }

    //--------------------------------
    // implementation of private slots
    //--------------------------------
    void MainWindow::on_action_Quit_triggered()
    {
        qApp->exit(0);
    }

    void MainWindow::on_action_Save_curve_triggered()
    {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save composite curve"), "", tr("Composite curve (*.crv);;All files(*)"));
        if (fileName.isEmpty())
        {
            return;
        }
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly))
        {
            throw Exception("Could not open file.");
        }
        std::ofstream out(file.fileName().toLocal8Bit());
        _gl_widget->saveCurves(out);
    }

    void MainWindow::on_action_Load_curve_triggered()
    {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Load composite curve"), "", tr("Composite curve (*.crv);;All files(*)"));
        if (fileName.isEmpty())
        {
            return;
        }
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly))
        {
            throw Exception("Could not open file.");
        }
        std::ifstream in(file.fileName().toLocal8Bit());
        _gl_widget->loadCurves(in);
    }

    void MainWindow::on_action_Save_surface_triggered()
    {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save composite surface"), "", tr("Composite surface (*.surf);;All files(*)"));
        if (fileName.isEmpty())
        {
            return;
        }
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly))
        {
            throw Exception("Could not open file.");
        }
        std::ofstream out(file.fileName().toLocal8Bit());
        _gl_widget->saveSurfaces(out);
    }

    void MainWindow::on_action_Load_surface_triggered()
    {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Load composite surface"), "", tr("Composite surface (*.surf);;All files(*)"));
        if (fileName.isEmpty())
        {
            return;
        }
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly))
        {
            throw Exception("Could not open file.");
        }
        std::ifstream in(file.fileName().toLocal8Bit());
        _gl_widget->loadSurfaces(in);
    }
}
