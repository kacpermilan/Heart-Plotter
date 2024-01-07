#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_HeartPlotterApp.h"

class HeartPlotterApp : public QMainWindow
{
    Q_OBJECT

public:
    HeartPlotterApp(QWidget *parent = nullptr);
    ~HeartPlotterApp();

private:
    Ui::HeartPlotterAppClass ui;
};
