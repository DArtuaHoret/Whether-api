/**
 * @file main.cpp
 * @brief Punkt wejścia do aplikacji GUI monitorującej jakość powietrza.
 *
 * Plik zawiera funkcję `main`, która inicjalizuje aplikację Qt,
 * ustawia styl interfejsu użytkownika na „Fusion” oraz uruchamia
 * główne okno aplikacji `MainWindow`.
 *
 * @author Artur Horetskyi
 */

#include "Okno_gui.h"
#include <QApplication>

/**
 * @brief Główna funkcja aplikacji.
 *
 * Inicjalizuje aplikację Qt, ustawia styl interfejsu użytkownika
 * na „Fusion” i uruchamia główne okno `MainWindow`.
 *
 * @param argc Liczba argumentów wiersza poleceń.
 * @param argv Tablica argumentów wiersza poleceń.
 * @return Kod zakończenia aplikacji zwrócony przez `QApplication::exec()`.
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setStyle("Fusion");

    MainWindow w;
    w.show();

    return a.exec();
}
