/**
 * @file Okno_gui.h
 * @brief Plik nagłówkowy klasy MainWindow
 *
 * Klasa MainWidnow zarządza interfejsem graficznym użytkownika (GUI), pozwalając na pobieranie i filtrowanie danych o stacjach, stanowiskach i pomiarach,
 * wyświetlanie wykresów oraz mapy Polski z naniesionymi stacjami pomiarowymi.
*/

#ifndef OKNO_GUI_H
#define OKNO_GUI_H

#include <QMainWindow>
#include <QListWidget>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QSplitter>
#include <QtCharts>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>

#include "API_pobieranie.h"

/**
 * @class MainWindow
 * @brief Główne okno aplikacji do monitorowania jakości powietrza.
 *
 * Klasa zarządza interfejsem graficznym użytkownika (GUI), pozwalając na pobieranie i filtrowanie danych o stacjach, stanowiskach i pomiarach,
 * wyświetlanie wykresów oraz mapy Polski z naniesionymi stacjami pomiarowymi.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Konstruktor klasy MainWindow.
     * @param parent Wskaźnik na obiekt rodzica (domyślnie nullptr).
     */
    explicit MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destruktor klasy MainWindow.
     */
    ~MainWindow();

private slots:
    /**
     * @brief Obsługuje kliknięcie przycisku "Pobierz stacje".
     */
    void on_pobierzStacje_clicked();

    /**
     * @brief Obsługuje kliknięcie przycisku "Filtruj stacje".
     */
    void on_filtrujStacje_clicked();

    /**
     * @brief Obsługuje kliknięcie przycisku "Szukaj w promieniu".
     */
    void on_szukajWPromieniu_clicked();

    /**
     * @brief Obsługuje wybór stacji z listy.
     * @param item Wybrany element listy.
     */
    void on_stacjaWybrana(QListWidgetItem* item);

    /**
     * @brief Obsługuje wybór stanowiska z listy.
     * @param item Wybrany element listy.
     */
    void on_stanowiskoWybrana(QListWidgetItem* item);

    /**
     * @brief Obsługuje kliknięcie przycisku "Pokaż mapę".
     */
    void on_toggleMapa_clicked();

    /**
     * @brief Obsługuje kliknięcie przycisku "Filtruj pomiary".
     */
    void on_filtrujPomiary_clicked();

    /**
     * @brief Wyświetla listę stacji na podstawie danych JSON.
     * @param stacje Dane stacji w formacie JSON.
     */
    void wyswietlStacje(const QJsonArray& stacje);

    /**
     * @brief Wyświetla listę stanowisk.
     * @param stanowiska Dane stanowisk w formacie JSON.
     */
    void wyswietlStanowiska(const QJsonArray& stanowiska);

    /**
     * @brief Wyświetla pomiary dla wybranego parametru.
     * @param pomiary Dane pomiarowe w formacie JSON.
     * @param parametrKod Kod parametru pomiarowego.
     */
    void wyswietlPomiary(const QJsonArray& pomiary, const QString& parametrKod);

    /**
     * @brief Wyświetla indeks jakości powietrza.
     * @param indeks Dane indeksu w formacie JSON.
     */
    void wyswietlIndeks(const QJsonObject& indeks);

    /**
     * @brief Wyświetla komunikat o błędzie.
     * @param blad Treść błędu.
     */
    void wyswietlBlad(const QString& blad);

private:
    /**
     * @brief Inicjalizuje interfejs użytkownika.
     */
    void setupUI();

    /**
     * @brief Inicjalizuje połączenia sygnałów i slotów.
     */
    void setupConnections();

    /**
     * @brief Wyświetla wykres na podstawie danych pomiarowych.
     * @param dane Dane pomiarowe.
     * @param parametrKod Kod parametru.
     */
    void wyswietlWykres(const QJsonArray& dane, const QString& parametrKod);

    /**
     * @brief Rysuje mapę Polski z naniesionymi stacjami.
     * @param stacje Dane stacji w formacie JSON.
     */
    void rysujMapePolski(const QJsonArray& stacje);

    /**
     * @brief Obsługuje zdarzenia filtrowania.
     */
    bool eventFilter(QObject *obj, QEvent *event) override;

    /**
     * @brief Obsługuje zdarzenie przewijania (zoom) na mapie.
     */
    void wheelEvent(QWheelEvent *event) override;

    /**
     * @brief Oblicza statystyki na podstawie pomiarów.
     */
    void obliczStatystyki();

    QListWidget *listaStacji;           /**< Lista dostępnych stacji pomiarowych */
    QListWidget *listaStanowisk;        /**< Lista stanowisk pomiarowych */
    QListWidget *listaPomiarow;         /**< Lista wyników pomiarów */
    QPushButton *przyciskPobierzStacje; /**< Przycisk do pobrania stacji */
    QPushButton *przyciskFiltrujStacje; /**< Przycisk do filtrowania stacji */
    QPushButton *przyciskZapisz;        /**< Przycisk do zapisu danych */
    QPushButton *przyciskWczytaj;       /**< Przycisk do wczytania danych */
    QLineEdit *poleMiasto;              /**< Pole do wpisania nazwy miasta */
    QChartView *widokWykresu;           /**< Widok wykresu danych pomiarowych */
    QSplitter *splitter;                /**< Główny rozdzielacz interfejsu */
    QSplitter *verticalSplitter;        /**< Dodatkowy pionowy rozdzielacz */

    QLineEdit *poleLokalizacja;         /**< Pole do wpisania lokalizacji */
    QLineEdit *polePromien;             /**< Pole do wpisania promienia wyszukiwania */
    QPushButton *przyciskSzukajWPromieniu; /**< Przycisk do wyszukiwania stacji w promieniu */

    QPushButton *przyciskPokazMape;     /**< Przycisk do pokazania/ukrycia mapy */
    QGraphicsView *widokMapy;           /**< Widok graficzny mapy Polski */
    QGraphicsScene *scenaMapy;          /**< Scena zawierająca elementy graficzne mapy */

    QLabel* indeksPowietrzaLabel;       /**< Etykieta wyświetlająca indeks powietrza */

    APIService *apiService;             /**< Wskaźnik do klasy obsługującej API */
    int aktualnaStacjaId;               /**< ID aktualnie wybranej stacji */

    bool mapaWidoczna;                  /**< Flaga widoczności mapy */

    QPointF lastMousePos;               /**< Ostatnia pozycja kursora (mapa) */
    bool isPanning;                     /**< Flaga przesuwania mapy */

    double currentZoomLevel;            /**< Aktualny poziom powiększenia mapy */

    QGraphicsSimpleTextItem* currentLabel = nullptr; /**< Aktualnie wyświetlana etykieta stacji */

    QDateTimeEdit *dataPoczatkowa;      /**< Pole wyboru daty początkowej */
    QDateTimeEdit *dataKoncowa;         /**< Pole wyboru daty końcowej */
    QPushButton *przyciskFiltrujPomiary;/**< Przycisk do filtrowania pomiarów według daty */

    QJsonArray ostatniePomiary;         /**< Ostatnio pobrane dane pomiarowe */
    QString ostatniParametrKod;         /**< Ostatnio używany kod parametru */

    QWidget *statystykiWidget;          /**< Widżet do wyświetlania statystyk */
    QLabel *statystykiLabel;            /**< Etykieta ze statystykami */
    QPushButton *przyciskObliczStatystyki; /**< Przycisk do obliczania statystyk */
};

#endif // OKNO_GUI_H
