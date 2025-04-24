/**
 * @file Okno_gui.cpp
 * @brief Plik źródłowy klasy MainWindow
 */

#include "Okno_gui.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QUrlQuery>
#include <QJsonValue>
#include <QJsonObject>
#include <QGraphicsEllipseItem>
#include <QPen>
#include <QGraphicsSimpleTextItem>
#include <QScrollBar>
#include <QtConcurrent>

/**
 * @brief Konstruktor klasy MainWindow.
 * @param parent Wskaźnik na rodzica.
 *
 * Inicjalizuje komponenty GUI, ustawia widok mapy oraz łączy sygnały z odpowiednimi slotami.
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    apiService(new APIService(this)),
    aktualnaStacjaId(-1),
    scenaMapy(new QGraphicsScene(this)),
    widokMapy(new QGraphicsView(this)),
    mapaWidoczna(false),
    isPanning(false),
    currentZoomLevel(1.0)
{
    setupUI();
    setupConnections();

    widokMapy->installEventFilter(this);
    widokMapy->setInteractive(true);
    widokMapy->setRenderHint(QPainter::Antialiasing);
    widokMapy->setRenderHint(QPainter::SmoothPixmapTransform);
    widokMapy->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    widokMapy->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    widokMapy->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    widokMapy->viewport()->installEventFilter(this);
    widokMapy->setFocusPolicy(Qt::NoFocus);


    widokMapy->viewport()->setAttribute(Qt::WA_AcceptTouchEvents, false);
    qApp->installEventFilter(this);

    setWindowTitle("Monitor jakości powietrza");
    resize(1200, 800);

}

/**
 * @brief Destruktor klasy MainWindow.
 *
 * Usuwa obiekt klasy APIService.
 */
MainWindow::~MainWindow() {
    delete apiService;
}

/**
 * @brief Tworzy i rozmieszcza wszystkie komponenty GUI w oknie głównym.
 */
