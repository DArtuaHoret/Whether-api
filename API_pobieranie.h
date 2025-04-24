/**
 * @file api_pobieranie.h
 * @brief Plik nagłówkowy klasy APIService
 *
 * Klasa APIService zapewnia kompleksową obsługę komunikacji z zewnętrznym API
 * monitorującym jakość powietrza. Zawiera metody do pobierania danych stacji,
 * stanowisk pomiarowych oraz aktualnych wyników pomiarów.
 */

#ifndef API_POBIERANIE_H
#define API_POBIERANIE_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QCache>
#include <QFile>
#include <QStandardPaths>

/**
 * @class APIService
 * @brief Klasa zarządzająca komunikacją z API jakości powietrza
 *
 * Klasa wykorzystuje QNetworkAccessManager do wysyłania żądań HTTP
 * i przetwarza otrzymane odpowiedzi w formacie JSON. Zapewnia cache'owanie
 * danych oraz automatyczne zapisywanie wyników do pliku.
 */
class APIService : public QObject
{
    Q_OBJECT
    friend class APIServiceTest; ///< Klasa testowa ma dostęp do prywatnych elementów

public:
    /**
     * @brief Konstruktor klasy APIService
     * @param parent Wskaźnik na obiekt rodzica (domyślnie nullptr)
     *
     * Inicjalizuje menedżera sieci i wczytuje zapisane dane z pliku.
     */
    explicit APIService(QObject *parent = nullptr);

    /**
     * @brief Pobiera wszystkie dostępne stacje pomiarowe
     *
     * Wysyła żądanie GET do endpointa /station/findAll i emituje sygnał
     * daneStacjiPobrane po otrzymaniu odpowiedzi.
     */
    void pobierzWszystkieStacje();

    /**
     * @brief Pobiera stacje w określonym mieście
     * @param miasto Nazwa miasta do filtrowania (wielkość liter nie ma znaczenia)
     *
     * Filtruje pobrane wcześniej stacje po nazwie miasta.
     */
    void pobierzStacjeWMiescie(const QString& miasto);

    /**
     * @brief Pobiera stanowiska pomiarowe dla konkretnej stacji
     * @param stacjaId Unikalny identyfikator stacji
     *
     * Wysyła żądanie GET do endpointa /station/sensors/{stacjaId}
     */
    void pobierzStanowiskaDlaStacji(int stacjaId);

    /**
     * @brief Pobiera dane pomiarowe dla wybranego stanowiska
     * @param stanowiskoId Unikalny identyfikator stanowiska
     *
     * Wysyła żądanie GET do endpointa /data/getData/{stanowiskoId}
     */
    void pobierzDanePomiarowe(int stanowiskoId);

    /**
     * @brief Pobiera indeks jakości powietrza dla stacji
     * @param stacjaId Unikalny identyfikator stacji
     *
     * Wysyła żądanie GET do endpointa /aqindex/getIndex/{stacjaId}
     */
    void pobierzIndeksJakosciPowietrza(int stacjaId);

    /**
     * @brief Zapisuje aktualne dane do pliku
     * @param sciezka Pełna ścieżka do pliku docelowego
     * @return true jeśli zapis się powiódł, false w przeciwnym przypadku
     */
    bool zapiszDaneDoPliku(const QString& sciezka);

    /**
     * @brief Wczytuje dane z pliku
     * @param sciezka Pełna ścieżka do pliku źródłowego
     * @return true jeśli wczytanie się powiodło, false w przeciwnym przypadku
     */
    bool wczytajDaneZPliku(const QString& sciezka);

    /**
     * @brief Wyszukuje stacje w określonym promieniu od lokalizacji
     * @param lokalizacja Adres lub nazwa miejsca (np. "Warszawa, Krakowskie Przedmieście 1")
     * @param promienKm Promień w kilometrach (min 0.1, max 1000)
     *
     * Wykorzystuje geokodowanie do znalezienia współrzędnych lokalizacji.
     */
    void znajdzStacjeWPromieniu(const QString& lokalizacja, double promienKm);

    /**
     * @brief Filtruje stacje w promieniu od podanych współrzędnych
     * @param lat Szerokość geograficzna (WGS84)
     * @param lon Długość geograficzna (WGS84)
     * @param promienKm Promień w kilometrach
     */
    void filtrujStacjeWPromieniu(double lat, double lon, double promienKm);

signals:
    /**
     * @brief Sygnał emitowany po pobraniu danych stacji
     * @param stacje Tablica JSON z danymi stacji
     */
    void daneStacjiPobrane(const QJsonArray& stacje);

