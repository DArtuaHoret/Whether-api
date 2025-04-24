/**
 * @file dane_pomiarowe.cpp
 * @brief Plik źródłowy klasy DanePomiarowe
 */

#include "dane_pomiarowe.h"

/**
 * @brief Domyślny konstruktor klasy DanePomiarowe.
 *
 * Inicjalizuje datę bieżącym czasem systemowym, wartość na 0.0f i pusty parametr.
 */
DanePomiarowe::DanePomiarowe() :
    m_data(QDateTime::currentDateTime()), m_wartosc(0.0f), m_parametr("")
{}

/**
 * @brief Konstruktor inicjalizujący klasę DanePomiarowe podanymi danymi.
 * @param data Data i czas pomiaru.
 * @param wartosc Wartość pomiaru.
 * @param parametr Nazwa parametru pomiarowego.
 */
DanePomiarowe::DanePomiarowe(const QDateTime& data, float wartosc, const QString& parametr) :
    m_data(data), m_wartosc(wartosc), m_parametr(parametr)
{}

/**
 * @brief Tworzy obiekt DanePomiarowe na podstawie danych z obiektu JSON.
 *
 * Oczekiwane klucze JSON: "date" (w formacie ISO), "value" (liczba), "key" (nazwa parametru).
 *
 * @param json Obiekt JSON zawierający dane.
 * @return Obiekt DanePomiarowe utworzony na podstawie JSON.
 */
DanePomiarowe DanePomiarowe::fromJson(const QJsonObject& json) {
    QDateTime data = QDateTime::fromString(json["date"].toString(), Qt::ISODate);
    float wartosc = json["value"].isNull() ? 0.0f : static_cast<float>(json["value"].toDouble());
    QString parametr = json["key"].toString();

    return DanePomiarowe(data, wartosc, parametr);
}

/**
 * @brief Konwertuje dane pomiarowe do formatu JSON.
 *
 * Tworzy obiekt JSON zawierający klucze: "date", "value" oraz "key".
 *
 * @return Obiekt JSON reprezentujący dane pomiarowe.
 */
QJsonObject DanePomiarowe::toJson() const {
    QJsonObject obj;
    obj["date"] = m_data.toString(Qt::ISODate);
    obj["value"] = m_wartosc;
    obj["key"] = m_parametr;
    return obj;
}

/**
 * @brief Zwraca datę i czas pomiaru.
 * @return Obiekt QDateTime reprezentujący datę i czas pomiaru.
 */
QDateTime DanePomiarowe::data() const {
    return m_data;
}

/**
 * @brief Zwraca wartość pomiaru.
 * @return Wartość jako float.
 */
float DanePomiarowe::wartosc() const {
    return m_wartosc;
}

/**
 * @brief Zwraca nazwę parametru pomiarowego.
 * @return Parametr jako QString.
 */
QString DanePomiarowe::parametr() const {
    return m_parametr;
}

/**
 * @brief Ustawia nową datę i czas pomiaru.
 * @param data Nowa data i czas.
 */
void DanePomiarowe::setData(const QDateTime& data) {
    m_data = data;
}

/**
 * @brief Ustawia nową wartość pomiaru.
 * @param wartosc Nowa wartość pomiarowa.
 */
void DanePomiarowe::setWartosc(float wartosc) {
    m_wartosc = wartosc;
}

/**
 * @brief Ustawia nową nazwę parametru pomiarowego.
 * @param parametr Nowy parametr.
 */
void DanePomiarowe::setParametr(const QString& parametr) {
    m_parametr = parametr;
}
