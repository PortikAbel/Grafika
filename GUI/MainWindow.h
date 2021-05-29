#pragma once

#include <iostream>
#include <fstream>
#include <QMainWindow>
#include "ui_MainWindow.h"
#include "SideWidget.h"
#include "GLWidget.h"
#include <QScrollArea>
#include <QFileDialog>

namespace cagd
{
    class MainWindow: public QMainWindow, public Ui::MainWindow
    {
        Q_OBJECT

    private:
        SideWidget  *_side_widget;      // pointer to our side widget
        GLWidget    *_gl_widget;        // pointer to our rendering OpenGL widget

        QScrollArea *_scroll_area;      // provides a scrolling view onto our side widget

    public:
        // special and default constructor
        MainWindow(QWidget *parent = 0);

    private slots:
        // private event handling methods/slots
        void on_action_Quit_triggered();
        void on_action_Save_curve_triggered();
        void on_action_Load_curve_triggered();
        void on_action_Save_surface_triggered();
        void on_action_Load_surface_triggered();
    };
}
