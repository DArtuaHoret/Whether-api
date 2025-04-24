#include "jpo_projekt.h"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <iostream>

Jpo_projekt::Jpo_projekt(QWidget *parent)
    : QMainWindow(parent)
{
    // Tworzymy główny widget i layout
    centralnyWidget = new QWidget(this);
    layout = new QVBoxLayout(centralnyWidget);

    // Tworzymy pole tekstowe
    poleTekstowe = new QLineEdit(this);
    poleTekstowe->setPlaceholderText("Wpisz liczbę k");
    layout->addWidget(poleTekstowe);

    // Tworzymy przycisk
    przycisk = new QPushButton("Zatwierdź", this);
    layout->addWidget(przycisk);

    // Tworzymy etykietę (na wynik)
    etykieta = new QLabel(this);
    layout->addWidget(etykieta);

    // Łączymy kliknięcie przycisku z funkcją `zatwierdz()`
    connect(przycisk, &QPushButton::clicked, this, &Jpo_projekt::zatwierdz);

    // Ustawiamy layout jako centralną zawartość okna
    centralnyWidget->setLayout(layout);
    setCentralWidget(centralnyWidget);

    QPushButton *przyciskPogody = new QPushButton("Pobierz pogodę", this);
    layout->addWidget(przyciskPogody);

    connect(przyciskPogody, &QPushButton::clicked, this, &Jpo_projekt::pobierzPogode);
}

Jpo_projekt::~Jpo_projekt() {}

void Jpo_projekt::zatwierdz()
{
    // Pobierz tekst z pola
    QString tekst = poleTekstowe->text();

    // Spróbuj skonwertować do liczby
    bool ok;
    int k = tekst.toInt(&ok);

    // Jeśli udało się skonwertować, pokaż wynik
    if (ok) {
        etykieta->setText("Wartość k = " + QString::number(k));
    } else {
        etykieta->setText("Niepoprawna liczba!");
    }
}

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <iostream>

void Jpo_projekt::pobierzPogode()
{
    namespace beast = boost::beast;
    namespace http = beast::http;
    namespace net = boost::asio;
    using tcp = net::ip::tcp;

    try {
        net::io_context ioc;
        tcp::resolver resolver(ioc);
        tcp::socket socket(ioc);

        auto const results = resolver.resolve("api.open-meteo.com", "80");
        net::connect(socket, results.begin(), results.end());

        http::request<http::string_body> req(http::verb::get,
                                             "/v1/forecast?latitude=52.400276&longitude=16.955308&current=temperature_2m,precipitation,weather_code,surface_pressure,wind_speed_10m",
                                             11);
        req.set(http::field::host, "api.open-meteo.com");
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        http::write(socket, req);

        beast::flat_buffer buffer;
        http::response<http::dynamic_body> res;
        http::read(socket, buffer, res);

        std::string wynik = beast::buffers_to_string(res.body().data());
        etykieta->setText(QString::fromStdString(wynik)); // Pokaż wynik w GUI

        beast::error_code ec;
        socket.shutdown(tcp::socket::shutdown_both, ec);
    }
    catch (std::exception const& e) {
        etykieta->setText(QString("Błąd: ") + e.what());
    }
}