void MainWindow::setupUI() {

    listaStacji = new QListWidget(this);
    listaStanowisk = new QListWidget(this);
    listaPomiarow = new QListWidget(this);

    przyciskPobierzStacje = new QPushButton("Pobierz wszystkie stacje", this);
    przyciskPokazMape = new QPushButton("Pokaż mapę", this);
    przyciskFiltrujStacje = new QPushButton("Filtruj po mieście", this);
    poleMiasto = new QLineEdit(this);
    poleMiasto->setPlaceholderText("Wpisz nazwę miasta...");

    poleLokalizacja = new QLineEdit(this);
    poleLokalizacja->setPlaceholderText("Wpisz lokalizację (np. Polanka 3, Poznań)");

    polePromien = new QLineEdit(this);
    polePromien->setPlaceholderText("Promień w km");
    polePromien->setValidator(new QDoubleValidator(0.1, 1000.0, 2, this));

    przyciskSzukajWPromieniu = new QPushButton("Szukaj stacji w promieniu", this);


    widokWykresu = new QChartView(this);
    widokWykresu->setRenderHint(QPainter::Antialiasing);
    widokWykresu->setMinimumHeight(200);

    widokMapy->setScene(scenaMapy);
    widokMapy->setRenderHint(QPainter::Antialiasing);
    widokMapy->setDragMode(QGraphicsView::ScrollHandDrag);
    widokMapy->setInteractive(true);
    widokMapy->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    widokMapy->setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    widokMapy->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    widokMapy->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    widokMapy->setFocusPolicy(Qt::StrongFocus);

    verticalSplitter = new QSplitter(Qt::Vertical);
    verticalSplitter->addWidget(widokMapy);
    verticalSplitter->addWidget(widokWykresu);
    verticalSplitter->setStretchFactor(0, 3);
    verticalSplitter->setStretchFactor(1, 1);

    indeksPowietrzaLabel = new QLabel("Indeks jakości powietrza: -", this);
    indeksPowietrzaLabel->setStyleSheet("font-weight: bold; color: green");

    QHBoxLayout *layoutPrzyciskow = new QHBoxLayout();
    layoutPrzyciskow->addWidget(przyciskPobierzStacje);
    layoutPrzyciskow->addWidget(przyciskPokazMape);

    QVBoxLayout *layoutFiltrowania = new QVBoxLayout();
    layoutFiltrowania->addLayout(layoutPrzyciskow);
    layoutFiltrowania->addWidget(new QLabel("Filtruj po mieście:"));
    layoutFiltrowania->addWidget(poleMiasto);
    layoutFiltrowania->addWidget(przyciskFiltrujStacje);
    layoutFiltrowania->addSpacing(10);
    layoutFiltrowania->addWidget(new QLabel("Lokalizacja:"));
    layoutFiltrowania->addWidget(poleLokalizacja);
    layoutFiltrowania->addWidget(new QLabel("Promień (km):"));
    layoutFiltrowania->addWidget(polePromien);
    layoutFiltrowania->addWidget(przyciskSzukajWPromieniu);
    layoutFiltrowania->addSpacing(10);
    layoutFiltrowania->addWidget(indeksPowietrzaLabel);

    QGroupBox *grupaFiltrowania = new QGroupBox("Opcje filtrowania");
    grupaFiltrowania->setLayout(layoutFiltrowania);

    QTabWidget *zakladki = new QTabWidget();
    zakladki->addTab(listaStacji, "Stacje");
    zakladki->addTab(listaStanowisk, "Stanowiska");

    QWidget *zakresWidget = new QWidget();
    QVBoxLayout *zakresLayout = new QVBoxLayout(zakresWidget);

    QLabel *labelZakres = new QLabel("Wybierz zakres czasowy pomiarów:");
    dataPoczatkowa = new QDateTimeEdit();
    dataPoczatkowa->setDisplayFormat("yyyy-MM-dd HH:mm");
    dataPoczatkowa->setDateTime(QDateTime());

    dataKoncowa = new QDateTimeEdit();
    dataKoncowa->setDisplayFormat("yyyy-MM-dd HH:mm");
    dataKoncowa->setDateTime(QDateTime());

    przyciskFiltrujPomiary = new QPushButton("Zastosuj filtr");

    zakresLayout->addWidget(labelZakres);
    zakresLayout->addWidget(new QLabel("Od:"));
    zakresLayout->addWidget(dataPoczatkowa);
    zakresLayout->addWidget(new QLabel("Do:"));
    zakresLayout->addWidget(dataKoncowa);
    zakresLayout->addWidget(przyciskFiltrujPomiary);
    zakresLayout->addStretch();

    QWidget *statystykiWidget = new QWidget();
    QVBoxLayout *statystykiLayout = new QVBoxLayout(statystykiWidget);

    przyciskObliczStatystyki = new QPushButton("Oblicz statystyki", this);
    statystykiLabel = new QLabel("Statystyki pojawią się tutaj po obliczeniu", this);
    statystykiLabel->setWordWrap(true);

    statystykiLayout->addWidget(przyciskObliczStatystyki);
    statystykiLayout->addWidget(statystykiLabel);
    statystykiLayout->addStretch();

    zakladki->addTab(zakresWidget, "Zakres pomiarów");
    zakladki->addTab(listaPomiarow, "Pomiary");
    zakladki->addTab(statystykiWidget, "Statystyki");

    QVBoxLayout *layoutZakladek = new QVBoxLayout();
    layoutZakladek->addWidget(zakladki);

    QGroupBox *grupaDanych = new QGroupBox("Dane");
    grupaDanych->setLayout(layoutZakladek);

    QVBoxLayout *layoutLewy = new QVBoxLayout();
    layoutLewy->addWidget(grupaFiltrowania);
    layoutLewy->addWidget(grupaDanych);

    QWidget *widgetLewy = new QWidget();
    widgetLewy->setLayout(layoutLewy);

    widokMapy->setScene(scenaMapy);
    widokMapy->setRenderHint(QPainter::Antialiasing);
    widokMapy->setDragMode(QGraphicsView::ScrollHandDrag);

    widokMapy->setScene(scenaMapy);
    widokMapy->setRenderHint(QPainter::Antialiasing);
    widokMapy->setDragMode(QGraphicsView::ScrollHandDrag);
    widokMapy->setInteractive(true);
    widokMapy->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    widokMapy->setResizeAnchor(QGraphicsView::AnchorUnderMouse);

    QVBoxLayout *layoutMapy = new QVBoxLayout();
    layoutMapy->addWidget(widokMapy);

    QGroupBox *grupaMapy = new QGroupBox("Mapa stacji");
    grupaMapy->setLayout(layoutMapy);

    widokWykresu->setRenderHint(QPainter::Antialiasing);
    widokWykresu->setMinimumHeight(200);

    QVBoxLayout *layoutWykresu = new QVBoxLayout();
    layoutWykresu->addWidget(widokWykresu);

    QGroupBox *grupaWykres = new QGroupBox("Wykres pomiarów");
    grupaWykres->setLayout(layoutWykresu);

    QVBoxLayout *layoutPrawy = new QVBoxLayout();
    layoutPrawy->addWidget(grupaMapy);
    layoutPrawy->addWidget(grupaWykres);

    QWidget *widgetPrawy = new QWidget();
    widgetPrawy->setLayout(layoutPrawy);

    splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(widgetLewy);
    splitter->addWidget(widgetPrawy);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 3);
    splitter->setSizes({200, 600});

    setCentralWidget(splitter);
}

