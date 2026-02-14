# Quick Start Guide - Hardware Stress Test Suite

## 🚀 Super Szybki Start

### Linux

```bash
# 1. Przejdź do katalogu projektu
cd hardware-stress-test

# 2. Zbuduj projekt (wybierz jedną metodę)

# METODA A - Użyj skryptu build
chmod +x build.sh
./build.sh

# METODA B - CMake
mkdir build && cd build
cmake ..
make
./stress_test

# METODA C - Make bezpośrednio
make
./stress_test

# 3. Otwórz przeglądarkę
# http://localhost:8080
```

### Windows

```batch
# 1. Otwórz Visual Studio Developer Command Prompt

# 2. Przejdź do katalogu projektu
cd hardware-stress-test

# 3. Uruchom skrypt build
build.bat

# 4. Otwórz przeglądarkę
# http://localhost:8080
```

## 📦 Minimalna Instalacja (Linux)

Jeśli nie masz zainstalowanych narzędzi:

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install build-essential cmake

# Fedora/RHEL
sudo dnf install gcc-c++ cmake make

# Arch Linux
sudo pacman -S base-devel cmake
```

## 🎮 Jak Używać

1. **Uruchom aplikację**
   ```bash
   ./stress_test
   # lub na innym porcie:
   ./stress_test 9000
   ```

2. **Otwórz przeglądarkę**
   - Wejdź na: `http://localhost:8080`

3. **Wybierz test**
   - CPU Single-Core - Test pojedynczego rdzenia
   - CPU Multi-Core - Test wszystkich rdzeni
   - RAM Stress - Test pamięci RAM
   - GPU Stress - Test karty graficznej
   - Combined - Test wszystkiego naraz

4. **Ustaw czas trwania**
   - Domyślnie: 60 sekund
   - Zalecane dla pierwszego razu: 30 sekund

5. **Kliknij "Start Test"**

6. **Obserwuj statystyki w czasie rzeczywistym:**
   - CPU Usage (zużycie procesora)
   - RAM Usage (zużycie pamięci)
   - RAM Used (MB) (użyta pamięć w MB)
   - CPU Temp (temperatura procesora)

## ⚠️ Ważne Ostrzeżenia

- **Zacznij od krótkiego testu** (30 sekund) żeby sprawdzić czy wszystko działa
- **Monitoruj temperaturę** - zatrzymaj test jeśli temperatura jest zbyt wysoka
- **Upewnij się że masz dobre chłodzenie** przed długimi testami
- **Zapisz wszystkie dokumenty** przed uruchomieniem testu
- **Combined Test jest najcięższy** - używaj ostrożnie

## 🐛 Rozwiązywanie Problemów

### Port zajęty

```bash
# Użyj innego portu
./stress_test 9000
```

### Nie pokazuje temperatury

```bash
# Sprawdź czy masz dostęp do thermal zones
ls /sys/class/thermal/
```

### Błędy kompilacji

```bash
# Sprawdź wersję kompilatora
g++ --version

# Musi być GCC 7+ lub równoważny
```

## 📊 Przykładowe Wyniki

### Bezpieczne parametry do testów:
- **Krótki test**: 30-60 sekund
- **Średni test**: 5-10 minut
- **Długi test**: 30+ minut (tylko z dobrym chłodzeniem!)

### Typowe wartości CPU:
- **Idle**: 5-15% użycia
- **Single-Core Test**: 10-25% (zależy od liczby rdzeni)
- **Multi-Core Test**: 95-100% (wszystkie rdzenie)

### Typowe temperatury (zależne od systemu chłodzenia):
- **Bezpieczne**: < 80°C
- **Wysokie**: 80-90°C
- **Krytyczne**: > 90°C (ZATRZYMAJ TEST!)

## 🎯 Wskazówki

1. **Przed testem:**
   - Zamknij inne aplikacje
   - Sprawdź czy wentylatory działają
   - Upewnij się że laptop jest na twardej powierzchni (nie na łóżku!)

2. **Podczas testu:**
   - Obserwuj temperatury
   - Słuchaj czy wentylatory pracują
   - Sprawdzaj czy komputer nie zwolnił znacząco

3. **Po teście:**
   - Poczekaj aż temperatury spadną
   - Sprawdź czy wszystko działa normalnie
   - Zapisz wyniki jeśli testujesz wydajność

## 💡 FAQ

**Q: Czy mogę uszkodzić komputer tym programem?**  
A: Program sam w sobie jest bezpieczny, ale ekstremalne testy mogą podnieść temperatury. Zawsze monitoruj temperatury i zatrzymaj test jeśli są zbyt wysokie.

**Q: Dlaczego temperatura nie jest pokazywana?**  
A: Na niektórych systemach dostęp do czujników temperatury wymaga uprawnień root lub specjalnej konfiguracji.

**Q: Czy mogę uruchomić test w tle?**  
A: Tak, ale pamiętaj że będzie to obciążać system i wpływać na inne aplikacje.

**Q: Jaki test wybrać pierwszy?**  
A: Zacznij od CPU Single-Core na 30 sekund, następnie Multi-Core, potem RAM, a na końcu Combined.

**Q: Czy mogę dostosować testy?**  
A: Tak! Edytuj plik `src/stress_test.cpp` i przekompiluj.

## 📞 Pomoc

Jeśli masz problemy:
1. Sprawdź sekcję Troubleshooting w README.md
2. Upewnij się że masz wszystkie zależności
3. Sprawdź logi w terminalu
4. Uruchom z flagą debug

---

**Miłego testowania! 🔥**

*MIT License © 2026 kj-devvixon*
