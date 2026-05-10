/**
 * @file api_pobieranie.cpp
 * @brief Plik źródłowy klasy APIService
 */

#include "API_pobieranie.h"
#include <QNetworkRequest>
#include <QMessageBox>
#include <QDebug>
#include <QUrlQuery>
#include <QGeoCodingManager>
#include <QGeoServiceProvider>
#include <QGeoCodeReply>
#include <QGeoCoordinate>
#include <QGeoLocation>

#include <QtConcurrent>

/**
 * @brief Konstruktor klasy APIService.
 * Inicjalizuje menedżer sieci i ładuje dane z pliku cache (jeśli dostępne).
 */
APIService::APIService(QObject *parent) :
    QObject(parent),
    networkManager(new QNetworkAccessManager(this)),
    cache(100)
{
    connect(networkManager, &QNetworkAccessManager::finished,
            this, &APIService::onReplyFinished);

    QFile file(sciezkaPliku);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        aktualneDane = QJsonDocument::fromJson(data).object();
        file.close();
    }
}

/**
 * @brief Zapisuje aktualne dane do pliku automatycznie.
 * Emituje sygnał po zakończeniu zapisu.
 */
void APIService::zapiszDaneAutomatycznie() {
    QFile file(sciezkaPliku);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Nie można otworzyć pliku do zapisu:" << sciezkaPliku;
        return;
    }

    file.write(QJsonDocument(aktualneDane).toJson());
    file.close();
    qDebug() << "Dane zostały automatycznie zapisane do pliku";
    emit daneAutomatycznieZapisane();
}

/**
 * @brief Pobiera wszystkie stacje pomiarowe z API lub z cache.
 * Dane są przetwarzane i przekazywane dalej za pomocą sygnału.
 */
void APIService::pobierzWszystkieStacje() {
    QtConcurrent::run([=]() {
        QUrl url("https://api.gios.gov.pl/pjp-api/v1/rest/station/findAll?size=500");
        if (cache.contains(url.toString())) {
            QMetaObject::invokeMethod(this, [=]() {
                przetworzOdpowiedzStacje(cache[url.toString()]->toJson());
            }, Qt::QueuedConnection);
            return;
        }
        QNetworkRequest request(url);
        QMetaObject::invokeMethod(this, [=]() {
            networkManager->get(request);
        }, Qt::QueuedConnection);
    });
}

/**
 * @brief Pobiera stacje znajdujące się w podanym mieście.
 * Wykorzystuje dane z cache lub pobiera z sieci.
 *
 * @param miasto Nazwa miasta do filtrowania.
 */
void APIService::pobierzStacjeWMiescie(const QString& miasto) {
    QtConcurrent::run([=]() {
        QUrl url("https://api.gios.gov.pl/pjp-api/v1/rest/station/findAll?size=500");

        if (cache.contains(url.toString())) {
            QJsonDocument* cachedDoc = cache[url.toString()];
            QJsonArray wszystkieStacje;
            if (cachedDoc->isObject()) {
                QJsonObject obj = cachedDoc->object();
                if (obj.contains("Lista stacji pomiarowych"))
                    wszystkieStacje = obj["Lista stacji pomiarowych"].toArray();
            }
            QJsonArray przefiltrowane = filtrujStacjePoMiescie(wszystkieStacje, miasto);
            QMetaObject::invokeMethod(this, [=]() {
                emit daneStacjiPobrane(przefiltrowane);
            }, Qt::QueuedConnection);
            return;
        }
        QMetaObject::invokeMethod(this, [=]() {
            QNetworkRequest request(url);
            networkManager->get(request);
        }, Qt::QueuedConnection);
    });
}

/**
 * @brief Pobiera stanowiska pomiarowe przypisane do stacji.
 *
 * @param stacjaId Identyfikator stacji.
 */
void APIService::pobierzStanowiskaDlaStacji(int stacjaId) {
    QUrl url(QString("https://api.gios.gov.pl/pjp-api/v1/rest/station/sensors/%1").arg(stacjaId));
    QNetworkRequest request(url);
    networkManager->get(request);
}

/**
 * @brief Pobiera dane pomiarowe z danego stanowiska.
 *
 * @param stanowiskoId Identyfikator stanowiska.
 */