/**
 * @brief Łączy sygnały z odpowiednimi metodami (slotami).
 */
void MainWindow::setupConnections() {
    connect(przyciskPobierzStacje, &QPushButton::clicked,
            this, &MainWindow::on_pobierzStacje_clicked);
    connect(przyciskFiltrujStacje, &QPushButton::clicked,
            this, &MainWindow::on_filtrujStacje_clicked);
    connect(listaStacji, &QListWidget::itemClicked,
            this, &MainWindow::on_stacjaWybrana);
    connect(listaStanowisk, &QListWidget::itemClicked,
            this, &MainWindow::on_stanowiskoWybrana);
    connect(apiService, &APIService::daneStacjiPobrane,
            this, &MainWindow::wyswietlStacje);
    connect(apiService, &APIService::daneStanowiskPobrane,
            this, &MainWindow::wyswietlStanowiska);
    connect(apiService, &APIService::danePomiarowePobrane,
            this, &MainWindow::wyswietlPomiary);
    connect(apiService, &APIService::indeksJakosciPobrany,
            this, &MainWindow::wyswietlIndeks);
    connect(apiService, &APIService::blad,
            this, &MainWindow::wyswietlBlad);
    connect(przyciskSzukajWPromieniu, &QPushButton::clicked,
            this, &MainWindow::on_szukajWPromieniu_clicked);
    connect(przyciskPokazMape, &QPushButton::clicked,
            this, &MainWindow::on_toggleMapa_clicked);
    connect(przyciskFiltrujPomiary, &QPushButton::clicked,
            this, &MainWindow::on_filtrujPomiary_clicked);
    connect(przyciskObliczStatystyki, &QPushButton::clicked,
            this, &MainWindow::obliczStatystyki);
}

/**
 * @brief Obsługuje kliknięcie przycisku "Pobierz wszystkie stacje".
 *
 * Czyści aktualne dane i uruchamia pobieranie wszystkich stacji.
 */
void MainWindow::on_pobierzStacje_clicked() {
    poleMiasto->clear();

    if (listaStanowisk) listaStanowisk->clear();
    if (listaPomiarow) listaPomiarow->clear();
    if (widokWykresu) {
        QChart* pustyWykres = new QChart();
        widokWykresu->setChart(pustyWykres);
    }

    apiService->pobierzWszystkieStacje();
}

/**
 * @brief Obsługuje kliknięcie przycisku "Filtruj po mieście".
 *
 * Sprawdza, czy pole miasta nie jest puste i uruchamia pobieranie stacji.
 */
