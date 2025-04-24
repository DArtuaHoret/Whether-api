/**
 * @file Stanowisko_pomiarowe.h
 * @brief Plik nagłówkowy klasy StanowiskoPomiarowe
 *
 * Klasa StanowiskoPomiarowe przechowuje dane identyfikujące stanowisko oraz szczegóły dotyczące mierzonego parametru.
 * Udostępnia metody do serializacji i deserializacji danych w formacie JSON.
*/

#ifndef STANOWISKO_POMIAROWE_H
#define STANOWISKO_POMIAROWE_H

#include <QString>
#include <QJsonObject>

/**
 * @class StanowiskoPomiarowe
 * @brief Reprezentuje pojedyncze stanowisko pomiarowe w ramach stacji pomiarowej.
 *
 * Klasa przechowuje dane identyfikujące stanowisko oraz szczegóły dotyczące mierzonego parametru.
 * Udostępnia metody do serializacji i deserializacji danych w formacie JSON.
 */
class StanowiskoPomiarowe
{
public:
    /**
     * @brief Domyślny konstruktor.
     *
     * Inicjalizuje pusty obiekt stanowiska pomiarowego.
     */
    StanowiskoPomiarowe();

    /**
     * @brief Konstruktor inicjalizujący.
     * @param id Identyfikator stanowiska.
     * @param stacjaId Identyfikator stacji, do której należy stanowisko.
     * @param parametr Nazwa mierzonego parametru (np. PM10).
     * @param formula Wzór chemiczny parametru.
     * @param kod Kod parametru.
     * @param idParam Identyfikator parametru.
     */
    explicit StanowiskoPomiarowe(int id, int stacjaId, const QString& parametr,
                                 const QString& formula, const QString& kod, int idParam);

    /**
     * @brief Tworzy obiekt StanowiskoPomiarowe na podstawie danych JSON.
     * @param json Obiekt QJsonObject zawierający dane stanowiska.
     * @return Obiekt StanowiskoPomiarowe utworzony na podstawie JSON.
     */
    static StanowiskoPomiarowe fromJson(const QJsonObject& json);

    /**
     * @brief Konwertuje dane stanowiska pomiarowego do formatu JSON.
     * @return Obiekt JSON reprezentujący stanowisko.
     */
    QJsonObject toJson() const;

    /**
     * @brief Zwraca identyfikator stanowiska.
     * @return ID stanowiska jako int.
     */
    int id() const;

    /**
     * @brief Zwraca ID stacji, do której należy stanowisko.
     * @return ID stacji jako int.
     */
    int stacjaId() const;

    /**
     * @brief Zwraca nazwę parametru pomiarowego.
     * @return Nazwa parametru jako QString.
     */
    QString parametr() const;

    /**
     * @brief Zwraca wzór chemiczny parametru.
     * @return Wzór chemiczny jako QString.
     */
    QString formula() const;

    /**
     * @brief Zwraca kod parametru pomiarowego.
     * @return Kod parametru jako QString.
     */
    QString kod() const;

    /**
     * @brief Zwraca identyfikator parametru.
     * @return ID parametru jako int.
     */
    int idParam() const;

private:
    int m_id;              /**< Identyfikator stanowiska */
    int m_stacjaId;        /**< Identyfikator stacji, do której należy stanowisko */
    QString m_parametr;    /**< Nazwa parametru pomiarowego */
    QString m_formula;     /**< Wzór chemiczny parametru */
    QString m_kod;         /**< Kod parametru */
    int m_idParam;         /**< Identyfikator parametru */
};

#endif // STANOWISKO_POMIAROWE_H