    /**
     * @brief Sygnał emitowany po pobraniu danych stanowisk
     * @param stanowiska Tablica JSON z danymi stanowisk
     */
    void daneStanowiskPobrane(const QJsonArray& stanowiska);

    /**
     * @brief Sygnał emitowany po pobraniu danych pomiarowych
     * @param pomiary Tablica JSON z pomiarami
     * @param parametrKod Kod parametru (np. "PM10", "NO2")
     */
    void danePomiarowePobrane(const QJsonArray& pomiary, const QString& parametrKod);

    /**
     * @brief Sygnał emitowany po pobraniu indeksu jakości powietrza
     * @param indeks Obiekt JSON z danymi indeksu
     */
    void indeksJakosciPobrany(const QJsonObject& indeks);

    /**
     * @brief Sygnał emitowany w przypadku błędu
     * @param opisBledu Opis błędu
     */
    void blad(const QString& opisBledu);

    /**
     * @brief Sygnał emitowany po zapisie danych
     * @param sukces true jeśli zapis się powiódł
     */
    void daneZapisane(bool sukces);

    /**
     * @brief Sygnał emitowany po wczytaniu danych
     * @param sukces true jeśli wczytanie się powiodło
     */
    void daneWczytane(bool sukces);

    /**
     * @brief Sygnał emitowany przy automatycznym zapisie danych
     */
    void daneAutomatycznieZapisane();

private slots:
    /**
     * @brief Slot obsługujący zakończenie żądania sieciowego
     * @param reply Wskaźnik na obiekt odpowiedzi
     */
    void onReplyFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *networkManager; ///< Menedżer połączeń sieciowych
    QCache<QString, QJsonDocument> cache;  ///< Cache przechowujący odpowiedzi API

    /**
     * @brief Przetwarza odpowiedź z danymi stacji
     * @param odpowiedz Dane odpowiedzi w formacie JSON
     */
    void przetworzOdpowiedzStacje(const QByteArray& odpowiedz);

    /**
     * @brief Przetwarza odpowiedź z danymi stanowisk
     * @param odpowiedz Dane odpowiedzi w formacie JSON
     */
    void przetworzOdpowiedzStanowiska(const QByteArray& odpowiedz);

    /**
     * @brief Przetwarza odpowiedź z danymi pomiarowymi
     * @param odpowiedz Dane odpowiedzi w formacie JSON
     */
    void przetworzOdpowiedzPomiary(const QByteArray& odpowiedz);

    /**
     * @brief Przetwarza odpowiedź z indeksem jakości powietrza
     * @param odpowiedz Dane odpowiedzi w formacie JSON
     */
    void przetworzOdpowiedzIndeks(const QByteArray& odpowiedz);

    /**
     * @brief Filtruje stacje po nazwie miasta
     * @param stacje Tablica JSON ze stacjami
     * @param miasto Nazwa miasta do filtrowania
     * @return Przefiltrowana tablica JSON
     */
    QJsonArray filtrujStacjePoMiescie(const QJsonArray& stacje, const QString& miasto);

    /**
     * @brief Automatycznie zapisuje dane do domyślnego pliku
     */
    void zapiszDaneAutomatycznie();

    QString sciezkaPliku = "dane_pomiarowe.json"; ///< Domyślna ścieżka pliku danych
    QJsonObject aktualneDane; ///< Bieżące dane w pamięci

    /**
     * @brief Przetwarza odpowiedź geokodowania
     * @param odpowiedz Dane odpowiedzi
     * @param promienKm Promień wyszukiwania w km
     */
    void przetworzGeokodowanie(const QByteArray& odpowiedz, double promienKm);

    /**
     * @brief Oblicza odległość między dwoma punktami na Ziemi
     * @param lat1 Szerokość geograficzna punktu 1
     * @param lon1 Długość geograficzna punktu 1
     * @param lat2 Szerokość geograficzna punktu 2
     * @param lon2 Długość geograficzna punktu 2
     * @return Odległość w kilometrach
     *
     * Wykorzystuje wzór haversine do obliczeń.
     */
    double obliczOdleglosc(double lat1, double lon1, double lat2, double lon2);
};

#endif // API_POBIERANIE_H
