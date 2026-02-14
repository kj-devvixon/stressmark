# Frequently Asked Questions (FAQ)

## General Questions

### Q: Co to jest Hardware Stress Test Suite?
**A:** To profesjonalne narzędzie do testowania stabilności hardware'u podobne do FurMark. Testuje CPU (single/multi-core), RAM, GPU oraz może testować wszystko jednocześnie. Ma nowoczesny interfejs webowy napisany w HTML/CSS, a backend w C++.

### Q: Czy to jest bezpieczne dla mojego komputera?
**A:** Program sam w sobie jest bezpieczny, ale testy obciążają hardware do 100%. Zawsze monitoruj temperatury i zatrzymaj test jeśli są zbyt wysokie (>85°C). Używaj na własną odpowiedzialność.

### Q: Czy potrzebuję uprawnień administratora?
**A:** Nie do uruchamiania programu. Potrzebujesz sudo/admin tylko do:
- Instalacji systemowej (`sudo make install`)
- Czytania niektórych sensorów temperatury
- Instalacji jako usługa systemd

### Q: Na jakich systemach działa?
**A:** 
- ✅ Linux (pełne wsparcie)
- ⚠️ Windows (podstawowa funkcjonalność, w rozwoju)
- ❓ macOS (nie testowane, może działać po kompilacji)

### Q: Czy jest to open source?
**A:** Tak! Licencja MIT - możesz robić co chcesz, pod warunkiem że zachowasz informację o licencji.

---

## Installation & Setup

### Q: Jak zainstalować?
**A:** Trzy metody:

```bash
# Metoda 1: Automatyczna instalacja
chmod +x install.sh
sudo ./install.sh

# Metoda 2: CMake
mkdir build && cd build
cmake ..
make
sudo make install

# Metoda 3: Make bezpośrednio
make
sudo make install
```

