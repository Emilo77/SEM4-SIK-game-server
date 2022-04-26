# 1. Gra Bombowe roboty

### 1.1. Zasady gry

Tegoroczne duże zadanie zaliczeniowe polega na napisaniu gry sieciowej -
uproszczonej wersji gry [Bomberman](https://en.wikipedia.org/wiki/Bomberman).
Gra rozgrywa się na prostokątnym ekranie.
Uczestniczy w niej co najmniej jeden gracz.
Każdy z graczy steruje ruchem robota.
Gra rozgrywa się w turach.
Trwa ona przez z góry znaną liczbę tur.
W każdej turze robot może:

- nic nie zrobić
- przesunąć się na sąsiednie pole (o ile nie jest ono zablokowane)
- położyć pod sobą bombę
- zablokować pole pod sobą

Gra toczy się cyklicznie - po uzbieraniu się odpowiedniej liczby graczy
rozpoczyna się nowa rozgrywka na tym samym serwerze.
Stan gry przed rozpoczęciem rozgrywki będziemy nazywać `Lobby`.

### 1.2. Architektura rozwiązania

Na grę składają się trzy komponenty: serwer, klient, serwer obsługujący
interfejs użytkownika.
Należy zaimplementować serwer i klient.
Aplikację implementującą serwer obsługujący graficzny interfejs użytkownika
(ang. *GUI*) dostarczamy.

Serwer komunikuje się z klientami, zarządza stanem gry, odbiera od klientów
informacje o wykonywanych ruchach oraz rozsyła klientom zmiany stanu gry.
Serwer pamięta wszystkie zdarzenia dla bieżącej partii i przesyła je w razie
potrzeby klientom.

Klient komunikuje się z serwerem gry oraz interfejsem użytkownika.

Specyfikacje protokołów komunikacyjnych, rodzaje zdarzeń oraz formaty
komunikatów i poleceń są opisane poniżej.

### 1.3. Parametry wywołania programów

Serwer:
```
    -b, --bomb-timer <u16>
    -c, --players-count <u8>
    -d, --turn-duration <u64, milisekundy>
    -e, --explosion-radius <u16>
    -h, --help                                   Print help information
    -k, --initial-blocks <u16>
    -l, --game-length <u16>
    -n, --server-name <String>
    -p, --port <u16>
    -s, --seed <u32, parametr opcjonalny>
    -x, --size-x <u16>
    -y, --size-y <u16>
```

Klient:
```
    -d, --display-address <(nazwa hosta):(port) lub (IPv4):(port) lub (IPv6):(port)>
    -h, --help                                 Print help information
    -n, --player-name <String>
    -p, --port <u16>
    -s, --server-address <(nazwa hosta):(port) lub (IPv4):(port) lub (IPv6):(port)>
```

Interfejs graficzny:
```
    -c, --client-address <(nazwa hosta):(port) lub (IPv4):(port) lub (IPv6):(port)>
    -h, --help                               Print help information
    -p, --port <u16>
```

Do parsowania parametrów linii komend można użyć funkcji `getopt`
z [biblioteki standardowej C](https://linux.die.net/man/3/getopt) lub modułu `program_options`
z biblioteki [Boost](https://www.boost.org/doc/libs/1_79_0/doc/html/program_options.html).

Wystarczy zaimplementować rozpoznawanie krótkich (`-c`, `-x` itd.) parametrów.

## 2. Protokół komunikacyjny pomiędzy klientem a serwerem

Wymiana danych odbywa się po TCP. Przesyłane są dane binarne, zgodne z poniżej zdefiniowanymi
formatami komunikatów. W komunikatach wszystkie liczby przesyłane są w sieciowej kolejności bajtów,
a wszystkie napisy muszą być zakodowane w UTF-8 i mieć długość krótszą niż 256 znaków.

Klient po podłączeniu się do serwera zaczyna obserwować rozgrywkę, jeżeli ta jest w toku.
W przeciwnym razie może zgłosić chęć wzięcia w niej udziału, wysyłając komunikat `Join`.

Napisy (String) mają następującą reprezentację binarną:
`[1 bajt określający długość napisu][bajty bez ostatniego bajtu zerowego]`

### 2.1. Komunikaty od klienta do serwera

| Komunikat                         | Reprezentacja binarna      |
|--------------------------------|-------------------------|
| `Join(player_name: String)`       | `0[player_name]`           |
| `PlaceBomb`                       | `1`                        |
| `PlaceBlock`                      | `2`                        |
| `Move(direction: Direction)`      | `3[direction]`             |

Typ Direction ma następującą reprezentację binarną:

| Direction | Reprezentacja binarna |
|-----------|-----------------------|
| `Up`      | `0`                   |
| `Right`   | `1`                   |
| `Down`    | `2`                   |
| `Left`    | `3`                   |

### 2.2. Komunikaty od serwera do klienta

    [0] Hello {
        server_name: String,
        players_count: u8,
        size_x: u16,
        size_y: u16,
        game_length: u16,
        explosion_radius: u16,
        bomb_timer: u16,
    },
    [1] AcceptedPlayer {
        id: PlayerId,
        player: Player,
    },
    [2] GameStarted {
        players: Map<PlayerId, Player>,
    },
    [3] Turn {
        turn: u16,
        events: List<Event>,
    },
    [4] GameEnded {
        scores: Map<PlayerId, Score>,
    },


Listy są serializowane w postaci `[4 bajty długości listy][elementy listy]`.
Mapy są serializowane w postaci `[4 bajty długości mapy][klucz][wartość][klucz][wartość]...`.

Pola w strukturze serializowane są bezpośrednio po bajcie oznaczającym typ struktury.

Zatem wiadomość od serwera typu `Turn`

```
ServerMessage::Turn {
        turn: 44,
        events: [
            Event::PlayerMoved {
                id: PlayerId(3),
                position: Position(2, 4),
            },
            Event::PlayerMoved {
                id: PlayerId(4),
                position: Position(3, 5),
            },
            Event::BombPlaced {
                id: BombId(5),
                position: Position(5, 7),
            },
        ],
```

będzie miała następującą reprezentację binarną:

```
[3, 0, 44, 0, 0, 0, 3, 2, 3, 0, 2, 0, 4, 2, 4, 0, 3, 0, 5, 0, 0, 0, 0, 5, 0, 5, 0, 7]

3 - rodzaj wiadomości od serwera (`Turn`)
0, 44 - numer tury
0, 0, 0, 3 - liczba zdarzeń
2 - rodzaj zdarzenia (`PlayerMoved`)
3 - id gracza
0, 2 - współrzędna x
0, 4 - współrzędna y
2 - rodzaj zdarzenia (`PlayerMoved`)
4 - id gracza
0, 3 - współrzędna x
0, 5 - współrzędna y
0 - rodzaj zdarzenia (`BombPlaced`)
0, 0, 0, 5 - id bomby
0, 5 - współrzędna x
0, 7 - współrzędna y
```

Dostarczymy program do weryfikowania poprawności danych.

### 2.3. Definicje użytych powyżej rekordów

    Event:
    [0] BombPlaced { id: BombId, position: Position },
    [1] BombExploded { id: BombId, robots_destroyed: List<PlayerId>, blocks_destroyed: List<Position> },
    [2] PlayerMoved { id: PlayerId, position: Position },
    [3] BlockPlaced { position: Position },

    BombId: u32
    PlayerId: u8
    Position: { x: u16, y: u16 }
    Player: { name: String, address: String }
    Score: u32

Pole `address` w strukturze `Player` może reprezentować zarówno adres IPv4 jak i adres IPv6.

Liczba typu `Score` informuje o tym, ile razy robot danego gracza został zniszczony.


### 2.4. Generator liczb losowych

Do wytwarzania wartości losowych należy użyć poniższego deterministycznego
generatora liczb 32-bitowych. Kolejne wartości zwracane przez ten generator
wyrażone są wzorem:

    r_0 = seed
    r_i = (r_{i-1} * 279410273) mod 4294967291

gdzie wartość `seed` jest 32-bitowa i jest przekazywana do serwera za pomocą
parametru `-s` (domyślnie są to 32 młodsze bity wartości zwracanej przez
wywołanie `time(NULL)`).
W pierwszym wywołaniu generatora powinna zostać zwrócona wartość `r_0 == seed`.

Należy użyć dokładnie takiego generatora, żeby umożliwić automatyczne testowanie
rozwiązania (uwaga na konieczność wykonywania pośrednich obliczeń na typie
64-bitowym).

Poniżej podajemy dwa przykładowe ciągi liczb 32-bitowego typu unsigned, które zostały wygenerowane powyższą metodą:

    r_0 = 1
    r_1 = 279410273
    r_2 = 3468058228
    r_3 = 2207013437
    r_4 = 1650159168

    r_0 = 200000000
    r_1 = 3248565286
    r_2 = 338750614
    r_3 = 4026670339
    r_4 = 1429408516

### 2.5. Stan gry

Serwer jest "zarządcą" stanu gry, do klientów przesyła informacje o zdarzeniach. Klienci je agregują
i przesyłają zagregowany stan do interfejsu użytkownika. Interfejs nie przechowuje w ogóle żadnego stanu.

Serwer powinien przechowywać następujące informacje:

- lista graczy (nazwa, adres IP, numer portu)
- stan generatora liczb losowych (innymi słowy stan generatora NIE restartuje się po każdej rozgrywce)

Oraz tylko w przypadku toczącej się rozgrywki:

- numer tury
- lista wszystkich zdarzeń od początku rozgrywki
- pozycje graczy
- liczba śmierci każdego gracza
- informacje o istniejących bombach (pozycja, czas)
- pozycje istniejących bloków

Lewy dolny róg planszy ma współrzędne `(0, 0)`, odcięte rosną w prawo,
a rzędne w górę.

### 2.6. Podłączanie i odłączanie klientów

Po podłączeniu klienta do serwera serwer wysyła do niego komunikat `Hello`.
Jeśli rozgrywka jeszcze nie została rozpoczęta,
serwer wysyła komunikaty `AcceptedPlayer` z informacją o podłączonych graczach.
Jeśli rozgrywka już została rozpoczęta, serwer wysyła komunikat `GameStarted` z informacją o rozpoczęciu rozgrywki,
a następnie wysyła komunikat `Turn` z informacją o aktualnym stanie gry. Numer tury w takim komunikacie to 0.

Jeśli rozgrywka nie jest jeszcze rozpoczęta, to wysłanie przez klienta komunikatu `Join`
powoduje dodanie go do listy graczy. Serwer następnie rozsyła do wszystkich klientów komunikat `AcceptedPlayer`.

Graczom nadawane jest ID w kolejności podłączenia. Gracze rozpoznawani są po adresie IP i numerze portu.
Dwoje graczy może mieć taką samą nazwę.

Odłączenie gracza w trakcie rozgrywki powoduje tylko tyle, że jego robot przestaje się ruszać.
Odłączenie klienta-gracza przed rozpoczęciem rozgrywki nie powoduje skreślenia go z listy graczy.
Odłączenie klienta-obserwatora nie wpływa na działanie serwera.

### 2.7. Rozpoczęcie partii i zarządzanie podłączonymi klientami

Partia rozpoczyna się, gdy odpowiednio wielu graczy się zgłosi. Musi być dokładnie tylu graczy, ile
jest wyspecyfikowane przy uruchomieniu serwera.

Inicjacja stanu gry przebiega następująco:

```
nr_tury = 0
zdarzenia = []

dla każdego gracza w kolejności id:
    pozycja_x_robota = random() % szerokość_planszy
    pozycja_y_robota = random() % wysokość_planszy
    
    dodaj zdarzenie `PlayerMoved` do listy
    
tyle razy ile wynosi parametr `initial_blocks`:
    pozycja_x_bloku = random() % szerokość_planszy
    pozycja_y_bloku = random() % wysokość_planszy
    
    dodaj zdarzenie `BlockPlaced` do listy
    
wyślij komunikat `Turn`
```

### 2.8. Przebieg partii

Zasady:

- Nie ma ograniczenia na liczbę bloków i bomb.
- Gracze nie mogą wchodzić na pole, które jest zablokowane. Mogą natomiast z niego zejść, jeśli znajdą się na nim,
  wskutek zablokowania go lub "odrodzenia" się na nim.
- Gracze nie mogą wychodzić poza planszę.
- Wielu graczy może zajmować to samo pole.
- Bomby mogą zajmować to samo pole.
- Gracze mogą położyć bombę, nawet jeśli stoją na zablokowanym polu (czyli na jednym polu może być blok, wielu graczy i wiele bomb)

```
zdarzenia = []

dla każdej bomby:
    zmniejsz jej licznik czasu o 1
    jeśli licznik wynosi 0:
        zaznacz, że bomba będzie eksplodować
    
dla każdej eksplodującej bomby:
    oblicz, które bloki znikną w wyniku eksplozji
    oblicz, które roboty zostaną zniszczone w wyniku eksplozji
    dodaj zdarzenie `BombExploded` do listy
    
dla każdego gracza w kolejności id:
    jeśli robot nie został zniszczony:
        jeśli gracz wykonał ruch:
            obsłuż ruch gracza i dodaj odpowiednie zdarzenie do listy
    jeśli robot został zniszczony:
        pozycja_x_robota = random() % szerokość_planszy
        pozycja_y_robota = random() % wysokość_planszy
    
        dodaj zdarzenie `PlayerMoved` do listy
        
zwiększ nr_tury o 1
```

W wyniku eksplozji bomby zostają zniszczone wszystkie roboty w jej zasięgu oraz jedynie najbliższe bloki w jej zasięgu. Eksplozja bomby ma kształt krzyża o długości ramienia równej parametrowi `explosion_radius`. Jeśli robot stoi na bloku, który zostanie zniszczony w wyniku eksplozji, to taki robot również jest niszczony.

Intuicyjnie oznacza to, że można się schować za blokiem, ale położenie bloku pod sobą nie chroni przed eksplozją.

Przykłady:
```
@ - blok
A, B, C... - bomby
1, 2, 3... - gracze
x - eksplozja
```

```
.@2..
..1..
@@A.@
..@..
.....
```

Pola oznaczone jako eksplozja po wybuchu A z promieniem równym 2:
```
.Bx..
..x..
Bxxxx
..x..
.....
```

A zatem zniszczone zostaną 3 bloki i oba roboty.



### 2.9. Wykonywanie ruchu

Serwer przyjmuje informacje o ruchach graczy w następujący sposób:
przez `turn_duration` milisekund oczekuje na informacje od graczy.
Jeśli gracz w tym czasie nie wyśle odpowiedniej wiadomości,
to w danej turze jego robot nic nie robi.
Jeśli w tym czasie gracz wyśle więcej niż jedną wiadomość,
to pod uwagę brana jest tylko ostatnia.

## 3. Protokół komunikacyjny pomiędzy klientem a interfejsem użytkownika

Komunikacja z interfejsem odbywa się po UDP przy użyciu komunikatów zakodowanych jako JSON.

Klient wysyła do interfejsu graficznego następujące komunikaty:

    Lobby {
        server_name: String,
        players_count: u8,
        size_x: u16,
        size_y: u16,
        game_length: u16,
        explosion_radius: u16,
        bomb_timer: u16,
        players: List<(PlayerId, Player)>
    },
    Game {
        server_name: String,
        size_x: u16,
        size_y: u16,
        game_length: u16,
        turn: u16,
        players: Map<PlayerId, Player>,
        player_positions: Map<PlayerId, Position>,
        blocks: List<Position>,
        bombs: List<Bomb>,
        explosions: List<Position>,
        scores: Map<PlayerId, Score>,
    },

Klient powinien wysłać taki komunikat po każdej zmianie stanu (tzn. otrzymaniu wiadomości `Turn` jeśli rozgrywka jest w
toku lub `AcceptedPlayer` jeśli rozgrywka się nie toczy).

Interfejs wysyła do klienta następujące komunikaty:

    PlaceBomb,
    PlaceBlock,
    Move { direction: Direction },

Są one wysyłane za każdym razem, gdy gracz naciśnie odpowiedni przycisk.

Można skorzystać z biblioteki [`json`](https://github.com/nlohmann/json).

## 4. Ustalenia dodatkowe

Programy powinny umożliwiać komunikację zarówno przy użyciu IPv4, jak i IPv6.

Można korzystać z biblioteki `Boost`, w szczególności z modułu `asio`.

Rozwiązanie ma kompilować się i działać na serwerze students.

Rozwiązania należy kompilować z flagami `-Wall -Wextra -O2`. Przy kompilowaniu z tymi flagami kompilator nie powinien wypisywać żadnych ostrzeżeń.

Rozwiązania napisane w języku C++ powinny być kompilowane z flagą `-std=c++17`, a w języku C z flagą `-std=c11`.

Rozwiązanie powinno być odpowiednio sformatowane (można użyć np. `clang-format`).

## 5. Oddawanie rozwiązania

Jako rozwiązanie można oddać tylko klienta (część A) lub tylko serwer (część B),
albo obie części.

Jako rozwiązanie należy dostarczyć pliki źródłowe oraz plik `makefile`, które
należy umieścić jako skompresowane archiwum w Moodle. Archiwum powinno zawierać
tylko pliki niezbędne do zbudowania programów. Nie wolno w nim umieszczać plików
binarnych ani pośrednich powstających podczas kompilowania programów.

Po rozpakowaniu dostarczonego archiwum, w wyniku wykonania w jego głównym
katalogu polecenia `make`, dla części A zadania ma powstać w tym katalogu plik
wykonywalny `robots-client` a dla części B zadania – plik
wykonywalny `robots-server`.
Ponadto `makefile` powinien obsługiwać cel `clean`, który po wywołaniu kasuje
wszystkie pliki powstałe podczas kompilowania.

## 6. Ocena

Za rozwiązanie części A zadania można dostać maksymalnie 10 punktów.
Za rozwiązanie części B zadania można dostać maksymalnie 15 punktów.
Każda część zadania będzie testowana i oceniana osobno.
Ocena każdej z części zadania będzie się składała z trzech składników:

1. ocena wzrokowa i manualna działania programu (20%)
2. testy automatyczne (50%)
3. jakość kodu źródłowego (30%)

### 6.1 Ocena wzrokowa i manualna działania programu

- jak program reaguje, gdy zostanie wywołany z bezsensownymi argumentami? (Najlepiej jeśli wypisuje jakiś komunikat o błędzie; ważne żeby nie było segfaulta)
- czy w grę rzeczywiście da się grać

### 6.2 Testy automatyczne

Testy będą obejmowały m.in.:
- bardzo proste scenariusze testowe (czy podłączenie gracza do serwera powoduje wysłanie odpowiedniego komunikatu do klientów, czy otrzymanie wiadomości od interfejsu powoduje wysłanie wiadomości do serwera, czy otrzymanie wiadomości od serwera powoduje wysłanie wiadomości do klienta itd., czy programy prawidłowo resolvują nazwy domenowe (np. localhost), czy można się połączyć zarówno po IPv4 jak i IPv6)
- proste scenariusze testowe (symulacja krótkiej rozgrywki z jednym graczem, czy generowanie planszy odbywa się zgodnie z powyższym opisem; czy wybuch bomby jest prawidłowo obliczany)
- złożone scenariusze testowe (symulacja długiej rozgrywki z wieloma graczami)

### 6.3 Jakość kodu źródłowego

- absolutne podstawy: kod powinien być jednolicie sformatowany (najlepiej użyć do tego clang-format lub formatera wbudowanego w cliona), nie wyciekać pamięci, po skompilowaniu z parametrami `-Wall -Wextra` nie powinno być żadnych ostrzeżeń. Dodatkowo można sprawdzić sobie program przy użyciu lintera `clang-tidy`
- kod powinien być sensownie podzielony na funkcje, nazwy funkcji i zmiennych powinny być znaczące (a nie np. a, b, x, y, temp) i w jednym języku
- komentarze powinny być w jednym języku
- “magiczne stałe” powinny być ponazywane
- [“Parse, don’t validate”](https://lexi-lambda.github.io/blog/2019/11/05/parse-don-t-validate/)


Prowadzący powinni zrobić code review wszystkim chętnym studentom i studentkom, którzy i które zgłoszą się odpowiednio wcześniej.