void MainWindow::on_filtrujStacje_clicked() {
    QString miasto = poleMiasto->text().trimmed();
    if (miasto.isEmpty()) {
        QMessageBox::warning(this, "Błąd", "Proszę wpisać nazwę miasta");
        return;
    }

    if (listaStanowisk) listaStanowisk->clear();
    if (listaPomiarow) listaPomiarow->clear();
    if (widokWykresu) {
        QChart* pustyWykres = new QChart();
        widokWykresu->setChart(pustyWykres);
    }

    poleMiasto->setFocus();
    apiService->pobierzWszystkieStacje();
}

/**
 * @brief Obsługuje kliknięcie elementu z listy stacji.
 * @param item Wybrany element z listy stacji.
 *
 * Pobiera stanowiska i indeks jakości powietrza dla wybranej stacji.
 */
void MainWindow::on_stacjaWybrana(QListWidgetItem* item) {
    int id = item->data(Qt::UserRole).toInt();
    aktualnaStacjaId = id;
    apiService->pobierzStanowiskaDlaStacji(id);
    apiService->pobierzIndeksJakosciPowietrza(id);
}

/**
 * @brief Obsługuje kliknięcie elementu z listy stanowisk.
 * @param item Wybrany element z listy stanowisk.
 *
 * Pobiera dane pomiarowe dla wybranego stanowiska.
 */
void MainWindow::on_stanowiskoWybrana(QListWidgetItem* item) {
    int id = item->data(Qt::UserRole).toInt();
    apiService->pobierzDanePomiarowe(id);
}

/**
 * @brief Obsługuje kliknięcie przycisku "Zastosuj filtr".
 *
 * Filtruje dane pomiarowe na podstawie wybranego zakresu dat.
 * Waliduje poprawność zakresu względem dostępnych danych.
 */
void MainWindow::on_filtrujPomiary_clicked() {
    if (aktualnaStacjaId == -1 || listaStanowisk->selectedItems().isEmpty()) {
        QMessageBox::warning(this, "Błąd", "Proszę wybrać stację i stanowisko");
        return;
    }

    if (ostatniePomiary.isEmpty()) {
        QListWidgetItem* selectedItem = listaStanowisk->currentItem();
        if (selectedItem) {
            int id = selectedItem->data(Qt::UserRole).toInt();
            apiService->pobierzDanePomiarowe(id);
        }
        return;
    }

    QDateTime wybranaDataPoczatkowa = dataPoczatkowa->dateTime();
    QDateTime wybranaDataKoncowa = dataKoncowa->dateTime();

    QDateTime minDate, maxDate;
    bool first = true;
    foreach (const QJsonValue& val, ostatniePomiary) {
        QJsonObject pomiar = val.toObject();
        QString data = pomiar["date"].toString();
        QDateTime dateTime = QDateTime::fromString(data, Qt::ISODate);

        if (first) {
            minDate = dateTime;
            maxDate = dateTime;
            first = false;
        } else {
            if (dateTime < minDate) minDate = dateTime;
            if (dateTime > maxDate) maxDate = dateTime;
        }
    }

    if (wybranaDataPoczatkowa < minDate || wybranaDataKoncowa > maxDate) {
        QString komunikat = QString("Wybrany zakres dat jest poza dostępnymi danymi.\n"
                                    "Dostępny zakres: %1 - %2")
                                .arg(minDate.toString("yyyy-MM-dd HH:mm"))
                                .arg(maxDate.toString("yyyy-MM-dd HH:mm"));
        QMessageBox::warning(this, "Błąd zakresu dat", komunikat);

        dataPoczatkowa->setDateTime(minDate);
        dataKoncowa->setDateTime(maxDate);
        return;
    }

    if (wybranaDataPoczatkowa > wybranaDataKoncowa) {
        QMessageBox::warning(this, "Błąd zakresu dat", "Data początkowa nie może być późniejsza niż data końcowa");
        return;
    }

    wyswietlWykres(ostatniePomiary, ostatniParametrKod);
}

