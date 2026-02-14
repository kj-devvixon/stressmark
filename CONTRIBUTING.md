# Contributing to Hardware Stress Test Suite

Dziękuję za zainteresowanie projektem! 🎉

## 🤝 Jak Pomóc

### Zgłaszanie Błędów

Jeśli znalazłeś bug:

1. Sprawdź czy bug nie został już zgłoszony
2. Stwórz nowy issue z:
   - Opisem problemu
   - Krokami do reprodukcji
   - Oczekiwanym zachowaniem
   - Aktualnym zachowaniem
   - Informacjami o systemie (OS, kompilator, etc.)
   - Logami jeśli dostępne

### Sugerowanie Nowych Funkcji

Masz pomysł na nową funkcję?

1. Sprawdź czy nie została już zasugerowana
2. Otwórz issue z tagiem "enhancement"
3. Opisz:
   - Jak funkcja powinna działać
   - Dlaczego jest potrzebna
   - Przykłady użycia

### Pull Requests

#### Przed rozpoczęciem:

1. Fork repozytorium
2. Stwórz nowy branch (`git checkout -b feature/amazing-feature`)
3. Skonfiguruj środowisko deweloperskie

#### Podczas pracy:

1. Pisz czysty, czytelny kod
2. Komentuj skomplikowane fragmenty
3. Trzymaj się obecnego stylu kodu
4. Testuj swoje zmiany
5. Commituj często z opisowymi wiadomościami

#### Przed submitem:

1. Upewnij się że kod się kompiluje
2. Przetestuj na swojej maszynie
3. Zaktualizuj dokumentację jeśli potrzeba
4. Dodaj opis zmian

#### Submit:

1. Push do swojego fork'a
2. Otwórz Pull Request
3. Opisz zmiany w szczegółach
4. Linkuj powiązane issues

## 📝 Styl Kodu

### C++

```cpp
// Używaj camelCase dla zmiennych i funkcji
int myVariable = 0;
void myFunction() { }

// PascalCase dla klas
class MyClass { };

// UPPER_CASE dla stałych
const int MAX_VALUE = 100;

// Wcięcia: 4 spacje
void example() {
    if (condition) {
        doSomething();
    }
}

// Nawiasy klamrowe zawsze, nawet dla single-line
if (test) {
    doThis();
}

// Komentarze dla skomplikowanych fragmentów
// Calculate the average of the array
double avg = sum / count;
```

### HTML/CSS/JavaScript

```javascript
// camelCase dla JS
let myVariable = 0;
function myFunction() { }

// kebab-case dla CSS classes
.my-class-name { }

// 2 spacje dla wcięć w HTML/CSS/JS
function example() {
  if (condition) {
    doSomething();
  }
}
```

## 🔍 Co Potrzebuje Pomocy

### Wysokie Priorytety:

- [ ] Support dla Windows (pełna implementacja)
- [ ] Prawdziwy GPU stress test (OpenGL/Vulkan)
- [ ] Wykresy historyczne w czasie rzeczywistym
- [ ] Export wyników do CSV/JSON
- [ ] Testy jednostkowe
- [ ] CI/CD pipeline

### Średnie Priorytety:

- [ ] Więcej metryk systemowych
- [ ] Konfigurowalny poziom intensywności testów
- [ ] Dark mode dla interfejsu
- [ ] Wsparcie dla innych architektur (ARM)
- [ ] Profil użytkownika z saved tests

### Niskie Priorytety:

- [ ] Więcej motywów kolorystycznych
- [ ] Sounds/notifications
- [ ] Mobile app wrapper
- [ ] Plugin system

## 🧪 Testowanie

### Przed commitem:

```bash
# Build test
make clean
make

# Run podstawowych testów
./stress_test &
sleep 2
curl http://localhost:8080/api/system-info
curl "http://localhost:8080/api/start-test?type=cpu_single&duration=5"
sleep 6
curl http://localhost:8080/api/test-status
```

### Manual Testing Checklist:

- [ ] Kompiluje się bez warningów
- [ ] Wszystkie testy uruchamiają się
- [ ] Można zatrzymać test przedwcześnie
- [ ] Statystyki aktualizują się w czasie rzeczywistym
- [ ] Interface reaguje na kliknięcia
- [ ] Nie ma memory leaks (sprawdź z valgrind)

## 📚 Struktura Projektu

```
hardware-stress-test/
├── include/           # Header files
│   └── stress_test.h  # Main header
├── src/               # Source files
│   ├── main.cpp       # Entry point + HTTP server
│   └── stress_test.cpp # Test implementations
├── build/             # Build artifacts (generated)
├── docs/              # Documentation (future)
└── tests/             # Unit tests (future)
```

## 🎯 Obszary do Rozwoju

### Backend (C++)
- System monitoring (więcej metryk)
- Test algorithms (optymalizacja)
- Cross-platform support
- Error handling
- Logging system

### Frontend (HTML/CSS/JS)
- More interactive graphs
- Better mobile support
- Accessibility improvements
- Internationalization
- Progressive Web App features

### DevOps
- Automated builds
- Unit tests
- Integration tests
- Performance benchmarks
- Code coverage

## 💬 Komunikacja

- Używaj Issues do dyskusji o funkcjach
- Bądź uprzejmy i konstruktywny
- Pomóż innym contributors
- Dokumentuj złożone decyzje

## 📄 Licencja

Wszystkie contributions są pod licencją MIT, tak jak główny projekt.

## 🌟 Uznania

Wszyscy contributors będą dodani do README.md w sekcji acknowledgments.

---

**Dzięki za pomoc w rozwoju projektu!** 🚀

*MIT License © 2026 kj-devvixon*
