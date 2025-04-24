/**
 * @file Stanowisko_pomiarowe.cpp
 * @brief Plik źródłowy klasy StanowiskoPomiarowe
 */

#include "stanowisko_pomiarowe.h"

/**
 * @brief Domyślny konstruktor klasy StanowiskoPomiarowe.
 *
 * Inicjalizuje wszystkie pola wartościami domyślnymi:
 * - ID: -1
 * - ID stacji: -1
 * - Tekstowe pola: puste ciągi
 * - ID parametru: -1
 */
StanowiskoPomiarowe::StanowiskoPomiarowe() :
    m_id(-1), m_stacjaId(-1), m_parametr(""), m_formula(""), m_kod(""), m_idParam(-1)
{}

/**
 * @brief Konstruktor inicjalizujący dane stanowiska pomiarowego.
 *
 * @param id Identyfikator stanowiska.
 * @param stacjaId Identyfikator stacji.
 * @param parametr Nazwa parametru.
 * @param formula Wzór chemiczny parametru.
 * @param kod Kod parametru.
 * @param idParam Identyfikator parametru.
 */
StanowiskoPomiarowe::StanowiskoPomiarowe(int id, int stacjaId, const QString& parametr,
                                         const QString& formula, const QString& kod, int idParam) :
    m_id(id), m_stacjaId(stacjaId), m_parametr(parametr), m_formula(formula), m_kod(kod), m_idParam(idParam)
{}

/**
 * @brief Tworzy obiekt StanowiskoPomiarowe z obiektu JSON.
 *
 * Oczekiwany format JSON:
 * - "id": ID stanowiska,
 * - "stationId": ID stacji,
 * - "param": obiekt zawierający:
 *    - "paramName": nazwa parametru,
 *    - "paramFormula": wzór chemiczny,
 *    - "paramCode": kod parametru,
 *    - "idParam": ID parametru.
 *
 * @param json Obiekt JSON z danymi.
 * @return StanowiskoPomiarowe zainicjalizowane danymi z JSON.
 */
StanowiskoPomiarowe StanowiskoPomiarowe::fromJson(const QJsonObject& json) {
    int id = json["id"].toInt();
    int stacjaId = json["stationId"].toInt();

    QJsonObject paramObj = json["param"].toObject();
    QString parametr = paramObj["paramName"].toString();
    QString formula = paramObj["paramFormula"].toString();
    QString kod = paramObj["paramCode"].toString();
    int idParam = paramObj["idParam"].toInt();

    return StanowiskoPomiarowe(id, stacjaId, parametr, formula, kod, idParam);
}

/**
 * @brief Konwertuje dane stanowiska do obiektu JSON.
 *
 * Zwraca obiekt w formacie:
 * - "id"
 * - "stationId"
 * - "param" zawierający:
 *    - "paramName"
 *    - "paramFormula"
 *    - "paramCode"
 *    - "idParam"
 *
 * @return QJsonObject reprezentujący dane stanowiska.
 */
QJsonObject StanowiskoPomiarowe::toJson() const {
    QJsonObject obj;
    obj["id"] = m_id;
    obj["stationId"] = m_stacjaId;

    QJsonObject paramObj;
    paramObj["paramName"] = m_parametr;
    paramObj["paramFormula"] = m_formula;
    paramObj["paramCode"] = m_kod;
    paramObj["idParam"] = m_idParam;

    obj["param"] = paramObj;

    return obj;
}

/**
 * @brief Zwraca identyfikator stanowiska.
 * @return ID stanowiska.
 */
int StanowiskoPomiarowe::id() const {
    return m_id;
}

/**
 * @brief Zwraca identyfikator stacji, do której należy stanowisko.
 * @return ID stacji.
 */
int StanowiskoPomiarowe::stacjaId() const {
    return m_stacjaId;
}

/**
 * @brief Zwraca nazwę mierzonego parametru.
 * @return Nazwa parametru.
 */
QString StanowiskoPomiarowe::parametr() const {
    return m_parametr;
}

/**
 * @brief Zwraca wzór chemiczny parametru.
 * @return Wzór chemiczny.
 */
QString StanowiskoPomiarowe::formula() const {
    return m_formula;
}

/**
 * @brief Zwraca kod parametru.
 * @return Kod parametru.
 */
QString StanowiskoPomiarowe::kod() const {
    return m_kod;
}

/**
 * @brief Zwraca identyfikator parametru.
 * @return ID parametru.
 */
int StanowiskoPomiarowe::idParam() const {
    return m_idParam;
}
