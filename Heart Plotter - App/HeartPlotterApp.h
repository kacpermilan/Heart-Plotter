#pragma once

#ifndef HEARTPLOTTERAPP_H
#define HEARTPLOTTERAPP_H

#include <QMainWindow>
#include <QTextDocument>
#include <QFileDialog>
#include "qcpdocumentobject.h"
#include "ui_HeartPlotterApp.h"

namespace Ui {
    class HeartPlotterAppClass;
}

class HeartPlotterApp : public QMainWindow
{
    Q_OBJECT

public:
    explicit HeartPlotterApp(QWidget *parent = 0);
    ~HeartPlotterApp();

    //void setupPlot();

private slots:
    
    void on_actionSave_Document_triggered();

private:
    Ui::HeartPlotterAppClass ui;
    Ui::HeartPlotterAppClass* pnt;
    
    
};
#endif // MAINWINDOW_H