void APIService::pobierzDanePomiarowe(int stanowiskoId) {
    QUrl url(QString("https://api.gios.gov.pl/pjp-api/v1/rest/data/getData/%1").arg(stanowiskoId));
    QNetworkRequest request(url);
    networkManager->get(request);
}

/**
 * @brief Pobiera indeks jakości powietrza dla stacji.
 *
 * @param stacjaId Identyfikator stacji.
 */
void APIService::pobierzIndeksJakosciPowietrza(int stacjaId) {
    QUrl url(QString("https://api.gios.gov.pl/pjp-api/v1/rest/aqindex/getIndex/%1").arg(stacjaId));
    QNetworkRequest request(url);
    networkManager->get(request);
}

/**
 * @brief Obsługuje zakończenie odpowiedzi sieciowej.
 * Przetwarza dane w zależności od typu zapytania.
 *
 * @param reply Wskaźnik do obiektu odpowiedzi sieciowej.
 */
void APIService::onReplyFinished(QNetworkReply *reply) {
    QString url = reply->url().toString();
    if (reply->error() != QNetworkReply::NoError) {
        int httpStatus = reply->attribute(
                                  QNetworkRequest::HttpStatusCodeAttribute).toInt();

        if (url.contains("data/getData") && httpStatus == 400) {
            emit danePomiarowePobrane(QJsonArray(), "Brak danych bieżących");
            reply->deleteLater();
            return;
        }

        emit blad("Błąd sieci: " + reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray response = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(response);

    if (doc.isNull()) {
        emit blad("Nieprawidłowy format JSON");
        reply->deleteLater();
        return;
    }

    cache.insert(url, new QJsonDocument(doc));

    if (reply->request().rawHeader("X-Geo-Filtr") == "1") {
        double lat = reply->request().attribute(QNetworkRequest::User).toDouble();
        double lon = reply->request().attribute(QNetworkRequest::UserMax).toDouble();
        double promienKm = reply->request().attribute(QNetworkRequest::HttpPipeliningAllowedAttribute).toDouble();

        filtrujStacjeWPromieniu(lat, lon, promienKm);
        reply->deleteLater();
        return;
    }

    if (url.contains("station/findAll")) {

        QJsonArray stacje;

        if (doc.isArray()) {
            stacje = doc.array();
        }
        else if (doc.isObject()) {
            QJsonObject obj = doc.object();

            if (obj.contains("Lista stacji pomiarowych")) {
                stacje =
                    obj["Lista stacji pomiarowych"].toArray();
            }
        }

        if (reply->request().url().query().contains("miasto")) {

            QString miasto =
                reply->request().url().query().split("=")[1];

            QJsonArray przefiltrowane =
                filtrujStacjePoMiescie(stacje, miasto);

            emit daneStacjiPobrane(przefiltrowane);
        }
        else {
            emit daneStacjiPobrane(stacje);
        }
    }
    else if (url.contains("station/sensors")) {
        przetworzOdpowiedzStanowiska(response);
    }
    else if (url.contains("data/getData")) {
        przetworzOdpowiedzPomiary(response);
    }
    else if (url.contains("aqindex/getIndex")) {
        przetworzOdpowiedzIndeks(response);
    }

    reply->deleteLater();
}

/**
 * @brief Przetwarza odpowiedź JSON zawierającą listę stacji.
 *
 * @param odpowiedz Dane odpowiedzi w postaci bajtów.
 */
void APIService::przetworzOdpowiedzStacje(const QByteArray& odpowiedz) {
    qDebug().noquote() << "RAW RESPONSE:";
    qDebug().noquote() << odpowiedz;

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(odpowiedz, &err);

    if (err.error != QJsonParseError::NoError) {
        emit blad("Błąd parsowania JSON: " + err.errorString());
        return;
    }

    QJsonArray stacje;


    if (doc.isArray()) {
        stacje = doc.array();
    }


    else if (doc.isObject()) {
        QJsonObject obj = doc.object();

        if (obj.contains("Lista stacji pomiarowych") &&
            obj["Lista stacji pomiarowych"].isArray()) {

            stacje = obj["Lista stacji pomiarowych"].toArray();
        }
        else {
            emit blad("JSON nie zawiera tablicy stacji");
            qDebug().noquote() << "OBJECT:";
            qDebug() << obj;
            return;
        }
    }
    else {
        emit blad("Nieprawidłowy format JSON");
        return;
    }

    aktualneDane["stacje"] = stacje;

    zapiszDaneAutomatycznie();

    emit daneStacjiPobrane(stacje);
}

/**
 * @brief Przetwarza odpowiedź JSON zawierającą listę stanowisk.
 *
 * @param odpowiedz Dane odpowiedzi w postaci bajtów.
 */
void APIService::przetworzOdpowiedzStanowiska(const QByteArray& odpowiedz) {
    QJsonDocument doc = QJsonDocument::fromJson(odpowiedz);
    QJsonArray stanowiska;

    if (doc.isArray()) {
        stanowiska = doc.array();
    } else if (doc.isObject()) {
        QJsonObject obj = doc.object();
        qDebug() << "Klucze stanowisk:" << obj.keys();

        const QString klucz = "Lista stanowisk pomiarowych dla podanej stacji";
        if (obj.contains(klucz)) {
            stanowiska = obj[klucz].toArray();
        } else {
            for (const QString& k : obj.keys()) {
                if (obj[k].isArray()) {
                    stanowiska = obj[k].toArray();
                    qDebug() << "Stanowiska fallback klucz:" << k;
                    break;
                }
            }
        }
    }

    if (stanowiska.isEmpty()) {
        emit blad("Brak stanowisk w odpowiedzi JSON");
        return;
    }


    QJsonArray znormalizowane;
    for (const QJsonValue& val : stanowiska) {
        QJsonObject s = val.toObject();

 
        if (s.contains("Identyfikator stanowiska")) {
            QJsonObject znorm;
            znorm["id"]        = s["Identyfikator stanowiska"].toInt();
            znorm["stationId"] = s["Identyfikator stacji"].toInt();
            QJsonObject param;
            param["paramName"]    = s["Wskaźnik"].toString();
            param["paramFormula"] = s[QString::fromUtf8("Wskaźnik - wzór")].toString();
            param["paramCode"]    = s["Wskaźnik - kod"].toString();
            param["idParam"]      = s["Id wskaźnika"].toInt();
            znorm["param"] = param;
            znormalizowane.append(znorm);
        } else {
            znormalizowane.append(s); 
        }
    }

    aktualneDane["stanowiska"] = znormalizowane;
    zapiszDaneAutomatycznie();
    emit daneStanowiskPobrane(znormalizowane);
}

/**
 * @brief Przetwarza odpowiedź JSON z danymi pomiarowymi.
 *
 * @param odpowiedz Dane odpowiedzi w postaci bajtów.
 */
void APIService::przetworzOdpowiedzPomiary(const QByteArray& odpowiedz) {
    QJsonDocument doc = QJsonDocument::fromJson(odpowiedz);
    if (!doc.isObject()) {
        emit blad("Oczekiwano obiektu JSON dla pomiarów");
        return;
    }

    QJsonObject obj = doc.object();


    if (obj.contains("key") && obj.contains("values")) {
        QString parametrKod = obj["key"].toString();
        aktualneDane["pomiary"] = obj;
        zapiszDaneAutomatycznie();
        emit danePomiarowePobrane(obj["values"].toArray(), parametrKod);
        return;
    }


    QJsonArray lista;
    for (const QString& k : obj.keys()) {
        if (obj[k].isArray()) {
            lista = obj[k].toArray();
            break;
        }
    }

    if (lista.isEmpty()) {
        emit blad("Brak danych pomiarowych w odpowiedzi");
        return;
    }


    QString parametrKod = "N/A";
    if (!lista.isEmpty()) {
        QString kodStanowiska = lista.first().toObject()["Kod stanowiska"].toString();
        QStringList czesci = kodStanowiska.split("-");
        if (czesci.size() >= 3) {
            czesci.removeFirst();
            czesci.removeLast();
            parametrKod = czesci.join("-");
        }
    }

    QJsonArray znormalizowane;
    for (const QJsonValue& val : lista) {
        QJsonObject p = val.toObject();
        QJsonObject znorm;
        znorm["date"]  = p["Data"].toString();
        znorm["value"] = p[QString::fromUtf8("Wartość")].toDouble();
        znormalizowane.append(znorm);
    }

    QJsonObject zapiszObj;
    zapiszObj["key"]    = parametrKod;
    zapiszObj["values"] = znormalizowane;
    aktualneDane["pomiary"] = zapiszObj;
    zapiszDaneAutomatycznie();
    emit danePomiarowePobrane(znormalizowane, parametrKod);
}

/**
 * @brief Przetwarza odpowiedź JSON z indeksem jakości powietrza.
 *
 * @param odpowiedz Dane odpowiedzi w postaci bajtów.
 */
void APIService::przetworzOdpowiedzIndeks(const QByteArray& odpowiedz) {
    QJsonDocument doc = QJsonDocument::fromJson(odpowiedz);
    if (!doc.isObject()) {
        emit blad("Oczekiwano obiektu JSON");
        return;
    }
    aktualneDane["indeks"] = doc.object();
    zapiszDaneAutomatycznie();
    emit indeksJakosciPobrany(doc.object());
}

/**
 * @brief Filtrowanie stacji na podstawie nazwy miasta.
 *
 * @param stacje Tablica stacji.
 * @param miasto Nazwa miasta do filtrowania.
 * @return QJsonArray Lista stacji spełniających warunek.
 */
QJsonArray APIService::filtrujStacjePoMiescie(const QJsonArray& stacje, const QString& miasto) {
    QJsonArray wynik;
    QString szukaneMiasto = miasto.toLower();

    for (const QJsonValue& val : stacje) {
        QJsonObject stacja = val.toObject();
        QString aktualneMiasto =
            stacja["Nazwa miasta"].toString().toLower();

        if (aktualneMiasto.contains(szukaneMiasto)) {
            wynik.append(stacja);
        }
    }

    return wynik;
}

/**
 * @brief Zapisuje dane (w tym cache) do pliku JSON.
 *
 * @param sciezka Ścieżka pliku, do którego zapisywane są dane.
 * @return true Jeśli operacja została rozpoczęta.
 */
bool APIService::zapiszDaneDoPliku(const QString& sciezka) {
    QtConcurrent::run([=]() {
        QFile file(sciezka);
        bool sukces = false;

        if (file.open(QIODevice::WriteOnly)) {
            QJsonObject mainObject;

            QJsonObject cacheObject;
            for (const QString& url : cache.keys()) {
                if (const QJsonDocument* doc = cache[url]) {
                    cacheObject[url] = doc->object();
                }
            }
            mainObject["cache"] = cacheObject;

            file.write(QJsonDocument(mainObject).toJson());
            file.close();
            sukces = true;
        }

        QMetaObject::invokeMethod(this, [=]() {
            emit daneZapisane(sukces);
        }, Qt::QueuedConnection);
    });

    return true;
}

/**
 * @brief Wczytuje dane (w tym cache) z pliku JSON.
 *
 * @param sciezka Ścieżka pliku, z którego dane są ładowane.
 * @return true Jeśli operacja została rozpoczęta.
 */
bool APIService::wczytajDaneZPliku(const QString& sciezka) {
    QtConcurrent::run([=]() {
        QFile file(sciezka);
        bool sukces = false;

        if (file.open(QIODevice::ReadOnly)) {
            QByteArray data = file.readAll();
            QJsonDocument doc = QJsonDocument::fromJson(data);

            if (!doc.isNull() && doc.isObject()) {
                QJsonObject mainObject = doc.object();

                if (mainObject.contains("cache")) {
                    QJsonObject cacheObject = mainObject["cache"].toObject();
                    for (const QString& url : cacheObject.keys()) {
                        cache.insert(url, new QJsonDocument(cacheObject[url].toObject()));
                    }
                }

                sukces = true;
            }

            file.close();
        }

        QMetaObject::invokeMethod(this, [=]() {
            emit daneWczytane(sukces);
        }, Qt::QueuedConnection);
    });

    return true;
}

/**
 * @brief Wyszukuje stacje w zadanym promieniu od wskazanej lokalizacji.
 *
 * @param lokalizacja Nazwa lokalizacji (adres, miasto).
 * @param promienKm Promień w kilometrach.
 */
void APIService::znajdzStacjeWPromieniu(const QString& lokalizacja, double promienKm) {
    QGeoServiceProvider *geoService = new QGeoServiceProvider("osm");
    QGeoCodingManager *geoManager = geoService->geocodingManager();

    if (!geoManager) {
        emit blad("Nie można zainicjować usługi geokodowania");
        return;
    }

    QGeoCodeReply *reply = geoManager->geocode(lokalizacja);
    connect(reply, &QGeoCodeReply::finished, [this, reply, promienKm]() {
        if (reply->error() != QGeoCodeReply::NoError) {
            emit blad("Błąd geokodowania: " + reply->errorString());
            reply->deleteLater();
            return;
        }

        QList<QGeoLocation> locations = reply->locations();
        if (locations.isEmpty()) {
            emit blad("Nie znaleziono lokalizacji");
            reply->deleteLater();
            return;
        }

        QGeoCoordinate coord = locations.first().coordinate();

        if (cache.contains("https://api.gios.gov.pl/pjp-api/v1/rest/station/findAll")) {
            QtConcurrent::run([=]() {
                filtrujStacjeWPromieniu(coord.latitude(), coord.longitude(), promienKm);
            });
        } else {
            QUrl url("https://api.gios.gov.pl/pjp-api/v1/rest/station/findAll");
            QNetworkRequest request(url);

            request.setAttribute(QNetworkRequest::User, coord.latitude());
            request.setAttribute(QNetworkRequest::UserMax, coord.longitude());
            request.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, promienKm);
            request.setRawHeader("X-Geo-Filtr", "1");

            networkManager->get(request);
        }

        reply->deleteLater();
    });
}

/**
 * @brief Oblicza odległość między dwoma punktami geograficznymi.
 *
 * @param lat1 Szerokość geograficzna punktu 1.
 * @param lon1 Długość geograficzna punktu 1.
 * @param lat2 Szerokość geograficzna punktu 2.
 * @param lon2 Długość geograficzna punktu 2.
 * @return double Odległość w kilometrach.
 */
double APIService::obliczOdleglosc(double lat1, double lon1, double lat2, double lon2) {
    const double R = 6371.0;

    double dLat = qDegreesToRadians(lat2 - lat1);
    double dLon = qDegreesToRadians(lon2 - lon1);

    double a = qSin(dLat / 2) * qSin(dLat / 2) +
               qCos(qDegreesToRadians(lat1)) * qCos(qDegreesToRadians(lat2)) *
                   qSin(dLon / 2) * qSin(dLon / 2);

    double c = 2 * qAtan2(qSqrt(a), qSqrt(1 - a));

    return R * c;
}

/**
 * @brief Filtrowanie stacji znajdujących się w określonym promieniu od punktu.
 * Emituje wynikową listę posortowaną według odległości.
 *
 * @param lat Szerokość geograficzna punktu odniesienia.
 * @param lon Długość geograficzna punktu odniesienia.
 * @param promienKm Promień w kilometrach.
 */
void APIService::filtrujStacjeWPromieniu(double lat, double lon, double promienKm) {
    const QString urlKey = "https://api.gios.gov.pl/pjp-api/v1/rest/station/findAll?size=500";
    if (!cache.contains(urlKey)) return;

    QJsonDocument* cachedDoc = cache[urlKey];
    QJsonArray wszystkieStacje;
    if (cachedDoc->isObject()) {
        QJsonObject obj = cachedDoc->object();
        if (obj.contains("Lista stacji pomiarowych"))
            wszystkieStacje = obj["Lista stacji pomiarowych"].toArray();
    } else if (cachedDoc->isArray()) {
        wszystkieStacje = cachedDoc->array();
    }

    const QString klatN = QString::fromUtf8("WGS84 \xCF\x86 N"); // φ
    const QString klatE = QString::fromUtf8("WGS84 \xCE\xBB E"); // λ

    QJsonArray stacjeWPromieniu;
    for (const QJsonValue& val : wszystkieStacje) {
        QJsonObject stacja = val.toObject();
        double sLat = stacja[klatN].toString().toDouble();
        double sLon = stacja[klatE].toString().toDouble();
        double odleglosc = obliczOdleglosc(lat, lon, sLat, sLon);
        if (odleglosc <= promienKm) {
            QJsonObject kopia = stacja;
            kopia["distance"] = odleglosc;
            stacjeWPromieniu.append(kopia);
        }
    }

    QList<QJsonObject> stacjeList;
    for (const QJsonValue& val : stacjeWPromieniu)
        stacjeList.append(val.toObject());

    std::sort(stacjeList.begin(), stacjeList.end(),
              [](const QJsonObject& a, const QJsonObject& b) {
                  return a["distance"].toDouble() < b["distance"].toDouble();
              });

    QJsonArray sortedArray;
    for (const QJsonObject& o : stacjeList)
        sortedArray.append(o);

    emit daneStacjiPobrane(sortedArray);
}
