================================================================================
                    HARDWARE STRESS TEST SUITE v1.0
                    MIT License (c) 2026 kj-devvixon
================================================================================

SZYBKI START:

  Linux:
    1. make
    2. ./stress_test
    3. Otwórz: http://localhost:8080

  Windows:
    1. build.bat
    2. build\Release\stress_test.exe
    3. Otwórz: http://localhost:8080

FUNKCJE:
  
  ✓ CPU Test (Single-Core)  - Test pojedynczego rdzenia
  ✓ CPU Test (Multi-Core)   - Test wszystkich rdzeni
  ✓ RAM Test                - Test pamięci (jak memtest)
  ✓ GPU Stress Test         - Test karty graficznej
  ✓ Combined Test           - Test wszystkiego jednocześnie
  ✓ Real-time Monitoring    - Monitorowanie w czasie rzeczywistym
  ✓ Web Interface           - Nowoczesny interfejs HTML/CSS

DOKUMENTACJA:

  README.md         - Pełna dokumentacja
  QUICKSTART.md     - Szybki start (polecam!)
  FAQ.md            - Często zadawane pytania
  EXAMPLES.md       - Przykłady użycia
  API.md            - Dokumentacja API
  CONTRIBUTING.md   - Jak pomóc w rozwoju
  SECURITY.md       - Polityka bezpieczeństwa
  CHANGELOG.md      - Historia zmian

WYMAGANIA:

  - C++17 compiler (GCC 7+, Clang 5+, MSVC 2017+)
  - CMake 3.10+ (opcjonalnie)
  - pthread
  - Moderna przeglądarka

INSTALACJA:

  Automatyczna:
    chmod +x install.sh
    sudo ./install.sh

  Ręczna:
    make
    sudo make install

  CMake:
    mkdir build && cd build
    cmake ..
    make
    sudo make install

UŻYCIE:

  Podstawowe:
    ./stress_test                 # Port 8080
    ./stress_test 9000           # Własny port
    
  Testy:
    1. Otwórz http://localhost:8080
    2. Wybierz typ testu
    3. Ustaw czas trwania
    4. Kliknij "Start Test"
    5. Obserwuj statystyki

  Przez API:
    curl http://localhost:8080/api/system-info
    curl "http://localhost:8080/api/start-test?type=cpu_multi&duration=60"
    curl http://localhost:8080/api/test-status
    curl http://localhost:8080/api/stop-test

BEZPIECZEŃSTWO:

  ⚠️ WAŻNE:
    - Zacznij od krótkich testów (30 sek)
    - Monitoruj temperaturę
    - Zatrzymaj test jeśli temp > 85°C
    - Upewnij się że masz dobre chłodzenie
    - Zapisz dokumenty przed testem

  Bezpieczne temperatury:
    < 70°C   - Doskonale
    70-80°C  - Dobrze
    80-85°C  - OK (monitoruj)
    > 85°C   - ZATRZYMAJ TEST!

STRUKTURA PROJEKTU:

  hardware-stress-test/
  ├── include/
  │   └── stress_test.h         # Header pliki
  ├── src/
  │   ├── main.cpp              # Główny plik + HTTP serwer
  │   └── stress_test.cpp       # Implementacja testów
  ├── build/                    # Pliki kompilacji
  ├── CMakeLists.txt            # Konfiguracja CMake
  ├── Makefile                  # Makefile
  ├── build.sh / build.bat      # Skrypty budowania
  ├── install.sh / uninstall.sh # Skrypty instalacji
  ├── config.ini                # Przykładowa konfiguracja
  └── *.md                      # Dokumentacja

POMOCNE KOMENDY:

  Budowanie:
    make                        # Zbuduj
    make clean                  # Wyczyść
    make install                # Zainstaluj (sudo)
    make run                    # Zbuduj i uruchom

  Debugging:
    make debug                  # Build z debug symbols
    valgrind ./stress_test      # Sprawdź memory leaks
    gdb ./stress_test           # Debug z GDB

  Testowanie:
    lsof -i :8080              # Sprawdź port
    curl localhost:8080/api/system-info  # Test API

ROZWIĄZYWANIE PROBLEMÓW:

  Port zajęty:
    ./stress_test 9000         # Użyj innego portu
    
  Nie kompiluje się:
    g++ --version              # Sprawdź wersję (potrzeba 7+)
    make clean && make         # Wyczyść i przebuduj
    
  Brak temperatury:
    ls /sys/class/thermal/     # Sprawdź sensory
    sudo ./stress_test         # Uruchom z sudo (niezalecane)

  Test nie działa:
    curl localhost:8080/api/test-status  # Sprawdź status
    pkill stress_test          # Zabij proces

WKŁAD:

  Chcesz pomóc? Super!
    - Zgłaszaj błędy
    - Sugeruj funkcje
    - Twórz pull requesty
    - Poprawiaj dokumentację
    
  Zobacz CONTRIBUTING.md dla szczegółów.

CHANGELOG:

  v1.0.0 (2026-02-14) - Pierwsze wydanie
    - CPU single/multi-core tests
    - RAM stress test
    - GPU stress test (basic)
    - Combined test
    - Web interface
    - Real-time monitoring
    - System info detection
    - Cross-platform (Linux/Windows)

PLANY NA PRZYSZŁOŚĆ:

  - Prawdziwy GPU test (OpenGL/Vulkan)
  - Wykresy w czasie rzeczywistym
  - Export wyników (CSV/JSON)
  - Dark mode
  - Disk I/O test
  - Network test
  - Benchmarking scores

LICENCJA:

  MIT License - bardzo permisywna
  
  Możesz:
    ✓ Używać komercyjnie
    ✓ Modyfikować
    ✓ Dystrybuować
    ✓ Sublicencjonować
    
  Musisz:
    • Zachować copyright notice
    • Dołączyć kopię licencji

  Software is provided "AS IS" - brak gwarancji.
  Używasz na własną odpowiedzialność.

KONTAKT:

  GitHub: Zobacz repozytorium projektu
  Issues: Dla bugów i feature requests
  Dyskusje: Dla pytań i pomocy

PODZIĘKOWANIA:

  Dziękuję wszystkim którzy:
    - Testują program
    - Zgłaszają błędy
    - Sugerują funkcje
    - Tworzą pull requesty
    - Dzielą się projektem

LINKI:

  Dokumentacja online: [TODO: GitHub pages]
  Repository: [TODO: GitHub link]
  Issues: [TODO: GitHub issues]
  Releases: [TODO: GitHub releases]

================================================================================

OSTATNIA UWAGA:

To narzędzie może mocno obciążyć twój hardware. Używaj odpowiedzialnie:

  ✓ Zacznij od krótkich testów
  ✓ Monitoruj temperatury
  ✓ Miej dobre chłodzenie
  ✓ Nie zostawiaj bez nadzoru
  ✓ Zatrzymaj jeśli coś jest nie tak

Autor nie ponosi odpowiedzialności za uszkodzenia hardware'u.

================================================================================

Miłego testowania! 🔥

MIT License © 2026 kj-devvixon

================================================================================