/**
 * @brief Wyświetla listę stacji w interfejsie oraz na mapie.
 * @param stacje Tablica JSON zawierająca dane stacji.
 *
 * Obsługuje także filtrowanie po mieście lub promieniu, w zależności od aktywnego trybu.
 */
void MainWindow::wyswietlStacje(const QJsonArray& stacje) {
    listaStacji->clear();

    bool filtrujPoMiescie = !poleMiasto->text().trimmed().isEmpty() &&
                            (poleMiasto->hasFocus() || przyciskFiltrujStacje->isDown());
    bool filtrujPoPromieniu = !poleLokalizacja->text().trimmed().isEmpty() &&
                              (poleLokalizacja->hasFocus() || przyciskSzukajWPromieniu->isDown());

    QString filtrMiasto = poleMiasto->text().trimmed();
    QJsonArray stacjeDoWyswietlenia;

    foreach (const QJsonValue& val, stacje) {
        QJsonObject stacja = val.toObject();
        QString nazwa = stacja["stationName"].toString();
        QString miasto = stacja["city"].toObject()["name"].toString();
        int id = stacja["id"].toInt();

        if (filtrujPoMiescie && !miasto.contains(filtrMiasto, Qt::CaseInsensitive)) {
            continue;
        }

        stacjeDoWyswietlenia.append(stacja);

        QListWidgetItem *item = new QListWidgetItem(nazwa + " (" + miasto + ")");
        item->setData(Qt::UserRole, id);
        listaStacji->addItem(item);
    }

    rysujMapePolski(stacjeDoWyswietlenia);
}


void MainWindow::wyswietlStanowiska(const QJsonArray& stanowiska) {
    listaStanowisk->clear();

    foreach (const QJsonValue& val, stanowiska) {
        QJsonObject stanowisko = val.toObject();
        QString paramName = stanowisko["param"].toObject()["paramName"].toString();
        int id = stanowisko["id"].toInt();

        QListWidgetItem *item = new QListWidgetItem(paramName);
        item->setData(Qt::UserRole, id);
        listaStanowisk->addItem(item);
    }
}

/**
 * @brief Wyświetla listę stanowisk dla wybranej stacji.
 * @param stanowiska Tablica JSON zawierająca dane stanowisk.
 */
void MainWindow::wyswietlPomiary(const QJsonArray& pomiary, const QString& parametrKod) {
    ostatniePomiary = pomiary;
    ostatniParametrKod = parametrKod;

    listaPomiarow->clear();
    listaPomiarow->addItem("Parametr: " + parametrKod);

    if (pomiary.isEmpty()) {
        listaPomiarow->addItem("Brak danych pomiarowych");
        dataPoczatkowa->setDateTime(QDateTime());
        dataKoncowa->setDateTime(QDateTime());
        return;
    }

    QDateTime minDate, maxDate;
    bool first = true;

    foreach (const QJsonValue& val, pomiary) {
        QJsonObject pomiar = val.toObject();
        QString data = pomiar["date"].toString();
        QDateTime dateTime = QDateTime::fromString(data, Qt::ISODate);

        if (first) {
            minDate = dateTime;
            maxDate = dateTime;
            first = false;
        } else {
            if (dateTime < minDate) minDate = dateTime;
            if (dateTime > maxDate) maxDate = dateTime;
        }
    }

    if (dataPoczatkowa->dateTime().isNull() || dataKoncowa->dateTime().isNull() ||
        dataPoczatkowa->dateTime() < minDate || dataKoncowa->dateTime() > maxDate) {
        dataPoczatkowa->setDateTime(minDate);
        dataKoncowa->setDateTime(maxDate);
    }

    foreach (const QJsonValue& val, pomiary) {
        QJsonObject pomiar = val.toObject();
        QString data = pomiar["date"].toString();
        QString wartosc = pomiar["value"].isNull() ? "Brak danych" : QString::number(pomiar["value"].toDouble());
        listaPomiarow->addItem(data + ": " + wartosc);
    }

    wyswietlWykres(pomiary, parametrKod);
}

/**
 * @brief Wyświetla poziom indeksu jakości powietrza.
 *
 * @param indeks Obiekt JSON zawierający informacje o poziomie indeksu.
 */
