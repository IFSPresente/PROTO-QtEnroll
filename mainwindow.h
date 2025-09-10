#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QGridLayout>
#include <QWidget>
#include <QPixmap>
#include <QTimer>
#include "passworddialog.h"

class MainWindow : public QMainWindow {
   Q_OBJECT
   
   public:
     explicit MainWindow(QWidget *parent = nullptr);
     
   private: 
     QTimer *timer;
     QLabel *imageLabel1;
     QLabel *imageLabel2;
     QLabel *textLabel1;
     QLabel *textLabel2;
     QPushButton *button1;
     QPushButton *button2;
     QLabel *userLabel;
     QLineEdit *userLineEdit;
     QLabel *emailLabel;
     QLineEdit *emailLineEdit;
     QPushButton *button3;
     QPushButton *button4;
     QPixmap *image1;
     QPixmap *image2;
     QFrame *line;
     QGridLayout *layout; 
     
   private slots:
     void openPasswordDialog();
     void scan();
     void cancelScan();
};

#endif //MAINWINDOW_H