### Q: Jakie są wymagania?
**A:** Minimalne:
- Kompilator C++17 (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.10+ (opcjonalnie)
- pthread
- Moderna przeglądarka

### Q: Nie mam CMake, czy mogę zainstalować?
**A:** Tak! Użyj bezpośrednio Makefile:
```bash
make
./stress_test
```

### Q: Jak odinstalować?
**A:**
```bash
sudo ./uninstall.sh
# lub
sudo make uninstall
# lub ręcznie
sudo rm /usr/local/bin/stress_test
```

---

## Usage Questions

### Q: Jak uruchomić program?
**A:**
```bash
# Domyślnie (port 8080)
./stress_test

# Inny port
./stress_test 9000

# Następnie otwórz: http://localhost:8080
```

### Q: Który test wybrać pierwszy?
**A:** Zalecana kolejność:
1. CPU Single-Core (30 sek) - test podstawowy
2. CPU Multi-Core (60 sek) - test wszystkich rdzeni
3. RAM Test (60 sek) - test pamięci
4. Combined (30 sek) - test wszystkiego
5. Długie testy (>10 min) - po upewnieniu się że działa

### Q: Jak długi powinien być test?
**A:** Zależy od celu:
- **Szybki test**: 30-60 sekund
- **Sprawdzenie stabilności**: 5-15 minut
- **Dokładny test**: 30-60 minut
- **Ekstremalny test**: 2+ godziny

### Q: Czy mogę zatrzymać test przedwcześnie?
**A:** Tak! Kliknij "Stop Test" lub użyj API:
```bash
curl http://localhost:8080/api/stop-test
```

### Q: Co oznaczają statystyki?
**A:**
- **CPU Usage**: Procent wykorzystania CPU (0-100%)
- **RAM Usage**: Procent zajętej pamięci RAM (0-100%)
- **RAM Used MB**: Ilość użytej pamięci w megabajtach
- **CPU Temp**: Temperatura procesora w °C (0 jeśli niedostępne)

---

## Performance & Results

### Q: Jakie wyniki są normalne?
**A:**
| Test | Oczekiwane CPU | Oczekiwana RAM |
|------|---------------|----------------|
| Single-Core | 10-25% | 5-10% |
| Multi-Core | 95-100% | 10-20% |
| RAM Test | 20-40% | 60-90% |
| GPU Test | 30-60% | 10-20% |
| Combined | 95-100% | 60-90% |

### Q: Jakie temperatury są bezpieczne?
**A:**
- ✅ **< 70°C**: Doskonale
- ⚠️ **70-80°C**: Dobrze
- ⚠️ **80-85°C**: Akceptowalne (monitoruj)
- ❌ **85-95°C**: Za gorąco (lepsze chłodzenie)
- 🔥 **> 95°C**: ZATRZYMAJ TEST! Ryzyko uszkodzenia

### Q: Mój procesor throttluje, co robić?
**A:**
1. Popraw wentylację obudowy
2. Wyczyść kurz z wentylatorów
3. Wymień pastę termiczną
4. Rozważ lepszy cooler
5. Zmniejsz overclocking (jeśli jest)

### Q: Test crashuje/zamraża system, co robić?
**A:** Możliwe przyczyny:
- Niestabilny overclocking → zmniejsz taktowanie
- Przegrzanie → popraw chłodzenie
- Wadliwy RAM → przetestuj pojedyncze kości
- Problem z zasilaczem → sprawdź PSU
- Błąd w BIOS → zaktualizuj/zresetuj BIOS

---

## Technical Questions

### Q: Dlaczego temperatura nie jest wyświetlana?
**A:** Na Linuxie potrzebujesz dostępu do `/sys/class/thermal/`:
```bash
# Sprawdź czy sensory są dostępne
ls /sys/class/thermal/

# Jeśli jest "Permission denied"
sudo chmod +r /sys/class/thermal/thermal_zone*/temp

# Lub uruchom z sudo (niezalecane)
sudo ./stress_test
```

### Q: Jak działa CPU test?
**A:** Wykonuje intensywne operacje matematyczne:
- sqrt, sin, cos, pow, log
- W pętli miliony razy
- Single-core: jeden wątek
- Multi-core: po wątku na każdy rdzeń

### Q: Jak działa RAM test?
**A:** 
- Alokuje bloki pamięci (domyślnie 50MB)
- Wypełnia losowymi danymi
- Sortuje dane (intensywny dostęp do pamięci)
- Oblicza statystyki
- Utrzymuje ciągłe obciążenie pamięci

### Q: Czy GPU test używa karty graficznej?
**A:** Obecna wersja (1.0) używa CPU do symulacji obliczeń graficznych. Prawdziwy GPU test z OpenGL/Vulkan jest planowany w przyszłej wersji.

### Q: Mogę używać API programatycznie?
**A:** Tak! Zobacz [API.md](API.md) i [EXAMPLES.md](EXAMPLES.md). Przykład:
```bash
curl http://localhost:8080/api/system-info
curl "http://localhost:8080/api/start-test?type=cpu_multi&duration=60"
curl http://localhost:8080/api/test-status
```

---

## Troubleshooting

### Q: Port 8080 jest zajęty, co robić?
**A:**
```bash
# Użyj innego portu
./stress_test 9000

# Lub znajdź co używa portu
lsof -i :8080
# i zabij ten proces
kill $(lsof -t -i:8080)
```

### Q: Program się nie kompiluje
**A:**
```bash
# Sprawdź wersję kompilatora (potrzebny GCC 7+)
g++ --version

# Zainstaluj/zaktualizuj
sudo apt-get install build-essential

# Wyczyść i przebuduj
make clean
make
```

### Q: Test nie zatrzymuje się
**A:**
```bash
# Wymuś zatrzymanie przez API
curl http://localhost:8080/api/stop-test

# Lub zabij proces
pkill stress_test
```

### Q: Wyniki są nierealistyczne
**A:** Sprawdź:
- Czy inne programy nie obciążają systemu
- Czy nie działa power saving mode
- Czy BIOS nie ogranicza wydajności
- Czy cooling throttling nie jest aktywny

---

## Advanced Usage

### Q: Mogę uruchomić jako serwis systemowy?
**A:** Tak! Podczas instalacji wybierz opcję systemd service, lub:
```bash
sudo ./install.sh
# Wybierz "y" gdy zapyta o systemd service
```

### Q: Jak zautomatyzować testy?
**A:** Zobacz [EXAMPLES.md](EXAMPLES.md) dla skryptów bash/python/node.js. Przykład:
```bash
#!/bin/bash
./stress_test &
sleep 2
curl "http://localhost:8080/api/start-test?type=combined&duration=3600"
```

### Q: Mogę zmienić parametry testów?
**A:** Tak! Edytuj `src/stress_test.cpp`:
- Zmień `iterations` w CPU test
- Zmień `blockSize` w RAM test
- Zmień render resolution w GPU test

Potem przekompiluj: `make clean && make`

### Q: Czy są plany na nowe funkcje?
**A:** Tak! Zobacz [CHANGELOG.md](CHANGELOG.md) i [CONTRIBUTING.md](CONTRIBUTING.md). Planowane:
- Prawdziwy GPU test (OpenGL/Vulkan)
- Wykresy historyczne
- Export wyników
- Dark mode
- Więcej metryk
- Disk I/O test

---

## Contributing

### Q: Mogę pomóc w rozwoju?
**A:** Oczywiście! Zobacz [CONTRIBUTING.md](CONTRIBUTING.md). Potrzebujemy pomocy przy:
- Implementacji Windows
- GPU testing (OpenGL/Vulkan)
- Testach jednostkowych
- Dokumentacji
- Tłumaczeniach

### Q: Znalazłem bug, co robić?
**A:** 
1. Sprawdź czy nie został już zgłoszony
2. Stwórz issue z:
   - Opisem problemu
   - Krokami do reprodukcji
   - Informacjami o systemie
   - Logami

### Q: Mam pomysł na funkcję
**A:** Super! Otwórz issue z tagiem "enhancement" i opisz:
- Co funkcja ma robić
- Dlaczego jest potrzebna
- Przykłady użycia

---

## Security & Privacy

### Q: Czy program wysyła dane do internetu?
**A:** NIE! Program:
- Nie zbiera telemetrii
- Nie wymaga rejestracji
- Nie łączy się z internetem
- Działa tylko lokalnie

### Q: Czy dane są bezpieczne?
**A:** Program nie zapisuje żadnych danych poza logami (jeśli włączone). Wszystko działa w pamięci.

### Q: Znalazłem lukę bezpieczeństwa
**A:** Przeczytaj [SECURITY.md](SECURITY.md) i zgłoś prywatnie (nie przez publiczne issue).

---

## License & Legal

### Q: Jaka jest licencja?
**A:** MIT License - bardzo permisywna. Możesz:
- ✅ Używać komercyjnie
- ✅ Modyfikować
- ✅ Dystrybuować
- ✅ Sublicencjonować

Musisz tylko:
- ℹ️ Zachować copyright notice
- ℹ️ Dołączyć kopię licencji

### Q: Mogę użyć w komercyjnym projekcie?
**A:** Tak! MIT License na to pozwala.

### Q: Kto odpowiada za szkody?
**A:** Zgodnie z MIT License - software is provided "AS IS". Używasz na własną odpowiedzialność.

---

## Support

### Q: Gdzie mogę znaleźć pomoc?
**A:**
1. Przeczytaj dokumentację:
   - [README.md](README.md) - pełna dokumentacja
   - [QUICKSTART.md](QUICKSTART.md) - szybki start
   - [EXAMPLES.md](EXAMPLES.md) - przykłady użycia
   - [API.md](API.md) - dokumentacja API

2. Sprawdź istniejące issues na GitHub

3. Stwórz nowe issue jeśli problemu nie ma

### Q: Jak mogę się skontaktować z autorem?
**A:** Przez GitHub repository - stwórz issue lub dyskusję.

---

**Masz inne pytanie?** Stwórz issue na GitHubie!

**MIT License © 2026 kj-devvixon**
