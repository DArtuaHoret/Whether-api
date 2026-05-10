### English below

### W linku do dysku jest sama aplikacja do pobrania


Opis projektu:

Aplikacja umożliwia monitorowanie jakości powietrza w różnych miejscach w Polsce. Dzięki niej użytkownk może sprawdzić dane o jakości powietrza w czasie rzeczywistym, 
a także uzyskać informacje o poziomach zanieczyszczeń i indeksie jakości powietrza. Aplikacja korzysta z danych dostarczanych przez Główny Inspektorat Ochrony Środowiska (GIOŚ), które są pobierane przez API.

Funkcjonalności:

    Wyszukiwanie stacji pomiarowych: Możliwość przeglądania dostępnych stacji w wybranym mieście.

    Indeks jakości powietrza: Obliczanie i wyświetlanie indeksu jakości powietrza na podstawie danych pomiarowych.

    Wyświetlanie danych w czasie rzeczywistym: Pobieranie i prezentowanie danych o poziomach zanieczyszczeń.

    Wyszukiwanie stacji w promieniu: Możliwość znalezienia stacji w zadanym promieniu od podanej lokalizacji.

    Zapis i odczyt danych: Możliwość zapisywania i wczytywania danych z plików JSON w celu ich późniejszego wykorzystania.

Dostępne 4 opcje wyboru stacji:

    Można pobrać wszystkie stacje;
   
    Można filtrować po mieście;

    Można wpisać lokalizację i promień dla znajdowania stacji w tym otoczeniu;
   (Dostępne formaty:
	Polanka 3, Poznań - ulica z budynkiem, miasto;
	60-695 Poznań, Polanka 3 - z kodem pocztowym;
	Poznań, Polanka - miasto  i budynek
   )

   Można wybrać stacje na interaktywnej mapie (mapę można schować)


Następnie należy wybrać stacje, stanowisko i ,dodatkowo, można wybrać zakres pomiarów. Również można policzyć statystyki pomiarów.


URUCHOMIENIA TESTOW JEDNOSTKOWYCH:
W folderze z projektem w sekcji lokalizacji foldera należy wpisać "cmd" , a w otworzonym oknie wpisać "ProjektTests.exe" dla urochomieniu testów.

DOKUMENTACJA:
W folderze html znajduje się dokumentacja proejktu (DOxygen).



MADE BY ARTUR HORETSKYI 

------------------------------------------------------------


### The download link contains the application executable files


## Project Description

The application allows users to monitor air quality in different locations across Poland. It enables users to check real-time air quality data, as well as obtain information about pollution levels and the Air Quality Index (AQI). The application uses data provided by the Chief Inspectorate of Environmental Protection (GIOŚ), retrieved through an API.

## Features

- **Measurement station search:**  
  Browse available monitoring stations in a selected city.

- **Air Quality Index:**  
  Calculate and display the Air Quality Index based on measurement data.

- **Real-time data display:**  
  Retrieve and present real-time pollution level data.

- **Radius-based station search:**  
  Find stations within a specified radius from a given location.

- **Data saving and loading:**  
  Save and load data from JSON files for later use.

## Available station selection methods

There are 4 available options for selecting stations:

1. Download all available stations;

2. Filter stations by city;

3. Enter a location and search radius to find nearby stations;  
   Supported formats:
   - `Polanka 3, Poznań` — street + building number, city
   - `60-695 Poznań, Polanka 3` — with postal code
   - `Poznań, Polanka` — city and street/building

4. Select stations directly on an interactive map  
   (the map can be hidden if needed).

After selecting a station, the user can choose a measuring point and optionally define a measurement range. The application also allows calculation of measurement statistics.

---

## Running Unit Tests

In the project folder, type `cmd` in the folder path bar to open the command prompt.  
Then run:

```bash
ProjektTests.exe
```

## Documentation

Project documentation generated with Doxygen is available in the html folder.
