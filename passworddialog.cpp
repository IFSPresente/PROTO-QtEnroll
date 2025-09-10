#include <QIntValidator>
#include "passworddialog.h"

PasswordDialog::PasswordDialog(QWidget *parent) : QDialog(parent) {
   setWindowTitle("Inserir Senha");
    
   passwordEdit = new QLineEdit(this);
   confirmPasswordEdit = new QLineEdit(this);
   passwordEdit->setEchoMode(QLineEdit::Password);
   confirmPasswordEdit->setEchoMode(QLineEdit::Password);
   QIntValidator *validator = new QIntValidator(0,99999999, this);
   passwordEdit->setValidator(validator);
   confirmPasswordEdit->setValidator(validator);
    
   QFormLayout *formLayout = new QFormLayout();
   formLayout->addRow("Senha:", passwordEdit);
   formLayout->addRow("Confirmar Senha:", confirmPasswordEdit);
    
   QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    
   connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
   connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    
   formLayout->addWidget(buttonBox); 
   //QVBoxLayout *layout = new QVBoxLayout();
   //layout->addLayout(formLayout);
   //layout->addWidget(buttonBox);
    
   setLayout(formLayout);
}

QString PasswordDialog::getPassword() const {
  return passwordEdit->text();
}

QString PasswordDialog::getConfirmPassword() const {
  return confirmPasswordEdit->text();
}

void PasswordDialog::onSubmitClicked() {
  accept(); //Fecha o diálogo
}
