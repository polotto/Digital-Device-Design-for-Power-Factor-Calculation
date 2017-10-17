/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "console.h"
#include "settingsdialog.h"

#include <QMessageBox>
#include <QtSerialPort/QSerialPort>

//! [0]
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
//! [0]
    ui->setupUi(this);
    console = new Console;
    console->setEnabled(false);

    //ui->tabWidget->setCurrentIndex(0);
    //ui->tabWidget->setCurrentWidget(tab1Contents);

    //QTabWidget* myTabWidget = new QTabWidget();
    //QWidget* tab1 = new QWidget();
    //QWidget* tab1Contents = new QWidget( console );

    // [1] Setup the first tab.
    ui->tabWidget->addTab( console, "Terminal" );
    //setCentralWidget(console);
//! [1]
    serial = new QSerialPort(this);
//! [1]
    settings = new SettingsDialog;

    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionQuit->setEnabled(true);
    ui->actionConfigure->setEnabled(true);

    initActionsConnections();

    connect(serial, SIGNAL(error(QSerialPort::SerialPortError)), this,
            SLOT(handleError(QSerialPort::SerialPortError)));

//! [2]
    connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
//! [2]
    connect(console, SIGNAL(getData(QByteArray)), this, SLOT(writeData(QByteArray)));
//! [3]
}
//! [3]

MainWindow::~MainWindow()
{
    delete settings;
    delete ui;
}

//! [4]
void MainWindow::openSerialPort()
{
    SettingsDialog::Settings p = settings->settings();
    serial->setPortName(p.name);
    serial->setBaudRate(p.baudRate);
    serial->setDataBits(p.dataBits);
    serial->setParity(p.parity);
    serial->setStopBits(p.stopBits);
    serial->setFlowControl(p.flowControl);
    if (serial->open(QIODevice::ReadWrite)) {
            console->setEnabled(true);
            console->setLocalEchoEnabled(p.localEchoEnabled);
            ui->actionConnect->setEnabled(false);
            ui->actionDisconnect->setEnabled(true);
            ui->actionConfigure->setEnabled(false);
            ui->statusBar->showMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
                                       .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                                       .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
    } else {
        QMessageBox::critical(this, tr("Error"), serial->errorString());

        ui->statusBar->showMessage(tr("Open error"));
    }
}
//! [4]

//! [5]
void MainWindow::closeSerialPort()
{
    if (serial->isOpen())
        serial->close();
    console->setEnabled(false);
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionConfigure->setEnabled(true);
    ui->statusBar->showMessage(tr("Disconnected"));
}
//! [5]

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Simple Terminal"),
                       tr("The <b>Simple Terminal</b> example demonstrates how to "
                          "use the Qt Serial Port module in modern GUI applications "
                          "using Qt, with a menu bar, toolbars, and a status bar."));
}

//! [6]
void MainWindow::writeData(const QByteArray &data)
{
    serial->write(data);
}
//! [6]

