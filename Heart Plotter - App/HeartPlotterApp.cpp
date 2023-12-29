#include "HeartPlotterApp.h"
#include "qcustomplot/qcustomplot.h"

HeartPlotterApp::HeartPlotterApp(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    // Create a QCustomPlot widget
    QCustomPlot *customPlot = new QCustomPlot(this);

    // Enable scroll bars for the plot's viewport
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    // Labels
    customPlot->xAxis->setLabel("X Axis");
    customPlot->yAxis->setLabel("Y Axis");

    // Legend
    customPlot->legend->setVisible(true);
    QFont legendFont = font();  
    legendFont.setPointSize(9); 
    customPlot->legend->setFont(legendFont);
    customPlot->legend->setBrush(QBrush(QColor(255, 255, 255, 230)));

    // Create a graph 0 and set data
    QVector<double> xData, yData; // Add your data here
    xData << 1 << 2 << 3; yData << 1 << 4 << 9;

    customPlot->addGraph();
    customPlot->graph(0)->setPen(QPen(Qt::blue)); 
    customPlot->graph(0)->setData(xData, yData);
    customPlot->graph(0)->setName("Data");

    // Create a graph 1 and set data
    customPlot->addGraph();
    customPlot->graph(1)->setPen(QPen(Qt::red)); 
    customPlot->graph(1)->setLineStyle(QCPGraph::lsNone);
    customPlot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 7));
    customPlot->graph(1)->setData(xData, yData);
    customPlot->graph(1)->setName("Data markers");

    // Create a table widget
    QTableWidget* dataTable = new QTableWidget(this);
    dataTable->setColumnCount(2); // Two columns: one for X data, one for Y data
    dataTable->setRowCount(xData.size()); // Number of rows is equal to the size of xData

    // Set headers for the table
    QStringList headers;
    headers << "X Data" << "Y Data";
    dataTable->setHorizontalHeaderLabels(headers);

    // Populate the table with data
    for (int i = 0; i < xData.size(); ++i) {
        QTableWidgetItem* itemX = new QTableWidgetItem(QString::number(xData[i]));
        QTableWidgetItem* itemY = new QTableWidgetItem(QString::number(yData[i]));

        dataTable->setItem(i, 0, itemX);
        dataTable->setItem(i, 1, itemY);
    }

    // Rescale axes and replot
    customPlot->rescaleAxes();
    customPlot->replot();

    // Add customPlot to the UI
    QWidget *plot_container =  ui.plotContainer;
    QVBoxLayout *plot_layout = new QVBoxLayout(plot_container);
    plot_layout->addWidget(customPlot);
    plot_layout->addWidget(dataTable);

    // Ensure the layout updates when the window is resized
    plot_layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    // Adjust layout and size policy
    plot_layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    customPlot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

}

HeartPlotterApp::~HeartPlotterApp() = default;
