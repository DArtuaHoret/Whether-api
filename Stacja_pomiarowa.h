/**
 * @file Stacja_pomiarowa.h
 * @brief Plik nagłówkowy klasy StacjaPomiarowa
 *
 * Klasa StacjaPomiarowa przechowuje dane identyfikujące stację, jej położenie geograficzne oraz adres.
 * Udostępnia metody do serializacji i deserializacji danych w formacie JSON.
*/

#ifndef STACJA_POMIAROWA_H
#define STACJA_POMIAROWA_H

#include <QString>
#include <QJsonObject>

/**
 * @class StacjaPomiarowa
 * @brief Reprezentuje stację pomiarową środowiska.
 *
 * Klasa przechowuje dane identyfikujące stację, jej położenie geograficzne oraz adres.
 * Udostępnia metody do serializacji i deserializacji danych w formacie JSON.
 */
class StacjaPomiarowa
{
public:
    /**
     * @brief Konstruktor domyślny.
     *
     * Inicjalizuje pusty obiekt stacji pomiarowej.
     */
    StacjaPomiarowa();

    /**
     * @brief Konstruktor inicjalizujący.
     * @param id Identyfikator stacji.
     * @param nazwa Nazwa stacji.
     * @param latitude Szerokość geograficzna.
     * @param longitude Długość geograficzna.
     * @param miasto Miasto, w którym znajduje się stacja.
     * @param ulica Ulica, przy której znajduje się stacja.
     */
    explicit StacjaPomiarowa(int id, const QString& nazwa, double latitude, double longitude,
                             const QString& miasto, const QString& ulica);

    /**
     * @brief Tworzy obiekt StacjaPomiarowa z obiektu JSON.
     * @param json Obiekt JSON zawierający dane stacji.
     * @return Obiekt StacjaPomiarowa utworzony z JSON.
     */
    static StacjaPomiarowa fromJson(const QJsonObject& json);

    /**
     * @brief Konwertuje dane stacji pomiarowej do formatu JSON.
     * @return Obiekt JSON reprezentujący stację pomiarową.
     */
    QJsonObject toJson() const;

    /**
     * @brief Zwraca identyfikator stacji.
     * @return Identyfikator stacji jako int.
     */
    int id() const;

    /**
     * @brief Zwraca nazwę stacji.
     * @return Nazwa stacji jako QString.
     */
    QString nazwa() const;

    /**
     * @brief Zwraca szerokość geograficzną stacji.
     * @return Wartość szerokości geograficznej.
     */
    double latitude() const;

    /**
     * @brief Zwraca długość geograficzną stacji.
     * @return Wartość długości geograficznej.
     */
    double longitude() const;

    /**
     * @brief Zwraca nazwę miasta, w którym znajduje się stacja.
     * @return Nazwa miasta jako QString.
     */
    QString miasto() const;

    /**
     * @brief Zwraca nazwę ulicy, przy której znajduje się stacja.
     * @return Nazwa ulicy jako QString.
     */
    QString ulica() const;

private:
    int m_id;              /**< Identyfikator stacji */
    QString m_nazwa;       /**< Nazwa stacji */
    double m_latitude;     /**< Szerokość geograficzna */
    double m_longitude;    /**< Długość geograficzna */
    QString m_miasto;      /**< Nazwa miasta */
    QString m_ulica;       /**< Nazwa ulicy */
};

#endif // STACJA_POMIAROWA_H
