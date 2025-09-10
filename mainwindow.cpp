#include <iostream>
#include "mainwindow.h"
#include "bioscanner.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
      cout << "Dentro do mainWindow" << endl;
      QWidget *centralWidget = new QWidget(this);
      imageLabel1 = new QLabel();
      imageLabel2 = new QLabel();
      textLabel1 = new QLabel("Leitura 1");
      textLabel2 = new QLabel("Leitura 2");
      button1 = new QPushButton("Clique para ler");
      button2 = new QPushButton("Clique para ler");
      userLabel = new QLabel("Usuário");
      userLineEdit = new QLineEdit();
      emailLabel = new QLabel("Email");
      emailLineEdit = new QLineEdit();
      button3 = new QPushButton("Cadastrar");
      button4 = new QPushButton("Cadastrar senha se digital falhar");
      image1 = new QPixmap("semImagem.png");
      image2 = new QPixmap("semImagem.png");
      timer = new QTimer(this);
      imageLabel1->setPixmap(image1->scaled(132,100, Qt::KeepAspectRatio));
      imageLabel2->setPixmap(image2->scaled(132,100, Qt::KeepAspectRatio));
      
      line = new QFrame();
      line->setFrameShape(QFrame::HLine);
      line->setFrameShadow(QFrame::Sunken);
    
      layout = new QGridLayout();
      layout->addWidget(imageLabel1, 0, 0);
      layout->addWidget(imageLabel2, 0, 1);
    
      layout->addWidget(textLabel1, 1, 0);
      layout->addWidget(textLabel2, 1, 1);
    
      layout->addWidget(button1, 2, 0);
      layout->addWidget(button2, 2, 1);
    
      layout->addWidget(userLabel, 3, 0);
      layout->addWidget(userLineEdit, 3, 1);
    
      layout->addWidget(emailLabel, 4, 0);
      layout->addWidget(emailLineEdit, 4, 1);
      
      layout->addWidget(button3, 5, 0, 1, 2);
    
      layout->addWidget(line,6,0,1,2);
    
      
    
      layout->addWidget(button4, 7, 0, 1, 2);
    
      //setLayout(layout);
      centralWidget->setLayout(layout);
      setCentralWidget(centralWidget);
      
      
      connect(button4, &QPushButton::clicked, this, &MainWindow::openPasswordDialog);
      connect(button1, &QPushButton::clicked, this, &MainWindow::scan);
      connect(button2, &QPushButton::clicked, this, &MainWindow::scan);
      connect(timer, &QTimer::timeout, this, &MainWindow::cancelScan);
}

void MainWindow::scan() {
        QPushButton *clicado = qobject_cast<QPushButton*>(sender());
        int button = 0;
        if (clicado == button1) {
           cout << "Leitura na esquerda" << endl;
           button = 1;
        }
        else {
           cout << "Leitura na direita" << endl;
           button = 2;
        }
        button1->setEnabled(false);
        button2->setEnabled(false);
        timer->setSingleShot(true);
        timer->setInterval(10000);
        timer->start();
  	main2(button);
  	button1->setEnabled(true);
  	button2->setEnabled(true);
  	timer->stop();
}

void MainWindow::cancelScan() {
       cout << "Timeout ocorreu no Qt " << endl;
       cancelScanning();
}

void MainWindow::openPasswordDialog() {
        PasswordDialog dialog(this);
        if(dialog.exec() == QDialog::Accepted) {
          QString password = dialog.getPassword();
          QString confirmPassword = dialog.getConfirmPassword();
          cout << "Aceitou senhas" << endl;
        }
        else {
          cout << "Rejeitou senhas" << endl;
        }
}


