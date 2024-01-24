#include "HeartPlotterApp.h"
#include "qcustomplot/qcustomplot.h"
#include "Input.h"
#include <ECG_Service/ECGService.h>

HeartPlotterApp::HeartPlotterApp(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    QStringList signalList = QStringList() << "100" << "100s" << "101" << "102" << "103" << "104" << "105" << "106" << "107" << "108" << "109" << "111" << "112" << "113" << "114" << "115" << "116" << "117" << "118" << "119" << "121" << "122" << "123" << "124" << "200" << "201" << "202" << "203" << "205" << "207" << "208" << "209" << "210" << "212" << "213" << "214" << "215" << "217" << "219" << "220" << "221" << "222" << "223" << "228" << "230" << "231" << "232" << "233" << "234";

    QComboBox* comboBox = ui.signalsBox;
    
    for (const QString &file : signalList) {
        comboBox->addItem(file);
    }

    ui.plot->plotLayout()->insertRow(0);
    connect(ui.signalsBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onSignalsBoxSelectionChanged(int)));
    ui.signalsBox->setCurrentIndex(1);
}

HeartPlotterApp::~HeartPlotterApp() = default;

void HeartPlotterApp::onSignalsBoxSelectionChanged(int index)
{
    qDebug() << "Selected item index: " << index;
    std::string selectedText = ui.signalsBox->itemText(index).toStdString();

    refresh_plot_data(selectedText);
}

void HeartPlotterApp::refresh_plot_data(std::string signal_name)
{
    const std::string x_axis = "Time";
    const std::string y_axis = "Amplitude";

    // Loading the input signal
    const auto input = new Input();
    std::vector<DataPoint> loaded_signal;
    if (input->check_availability(signal_name))
    {
         loaded_signal = input->get_preprocessed_data(signal_name);
    }

    ECGService service = ECGService();
    service.perform_ecg_analysis(loaded_signal);
    Parameters params = service.get_parameters();

    // Get a QCustomPlot widget
    QCustomPlot* customPlot = ui.plot;
    customPlot->clearGraphs();

    // Enable scroll bars for the plot's viewport
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    // Labels
    customPlot->xAxis->setLabel(x_axis.data());
    customPlot->yAxis->setLabel(y_axis.data());

    // Set plot title
    QCPTextElement* title = new QCPTextElement(customPlot, "ECG plot of a signal", QFont("sans", 12, QFont::Bold));
    customPlot->plotLayout()->addElement(0, 0, title);

    // Legend
    customPlot->legend->setVisible(true);
    QFont legendFont = font();
    legendFont.setPointSize(9);
    customPlot->legend->setFont(legendFont);
    customPlot->legend->setBrush(QBrush(QColor(255, 255, 255, 230)));

    // Create a graph 0 and set data
    QVector<double> xData, yData; 
    for (const auto& point : loaded_signal)
    {
        xData.push_back(point.x);
        yData.push_back(point.y);
    }

    customPlot->addGraph();
    customPlot->graph(0)->setPen(QPen(Qt::blue));
    customPlot->graph(0)->setData(xData, yData);
    customPlot->graph(0)->setName(signal_name.data());

    
    /*customPlot->addGraph();
    customPlot->graph(1)->setPen(QPen(Qt::red));
    customPlot->graph(1)->setLineStyle(QCPGraph::lsNone);
    customPlot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 7));
    customPlot->graph(1)->setData(xData, yData);
    customPlot->graph(1)->setName("Data markers");*/
    

    // Get and populate the table widget
    QTableWidget* dataTable = ui.table;
    dataTable->clear();

    dataTable->setColumnCount(2);
    dataTable->setRowCount(15);

    // Set headers for the table
    QStringList headers;
    headers << "Parameter" << "Value";
    dataTable->setHorizontalHeaderLabels(headers);

    // List of parameter names
    QStringList paramNames = {
        "RR_mean", "SDNN", "RMSSD", "NN50", "pNN50", "ULF", "VLF", "LF", 
        "HF", "LFHF", "TP", "TiNN", "triangular_index", "SD_1", "SD_2"
    };

    double paramValues[15] = {
        params.RR_mean, params.SDNN, params.RMSSD, params.NN50, params.pNN50, 
        params.ULF, params.VLF, params.LF, params.HF, params.LFHF, params.TP, 
        params.TiNN, params.triangular_index, params.SD_1, params.SD_2
    };

    // Populate the table with data
    for (int i = 0; i < 15; ++i) {
        QTableWidgetItem* itemName = new QTableWidgetItem(paramNames[i]);
        QTableWidgetItem* itemValue = new QTableWidgetItem(QString::number(paramValues[i]));

        dataTable->setItem(i, 0, itemName);
        dataTable->setItem(i, 1, itemValue);
    }

    // Rescale axes and replot
    customPlot->rescaleAxes();
    customPlot->replot();

    QCPDocumentObject* plotObjectHandler = new QCPDocumentObject(this);
    pnt = &ui;
    pnt->textEdit->document()->documentLayout()->registerHandler(QCPDocumentObject::PlotTextFormat, plotObjectHandler);
    pnt->plot = customPlot;
}

void HeartPlotterApp::on_actionInsert_Plot_triggered()
{
    pnt = &ui;
    QTextCursor cursor = pnt->textEdit->textCursor();

    // insert the current plot at the cursor position. QCPDocumentObject::generatePlotFormat creates a
    // vectorized snapshot of the passed plot (with the specified width and height) which gets inserted
    // into the text document.
    double width = 720;
    double height = 360;
    cursor.insertText(QString(QChar::ObjectReplacementCharacter), QCPDocumentObject::generatePlotFormat(pnt->plot, width, height));

    pnt->textEdit->setTextCursor(cursor);
}


void HeartPlotterApp::on_actionSave_Document_triggered()
{
    pnt = &ui;
    QString fileName = QFileDialog::getSaveFileName(this, "Saving...", qApp->applicationDirPath(), "*.pdf");
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

