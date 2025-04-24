#ifndef JPO_PROJEKT_H
#define JPO_PROJEKT_H

#include <QMainWindow>
#include <QLineEdit>   // Pole tekstowe
#include <QPushButton> // Przycisk
#include <QLabel>      // Etykieta
#include <QVBoxLayout> // Layout pionowy

class Jpo_projekt : public QMainWindow
{
    Q_OBJECT

public:
    Jpo_projekt(QWidget *parent = nullptr);
    ~Jpo_projekt();

private slots:
    void zatwierdz();  // Slot – obsługa kliknięcia przycisku

private:
    void pobierzPogode();  // Deklaracja funkcji

private:
    QWidget *centralnyWidget;
    QVBoxLayout *layout;
    QLineEdit *poleTekstowe;
    QPushButton *przycisk;
    QLabel *etykieta;
};



#endif // JPO_PROJEKT_H
