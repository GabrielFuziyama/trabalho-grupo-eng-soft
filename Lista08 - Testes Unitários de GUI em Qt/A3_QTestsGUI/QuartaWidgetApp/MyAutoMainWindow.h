#if !defined( MyAutoMainWindow_H )
#define MyAutoMainWindow_H 

#include <QMainWindow>

#include "ui_CalculadoraWnd.h"
class MyAutoMainWindow : public QMainWindow, private Ui::MainWindow
{                                 // relacionamento: é um
     Q_OBJECT                     
 public:
     MyAutoMainWindow(QWidget *parent = 0);
 public slots:
     void on_execButton_clicked( void );
};

#endif