void MainWindow::wyswietlIndeks(const QJsonObject& indeks) {
    if (indeks.contains("stIndexLevel")) {
        QJsonObject poziom = indeks["stIndexLevel"].toObject();
        QString nazwa = poziom["indexLevelName"].toString();
        indeksPowietrzaLabel->setText("Indeks jakości powietrza: " + nazwa);
    } else {
        indeksPowietrzaLabel->setText("Indeks jakości powietrza: Brak danych");
    }
}

/**
 * @brief Wyświetla komunikat o błędzie w formie okna dialogowego.
 *
 * @param blad Treść błędu do wyświetlenia.
 */
void MainWindow::wyswietlBlad(const QString& blad) {
    QMessageBox::critical(this, "Błąd", blad);
}

/**
 * @brief Tworzy i wyświetla wykres z danych pomiarowych dla danego parametru.
 *
 * @param dane Tablica JSON z pomiarami.
 * @param parametrKod Kod parametru (np. PM10, NO2).
 */
void MainWindow::wyswietlWykres(const QJsonArray& dane, const QString& parametrKod) {
    if (dane.isEmpty()) {
        widokWykresu->setVisible(false);
        return;
    }

    widokWykresu->setVisible(true);

    QLineSeries *series = new QLineSeries();
    series->setName(parametrKod);

    QDateTime startDate = dataPoczatkowa->dateTime();
    QDateTime endDate = dataKoncowa->dateTime();

    QVector<QPointF> points;
    foreach (const QJsonValue& val, dane) {
        QJsonObject pomiar = val.toObject();
        if (!pomiar["value"].isNull()) {
            QString data = pomiar["date"].toString();
            QDateTime dateTime = QDateTime::fromString(data, Qt::ISODate);
            if (dateTime >= startDate && dateTime <= endDate) {
                double value = pomiar["value"].toDouble();
                points.append(QPointF(dateTime.toMSecsSinceEpoch(), value));
            }
        }
    }

    std::sort(points.begin(), points.end(), [](const QPointF &a, const QPointF &b) {
        return a.x() < b.x();
    });

    series->append(points);

    QChart *chart = new QChart();
    chart->addSeries(series);

    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setFormat("yyyy-MM-dd HH:mm");
    axisX->setTitleText("Czas");
    axisX->setRange(startDate, endDate);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis;
    axisY->setTitleText("Wartość");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->setTitle("Wykres danych pomiarowych: " + parametrKod +
                    "\nZakres: " + startDate.toString("yyyy-MM-dd HH:mm") +
                    " - " + endDate.toString("yyyy-MM-dd HH:mm"));
    chart->legend()->setVisible(true);

    widokWykresu->setChart(chart);
}

/**
 * @brief Obsługuje kliknięcie przycisku "Szukaj w promieniu".
 *
 * Inicjuje wyszukiwanie stacji pomiarowych w zadanym promieniu od lokalizacji.
 */
void MainWindow::on_szukajWPromieniu_clicked() {
    QString lokalizacja = poleLokalizacja->text().trimmed();
    if (lokalizacja.isEmpty()) {
        QMessageBox::warning(this, "Błąd", "Proszę wpisać lokalizację");
        return;
    }

    bool ok;
    double promien = polePromien->text().toDouble(&ok);
    if (!ok || promien <= 0) {
        QMessageBox::warning(this, "Błąd", "Proszę wpisać poprawny promień");
        return;
    }


    if (listaStanowisk) listaStanowisk->clear();
    if (listaPomiarow) listaPomiarow->clear();
    if (widokWykresu) {
        QChart* pustyWykres = new QChart();
        widokWykresu->setChart(pustyWykres);
    }

    poleLokalizacja->setFocus();
    poleMiasto->clear();
    apiService->znajdzStacjeWPromieniu(lokalizacja, promien);
}

/**
 * @brief Rysuje mapę Polski z naniesionymi stacjami pomiarowymi.
 *
 * @param stacje Tablica JSON zawierająca dane stacji.
 */
