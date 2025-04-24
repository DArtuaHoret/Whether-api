/**
 * @file Dane_pomiarowe.h
 * @brief Plik nagłówkowy klasy DanePomiarowe
 *
 * Klasa DanePomiarowe przechowuje informacje o dacie i godzinie pomiaru, jego wartości oraz rodzaju parametru.
 * Umożliwia również konwersję danych do i z formatu JSON.
*/

#ifndef DANE_POMIAROWE_H
#define DANE_POMIAROWE_H

#include <QDateTime>
#include <QString>
#include <QJsonObject>

/**
 * @class DanePomiarowe
 * @brief Reprezentuje pojedynczy rekord danych pomiarowych.
 *
 * Klasa przechowuje informacje o dacie i godzinie pomiaru, jego wartości oraz rodzaju parametru.
 * Umożliwia również konwersję danych do i z formatu JSON.
 */
class DanePomiarowe
{
public:
    /**
     * @brief Konstruktor domyślny.
     *
     * Tworzy pusty obiekt danych pomiarowych.
     */
    DanePomiarowe();

    /**
     * @brief Konstruktor inicjalizujący.
     * @param data Data i czas pomiaru.
     * @param wartosc Zmierzona wartość.
     * @param parametr Nazwa parametru pomiarowego.
     */
    explicit DanePomiarowe(const QDateTime& data, float wartosc, const QString& parametr);

    /**
     * @brief Tworzy obiekt DanePomiarowe z obiektu JSON.
     * @param json Obiekt JSON zawierający dane.
     * @return Obiekt DanePomiarowe utworzony na podstawie danych JSON.
     */
    static DanePomiarowe fromJson(const QJsonObject& json);

    /**
     * @brief Konwertuje dane pomiarowe do formatu JSON.
     * @return Obiekt JSON reprezentujący dane pomiarowe.
     */
    QJsonObject toJson() const;

    /**
     * @brief Zwraca datę i czas pomiaru.
     * @return Obiekt QDateTime reprezentujący datę i czas pomiaru.
     */
    QDateTime data() const;

    /**
     * @brief Zwraca wartość pomiaru.
     * @return Wartość pomiarowa jako float.
     */
    float wartosc() const;

    /**
     * @brief Zwraca nazwę parametru pomiarowego.
     * @return Parametr jako QString.
     */
    QString parametr() const;

    /**
     * @brief Ustawia datę i czas pomiaru.
     * @param data Nowa data i czas pomiaru.
     */
    void setData(const QDateTime& data);

    /**
     * @brief Ustawia wartość pomiaru.
     * @param wartosc Nowa wartość pomiarowa.
     */
    void setWartosc(float wartosc);

    /**
     * @brief Ustawia nazwę parametru pomiarowego.
     * @param parametr Nowy parametr.
     */
    void setParametr(const QString& parametr);

private:
    QDateTime m_data;     /**< Data i czas pomiaru */
    float m_wartosc;      /**< Zmierzona wartość */
    QString m_parametr;   /**< Nazwa parametru pomiarowego */
};

#endif // DANE_POMIAROWE_H