//! [7]
void MainWindow::readData()
{
    QByteArray data = serial->readAll();
    console->putData(data);
    QString s_data = data.trimmed();
    list_data.append(s_data);
    if(s_data.contains("*")){
        data_join =list_data.join("");
        serial->clear();
        list_data.clear();
        QStringList aux;
        //qDebug()<<data_join;
        list_data=data_join.split(";");
        //qDebug()<<list_data;
        unsigned int i=0;
        aux=list_data[i].split(":");
        ui->lbl_vrms->setText(aux[1]);

        aux=list_data[++i].split(":");
        ui->lbl_irms->setText(aux[1]);

        aux=list_data[++i].split(":");
        ui->lbl_p->setText(aux[1]);

        aux=list_data[++i].split(":");
        ui->lbl_s->setText(aux[1]);

        aux=list_data[++i].split(":");
        ui->lbl_q->setText(aux[1]);

        aux=list_data[++i].split(":");
        ui->lbl_d->setText(aux[1]);

        aux=list_data[++i].split(":");
        ui->lbl_sproj->setText(aux[1]);

        aux=list_data[++i].split(":");
        ui->lbl_fp->setText(aux[1]);

        aux=list_data[++i].split(":");
        ui->lbl_fpcos->setText(aux[1]);

        aux=list_data[++i].split(":");
        ui->lbl_vthd->setText(aux[1]);

        aux=list_data[++i].split(":");
        ui->lbl_ithd->setText(aux[1]);

        aux=list_data[++i].split(":");
        ui->lbl_theta->setText(aux[1]);

        aux=list_data[++i].split(":");
        ui->lbl_costheta->setText(aux[1]);

        aux=list_data[++i].split(":");
        ui->lbl_lamb->setText(aux[1]);

        aux=list_data[++i].split(":");
        ui->lbl_coslamb->setText(aux[1]);

        aux=list_data[++i].split(":");
        ui->lbl_phi->setText(aux[1]);

        aux=list_data[++i].split(":");
        ui->lbl_cosphi->setText(aux[1]);

        QStringList t=list_data[++i].split(":");
        QStringList V=list_data[++i].split(":");
        QStringList I=list_data[++i].split(":");

        V=V[1].split(",");
        I=I[1].split(",");
        t=t[1].split(",");
        /*qDebug()<<t
        qDebug()<<V;
        qDebug()<<I;*/
        plot(t, V, I);
        list_data.clear();
        V.clear();
        I.clear();
        t.clear();
    }
    //qDebug()<<list_data;
}
//! [7]
void MainWindow::plot(QStringList t, QStringList V, QStringList I)
{
    QVector<double> t_d(t.length()), V_d(t.length()), I_d(t.length());
    QString aux;
    for (int i=0; i<t.length(); ++i)
    {
      aux=t[i];
      t_d[i] = aux.toDouble();
      aux=V[i];
      V_d[i] = aux.toDouble(); // exponentially decaying cosine
      aux=I[i];
      I_d[i] = aux.toDouble(); // exponential envelope
    }
    /*qDebug()<<t_d;
    qDebug()<<V_d;
    qDebug()<<I_d;*/


    // add two new graphs and set their look:
    //ui->customPlot->clearGraphs();
    ui->customPlot->addGraph();
    ui->customPlot->graph(0)->setPen(QPen(Qt::blue)); // line color blue for first graph
    ui->customPlot->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20))); // first graph will be filled with translucent blue
    //ui->customPlot->addGraph();
    //ui->customPlot->graph(1)->setPen(QPen(Qt::red)); // line color red for second graph
    // generate some points of data (y0 for first, y1 for second graph):
    // configure right and top axis to show ticks but no labels:
    // (see QCPAxisRect::setupFullAxesBox for a quicker method to do this)
    ui->customPlot->xAxis2->setVisible(true);
    ui->customPlot->xAxis2->setTickLabels(false);
    ui->customPlot->yAxis2->setVisible(true);
    ui->customPlot->yAxis2->setTickLabels(false);
    // make left and bottom axes always transfer their ranges to right and top axes:
    connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));
    // pass data points to graphs:
    ui->customPlot->graph(0)->setData(t_d, V_d);
    //ui->customPlot->graph(1)->setData(t_d, I_d);
    // let the ranges scale themselves so graph 0 fits perfectly in the visible area:
    ui->customPlot->graph(0)->rescaleAxes();
    // same thing for graph 1, but only enlarge ranges (in case graph 1 is smaller than graph 0):
    //ui->customPlot->graph(1)->rescaleAxes(true);
    // Note: we could have also just called customPlot->rescaleAxes(); instead
    // Allow user to drag axis ranges with mouse, zoom with mouse wheel and select graphs by clicking:
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->customPlot->replot();

    // add two new graphs and set their look:
    //ui->customPlot->clearGraphs();
    ui->customPlot2->addGraph();
    ui->customPlot2->graph(0)->setPen(QPen(Qt::red)); // line color blue for first graph
    ui->customPlot2->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20))); // first graph will be filled with translucent blue
    //ui->customPlot->addGraph();
    //ui->customPlot->graph(1)->setPen(QPen(Qt::red)); // line color red for second graph
    // generate some points of data (y0 for first, y1 for second graph):
    // configure right and top axis to show ticks but no labels:
    // (see QCPAxisRect::setupFullAxesBox for a quicker method to do this)
    ui->customPlot2->xAxis2->setVisible(true);
    ui->customPlot2->xAxis2->setTickLabels(false);
    ui->customPlot2->yAxis2->setVisible(true);
    ui->customPlot2->yAxis2->setTickLabels(false);
    // make left and bottom axes always transfer their ranges to right and top axes:
    connect(ui->customPlot2->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot2->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->customPlot2->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot2->yAxis2, SLOT(setRange(QCPRange)));
    // pass data points to graphs:
    //ui->customPlot2->graph(0)->setData(t_d, V_d);
    ui->customPlot2->graph(0)->setData(t_d, I_d);
    // let the ranges scale themselves so graph 0 fits perfectly in the visible area:
    ui->customPlot2->graph(0)->rescaleAxes();
    // same thing for graph 1, but only enlarge ranges (in case graph 1 is smaller than graph 0):
    //ui->customPlot2->graph(1)->rescaleAxes(true);
    // Note: we could have also just called customPlot->rescaleAxes(); instead
    // Allow user to drag axis ranges with mouse, zoom with mouse wheel and select graphs by clicking:
    ui->customPlot2->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->customPlot2->replot();
}

//! [8]
void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), serial->errorString());
        closeSerialPort();
    }
}
//! [8]

void MainWindow::initActionsConnections()
{
    connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(openSerialPort()));
    connect(ui->actionDisconnect, SIGNAL(triggered()), this, SLOT(closeSerialPort()));
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionConfigure, SIGNAL(triggered()), settings, SLOT(show()));
    connect(ui->actionClear, SIGNAL(triggered()), console, SLOT(clear()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui->actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}
