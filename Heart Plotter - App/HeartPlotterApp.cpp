#include "HeartPlotterApp.h"
#include "qcustomplot/qcustomplot.h"
#include "Input.h"

//QCustomPlot* customPlot;

HeartPlotterApp::HeartPlotterApp(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    const auto input = new Input();
    std::vector<DataPoint> loaded_signal;
    if (input->check_availability("100s"))
    {
         loaded_signal = input->get_preprocessed_data("100s");
    }

    // Create a QCustomPlot widget
    QCustomPlot* customPlot = new QCustomPlot(this);

    // Enable scroll bars for the plot's viewport
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    // Labels
    customPlot->xAxis->setLabel("X Axis");
    customPlot->yAxis->setLabel("Y Axis");

    // Set plot title
    customPlot->plotLayout()->insertRow(0); // Insert a new row for the title
    QCPTextElement* title = new QCPTextElement(customPlot, "Plot title", QFont("sans", 12, QFont::Bold));
    customPlot->plotLayout()->addElement(0, 0, title);

    // Legend
    customPlot->legend->setVisible(true);
    QFont legendFont = font();
    legendFont.setPointSize(9);
    customPlot->legend->setFont(legendFont);
    customPlot->legend->setBrush(QBrush(QColor(255, 255, 255, 230)));

    // Create a graph 0 and set data
    QVector<double> xData, yData; 
    xData << 1 << 2 << 3;
    yData << 1 << 4 << 9;

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

    // QTextEdit* textEdit = new QTextEdit(this);

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

    /*
    // Take a screenshot of the QCustomPlot widget
    QPixmap pixmap = customPlot->toPixmap();

    // Extract colors and create an RGB table from the screenshot
    std::vector<QColor> rgbTable;
    for (int y = 0; y < pixmap.height(); ++y) {
        for (int x = 0; x < pixmap.width(); ++x) {
            QColor color = pixmap.toImage().pixelColor(x, y);
            rgbTable.push_back(color);
        }
    }

    // Show RGB values in a QMessageBox
    QString rgbValues;
    for (const QColor& color : rgbTable) {
        rgbValues += QString("RGB values: %1, %2, %3\n")
            .arg(color.red())
            .arg(color.green())
            .arg(color.blue());
    }

    QMessageBox::information(this, "RGB Values", rgbValues);
    */

    // Rescale axes and replot
    customPlot->rescaleAxes();
    customPlot->replot();

    // Create a main layout
    QWidget *plot_container =  ui.plotContainer;
    QVBoxLayout *plot_layout = new QVBoxLayout(plot_container);

    // Add the QCustomPlot and QTableWidget to the main layout
    plot_layout->addWidget(customPlot);
    plot_layout->addWidget(dataTable);
    //plot_layout->addWidget(textEdit);

    // Set stretch factors to control how the available space is distributed
    plot_layout->setStretchFactor(customPlot, 2);  // Adjust the stretch factors as needed
    plot_layout->setStretchFactor(dataTable, 1);
    //plot_layout->setStretchFactor(textEdit, 1);

    // Set the main layout for the central widget
    //QWidget* centralWidget = new QWidget(this);
    //centralWidget->setLayout(plot_layout);
    //setCentralWidget(centralWidget);

    QCPDocumentObject* plotObjectHandler = new QCPDocumentObject(this);
    pnt = &ui;
    pnt->textEdit->document()->documentLayout()->registerHandler(QCPDocumentObject::PlotTextFormat, plotObjectHandler);
    pnt->plotContainer = customPlot;
}

HeartPlotterApp::~HeartPlotterApp() = default;

void HeartPlotterApp::on_actionInsert_Plot_triggered()
{
    pnt = &ui;
    QTextCursor cursor = pnt->textEdit->textCursor();

    // insert the current plot at the cursor position. QCPDocumentObject::generatePlotFormat creates a
    // vectorized snapshot of the passed plot (with the specified width and height) which gets inserted
    // into the text document.
    double width = 720;
    double height = 360;
    cursor.insertText(QString(QChar::ObjectReplacementCharacter), QCPDocumentObject::generatePlotFormat(pnt->plotContainer, width, height));

    pnt->textEdit->setTextCursor(cursor);
}


void HeartPlotterApp::on_actionSave_Document_triggered()
{
    pnt = &ui;
    QString fileName = QFileDialog::getSaveFileName(this, "Zapisywanie", qApp->applicationDirPath(), "*.pdf");
    if (!fileName.isEmpty())
    {
        QPrinter printer;
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(fileName);
        QMargins pageMargins(20, 20, 20, 20);
#if QT_VERSION < QT_VERSION_CHECK(5, 3, 0)
        printer.setFullPage(false);
        printer.setPaperSize(QPrinter::A4);
        printer.setOrientation(QPrinter::Portrait);
        printer.setPageMargins(pageMargins.left(), pageMargins.top(), pageMargins.right(), pageMargins.bottom(), QPrinter::Millimeter);
#else
        QPageLayout pageLayout;
        pageLayout.setMode(QPageLayout::StandardMode);
        pageLayout.setOrientation(QPageLayout::Portrait);
        pageLayout.setPageSize(QPageSize(QPageSize::A4));
        pageLayout.setUnits(QPageLayout::Millimeter);
        pageLayout.setMargins(QMarginsF(pageMargins));
        printer.setPageLayout(pageLayout);
#endif

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        pnt->textEdit->document()->setPageSize(printer.pageRect().size());
#else
        auto sizem = printer.resolution();
        auto pixelsm = printer.pageLayout().paintRectPixels(sizem).size();
        auto textem = pnt->textEdit;
        auto documenta = textem -> document();
        documenta->setPageSize(pixelsm);
#endif
        auto q = pnt->textEdit;
        auto w = q->document();
        w->print(&printer);
    }
}

