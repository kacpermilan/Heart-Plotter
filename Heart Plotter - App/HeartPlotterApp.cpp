#include "HeartPlotterApp.h"
#include "qcustomplot/qcustomplot.h"

HeartPlotterApp::HeartPlotterApp(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    // Create a QCustomPlot widget
    QCustomPlot *customPlot = new QCustomPlot(this);

    // Set up axes, for example
    customPlot->xAxis->setLabel("X Axis");
    customPlot->yAxis->setLabel("Y Axis");

    // Create a graph and set data
    QVector<double> xData, yData; // Add your data here
    xData << 1 << 2 << 3; yData << 1 << 4 << 9;

    customPlot->addGraph();
    customPlot->graph(0)->setData(xData, yData);

    // Rescale axes and replot
    customPlot->rescaleAxes();
    customPlot->replot();

    // Add customPlot to the UI
    QWidget *plot_container =  ui.plotContainer;
    QVBoxLayout *plot_layout = new QVBoxLayout(plot_container);
    plot_layout->addWidget(customPlot);
}

HeartPlotterApp::~HeartPlotterApp() = default;
