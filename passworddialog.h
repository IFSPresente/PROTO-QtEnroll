#ifndef PASSWORDDIALOG_H
#define PASSWORDDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QFormLayout>

class PasswordDialog: public QDialog {
  Q_OBJECT
  
  public: 
    explicit PasswordDialog(QWidget *parent = nullptr);
    
    QString getPassword() const;
    QString getConfirmPassword() const;
    
  private:
    QLabel *passwordLabel;
    QLabel *confirPasswordLabel;
    QLineEdit *passwordEdit;
    QLineEdit *confirmPasswordEdit;
    QPushButton *submitButton;
    
  private slots:
    void onSubmitClicked();
  
};


#endif //PASSWORDDIALOG_H