void MainWindow::rysujMapePolski(const QJsonArray& stacje) {
    scenaMapy->clear();

    QPixmap mapaPixmap("kontur/poland.png");
    if (mapaPixmap.isNull()) {
        qDebug() << "Nie udało się wczytać mapy!";
        return;
    }

    QGraphicsPixmapItem* mapaItem = new QGraphicsPixmapItem(mapaPixmap);
    scenaMapy->addItem(mapaItem);
    scenaMapy->setSceneRect(mapaPixmap.rect());

    widokMapy->fitInView(mapaItem, Qt::KeepAspectRatio);

    const double minLat = 49.0;
    const double maxLat = 54.9;
    const double minLon = 14.1;
    const double maxLon = 24.2;

    double mapWidth = mapaPixmap.width();
    double mapHeight = mapaPixmap.height();

    for (const QJsonValue& val : stacje) {
        QJsonObject stacja = val.toObject();
        double lat = stacja["gegrLat"].toString().toDouble();
        double lon = stacja["gegrLon"].toString().toDouble();

        double x = (lon - minLon) / (maxLon - minLon) * mapWidth;
        double y = (1.0 - (lat - minLat) / (maxLat - minLat)) * mapHeight;

        QGraphicsEllipseItem* kolo = scenaMapy->addEllipse(x - 4, y - 4, 8, 8, QPen(Qt::blue), QBrush(Qt::blue));

        kolo->setToolTip(stacja["stationName"].toString() + "\n" + stacja["city"].toObject()["name"].toString());

        kolo->setData(Qt::UserRole, stacja["id"].toInt());

        kolo->setAcceptHoverEvents(true);
        kolo->setFlag(QGraphicsItem::ItemIsSelectable, true);
        kolo->setFlag(QGraphicsItem::ItemIsFocusable, true);
        kolo->setCursor(Qt::PointingHandCursor);
    }
}

/**
 * @brief Obsługuje kliknięcie przycisku przełączającego widoczność mapy.
 */
void MainWindow::on_toggleMapa_clicked() {
    bool mapaJestWidoczna = (przyciskPokazMape->text() == "Schowaj mapę");

    if (mapaJestWidoczna) {
        przyciskPokazMape->setText("Pokaż mapę");
        widokMapy->hide();
        widokWykresu->show();
    } else {
        przyciskPokazMape->setText("Schowaj mapę");
        widokWykresu->hide();
        widokMapy->show();
    }
}

/**
 * @brief Obsługuje zdarzenia interakcji użytkownika z widokiem mapy.
 *
 * @param obj Obiekt zdarzenia.
 * @param event Wskaźnik do obiektu zdarzenia.
 * @return true Jeśli zdarzenie zostało obsłużone.
 * @return false Jeśli zdarzenie nie zostało obsłużone.
 */
bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    if (obj == widokMapy->viewport() && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        QPointF scenePos = widokMapy->mapToScene(mouseEvent->pos());
        QGraphicsItem* item = scenaMapy->itemAt(scenePos, QTransform());

        if (item && item->data(Qt::UserRole).isValid()) {
            int stacjaId = item->data(Qt::UserRole).toInt();
            qDebug() << "Kliknięto stację o ID:" << stacjaId;

            for (int i = 0; i < listaStacji->count(); ++i) {
                QListWidgetItem* listItem = listaStacji->item(i);
                if (listItem->data(Qt::UserRole).toInt() == stacjaId) {
                    listaStacji->setCurrentItem(listItem);
                    listaStacji->scrollToItem(listItem);

                    on_stacjaWybrana(listItem);
                    return true;
                }
            }
        }
    }

    if ((obj == widokMapy || obj == widokMapy->viewport()) &&
        event->type() == QEvent::Wheel) {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);
        event->accept();
        const double angle = wheelEvent->angleDelta().y();
        if (angle != 0) {
            const double factor = (angle > 0) ? 1.1 : 1/1.1;
            widokMapy->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
            widokMapy->scale(factor, factor);
            currentZoomLevel *= factor;
        }
        return true;
    }

    return QMainWindow::eventFilter(obj, event);
}

