// linegr.cpp

#include <gfx/multigraph.h>
#include <gfx/linegraph.h>
#include <gfx/traceinfo.h>
#include <QApplication>
#include <math.h>
#include <QWidget>

LineGraph *makeGraph(QWidget *parent, double phase) {
  int N=10000;
  double *yy = new double[N];
  for (int n=0; n<N; n++)
    yy[n] = cos(n*5*3.1415/N + phase);

  TraceInfo::DataPtr yy_ptr;
  yy_ptr.dp_double = yy;
  
  TraceInfo ti(TraceInfo::dataDouble);
  ti.setData(0,5*3.1415/N,yy_ptr,N);

  double *zz = new double[N];
  for (int n=0; n<N; n++)
    zz[n] = sin(n*5*3.1415/N+phase);

  TraceInfo::DataPtr zz_ptr;
  zz_ptr.dp_double = zz;
  
  TraceInfo tiz(TraceInfo::dataDouble);
  tiz.setData(0,5*3.1415/N,zz_ptr,N);

  tiz.setScaleFactor(1.5);

  LineGraph *qlg = new LineGraph(parent);
  qlg->addTrace("A",&ti);
  qlg->addTrace("B",&tiz);

  qlg->autoSetXRange();
  qlg->autoSetYRange();
  qlg->setXTicks();
  qlg->setYTicks();
    
  qlg->setTraceLabel("A","Aaa");
  qlg->setTraceLabel("B","Bbb");
  qlg->setTracePen("B",QColor("red"));

  qlg->setXLabel("Time (s)");
  qlg->setYLabel("Amp (V)");

  return qlg;
}

int main(int argc, char **argv) {
  QApplication app(argc, argv);

  MultiGraph mlg(0);
  for (int n=0; n<4; n++) {
    LineGraph *lg = makeGraph(&mlg,n*3.14/6);
    mlg.addGraph(QString::number(n),lg,n==1 || n==3);
    if (n==1 || n==3) {
      lg->showXTickLabels(false);
      lg->showYTickLabels(false);
      lg->setTraceLabel("A","");
      lg->setTraceLabel("B","");
      lg->setXLabel("");
      lg->setYLabel("");
    } 

  }
  mlg.show();

  return app.exec();
}
