/**
 * @file Stacja_pomiarowa.cpp
 * @brief Plik źródłowy klasy StacjaPomiarowa
 */

#include "stacja_pomiarowa.h"

/**
 * @brief Domyślny konstruktor klasy StacjaPomiarowa.
 *
 * Inicjalizuje stację wartościami domyślnymi: ID = -1, współrzędne = 0, teksty puste.
 */
StacjaPomiarowa::StacjaPomiarowa() :
    m_id(-1), m_nazwa(""), m_latitude(0), m_longitude(0), m_miasto(""), m_ulica("")
{}

/**
 * @brief Konstruktor inicjalizujący obiekt danymi stacji pomiarowej.
 *
 * @param id Identyfikator stacji.
 * @param nazwa Nazwa stacji.
 * @param latitude Szerokość geograficzna.
 * @param longitude Długość geograficzna.
 * @param miasto Nazwa miasta.
 * @param ulica Nazwa ulicy.
 */
StacjaPomiarowa::StacjaPomiarowa(int id, const QString& nazwa, double latitude, double longitude,
                                 const QString& miasto, const QString& ulica) :
    m_id(id), m_nazwa(nazwa), m_latitude(latitude), m_longitude(longitude),
    m_miasto(miasto), m_ulica(ulica)
{}

/**
 * @brief Tworzy obiekt StacjaPomiarowa na podstawie danych JSON.
 *
 * Oczekiwane pola JSON:
 * - "id": identyfikator stacji,
 * - "stationName": nazwa stacji,
 * - "gegrLat": szerokość geograficzna (jako string),
 * - "gegrLon": długość geograficzna (jako string),
 * - "city": obiekt zawierający pole "name" z nazwą miasta,
 * - "addressStreet": nazwa ulicy.
 *
 * @param json Obiekt QJsonObject zawierający dane stacji.
 * @return Obiekt StacjaPomiarowa utworzony z JSON.
 */
StacjaPomiarowa StacjaPomiarowa::fromJson(const QJsonObject& json) {
    int id = json["id"].toInt();
    QString nazwa = json["stationName"].toString();

    double lat = json["gegrLat"].toString().toDouble();
    double lon = json["gegrLon"].toString().toDouble();

    QJsonObject cityObj = json["city"].toObject();
    QString miasto = cityObj["name"].toString();

    QString ulica = json["addressStreet"].toString();

    return StacjaPomiarowa(id, nazwa, lat, lon, miasto, ulica);
}

/**
 * @brief Konwertuje dane stacji pomiarowej do formatu JSON.
 *
 * Tworzy strukturę JSON zawierającą:
 * - "id"
 * - "stationName"
 * - "gegrLat"
 * - "gegrLon"
 * - "city" z podpolem "name"
 * - "addressStreet"
 *
 * @return Obiekt JSON reprezentujący stację pomiarową.
 */
QJsonObject StacjaPomiarowa::toJson() const {
    QJsonObject obj;
    obj["id"] = m_id;
    obj["stationName"] = m_nazwa;
    obj["gegrLat"] = QString::number(m_latitude);
    obj["gegrLon"] = QString::number(m_longitude);

    QJsonObject cityObj;
    cityObj["name"] = m_miasto;
    obj["city"] = cityObj;

    obj["addressStreet"] = m_ulica;

    return obj;
}

/**
 * @brief Zwraca identyfikator stacji.
 * @return ID stacji jako int.
 */
int StacjaPomiarowa::id() const {
    return m_id;
}

/**
 * @brief Zwraca nazwę stacji.
 * @return Nazwa stacji jako QString.
 */
QString StacjaPomiarowa::nazwa() const {
    return m_nazwa;
}

/**
 * @brief Zwraca szerokość geograficzną stacji.
 * @return Szerokość geograficzna jako double.
 */
double StacjaPomiarowa::latitude() const {
    return m_latitude;
}

/**
 * @brief Zwraca długość geograficzną stacji.
 * @return Długość geograficzna jako double.
 */
double StacjaPomiarowa::longitude() const {
    return m_longitude;
}

/**
 * @brief Zwraca nazwę miasta, w którym znajduje się stacja.
 * @return Nazwa miasta jako QString.
 */
QString StacjaPomiarowa::miasto() const {
    return m_miasto;
}

/**
 * @brief Zwraca nazwę ulicy, przy której znajduje się stacja.
 * @return Nazwa ulicy jako QString.
 */
QString StacjaPomiarowa::ulica() const {
    return m_ulica;
}