/**
 * @brief Obsługuje zdarzenie przewijania kółkiem myszy.
 *
 * @param event Wskaźnik do zdarzenia kółka myszy.
 */
void MainWindow::wheelEvent(QWheelEvent *event) {
    if (widokMapy->underMouse()) {
        event->accept();
    } else {
        QMainWindow::wheelEvent(event);
    }
}

/**
 * @brief Oblicza statystyki z ostatnich pomiarów i prezentuje je w widoku.
 *
 * Statystyki obejmują wartości minimalne, maksymalne, średnie oraz trend czasowy.
 */
void MainWindow::obliczStatystyki() {
    if (ostatniePomiary.isEmpty()) {
        QMessageBox::warning(this, "Błąd", "Brak danych do obliczenia statystyk");
        return;
    }

    QJsonArray kopiaPomiary = ostatniePomiary;
    QString parametr = ostatniParametrKod;

    QtConcurrent::run([=]() {
        double minWartosc = std::numeric_limits<double>::max();
        double maxWartosc = std::numeric_limits<double>::lowest();
        QDateTime minData, maxData;
        double suma = 0;
        int liczbaPomiarow = 0;
        QList<QPair<QDateTime, double>> danePomiarowe;

        for (const QJsonValue& val : kopiaPomiary) {
            QJsonObject pomiar = val.toObject();
            if (pomiar["value"].isNull()) continue;

            double wartosc = pomiar["value"].toDouble();
            QDateTime data = QDateTime::fromString(pomiar["date"].toString(), Qt::ISODate);

            if (wartosc < minWartosc) {
                minWartosc = wartosc;
                minData = data;
            }
            if (wartosc > maxWartosc) {
                maxWartosc = wartosc;
                maxData = data;
            }

            suma += wartosc;
            liczbaPomiarow++;
            danePomiarowe.append(qMakePair(data, wartosc));
        }

        double srednia = liczbaPomiarow > 0 ? suma / liczbaPomiarow : 0;

        double trend = 0;
        QString opisTrendu = "stabilny";
        double sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;
        int n = 0;

        if (!danePomiarowe.isEmpty()) {
            QDateTime startTime = danePomiarowe.first().first;
            qint64 startEpoch = startTime.toMSecsSinceEpoch();

            for (const auto& pair : danePomiarowe) {
                double x = (pair.first.toMSecsSinceEpoch() - startEpoch) / 3600000.0;
                double y = pair.second;
                sumX += x;
                sumY += y;
                sumXY += x * y;
                sumX2 += x * x;
                n++;
            }
        }

        if (n > 0) {
            double numerator = n * sumXY - sumX * sumY;
            double denominator = n * sumX2 - sumX * sumX;
            if (denominator != 0) {
                trend = numerator / denominator;
                if (trend > 0.0001) opisTrendu = "wzrostowy";
                else if (trend < -0.0001) opisTrendu = "spadkowy";
            }
        }

        QString wynik = QString(
                            "<h3>Statystyki dla parametru: %1</h3>"
                            "<b>Minimalna wartość:</b> %2 (%3)<br>"
                            "<b>Maksymalna wartość:</b> %4 (%5)<br>"
                            "<b>Średnia wartość:</b> %6<br>"
                            "<b>Trend:</b> %7 (współczynnik: %8)<br>"
                            "<b>Liczba pomiarów:</b> %9"
                            ).arg(
                                parametr,
                                QString::number(minWartosc, 'f', 2),
                                minData.toString("yyyy-MM-dd HH:mm"),
                                QString::number(maxWartosc, 'f', 2),
                                maxData.toString("yyyy-MM-dd HH:mm"),
                                QString::number(srednia, 'f', 2),
                                opisTrendu,
                                QString::number(trend, 'e', 2),
                                QString::number(liczbaPomiarow)
                                );

        QMetaObject::invokeMethod(qApp, [=]() {
            statystykiLabel->setText(wynik);
        }, Qt::QueuedConnection);
    });
}



