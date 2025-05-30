#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MIN_PLAYERS 2
#define MAX_PLAYERS 20

#define MAX_LIFE_POINTS 2

#define MAX_CARDS_PLAYERS 2

#define PLAYERS_TURN 1

#define TYPES 4
#define VALUES 10
#define TOTAL_NUMBER_CARDS 40

#define SIZE_CARD_TYPE 10
#define SIZE_CARD_VALUE 3
#define SIZE_NAME 50

typedef struct
{
    char cardType[SIZE_CARD_TYPE];
    char cardValue[SIZE_CARD_VALUE];
    int isCardVisible;
} CARDS;

typedef struct
{
    char name[SIZE_NAME];
    CARDS visibleCard[1];
    CARDS hiddenCard[1];
    int lifePoints;
    int round;
} PLAYER;

int gameLifePoints = 0;

// Funzioni main per le fasi di gioco
void initializeNumberOfPlayers(int *numberOfPlayers);
PLAYER* initializePlayers(int *numberOfPlayers, int *lifePoints);
CARDS* buildBaseDeck();
void deckShuffle(CARDS *deck);
void dealCardsToPlayers(PLAYER *players, CARDS *deck, int *numberOfPlayers);
void selectFirstPlayerToPlay(PLAYER *players, int *numberOfPlayers);
void displayGameField(PLAYER *players, int *numberOfPlayers);
void gamePhase(PLAYER *players, CARDS *deck, int *numberOfPlayers);
void removeDeadPlayers(PLAYER *players, int *numberOfPlayers);

// Funzioni per effetto carte
int displayCardEffect(CARDS *targetCard);
void applyCardEffect(CARDS *targetCard, PLAYER *players, int *indexPlayerTurn, int *numberOfPlayers, int printCardInfo);

// Funzioni generiche (utils)
int getIntInput(int min, int max);
int getWrappedIndex(int baseIndex, int offset, int numberOfPlayers);
int resetIndex(int *indexPlayer, int *numberOfPlayers);

int main(void)
{
    int numberOfPlayers = 0;
    int lifePoints = 2;

    puts("");
    puts("Ciao, benvenuto al gioco di carte senza nome.\n\nIl numero di giocatori dev'essere compreso tra 2-20");
    puts("Ogni giocatore ha 2 life points, vince chi rimane in vita per ultimo");
    printf("Vuoi cambiare i life points dei giocatori? [Si=1/No=0]: "); 

    int userInput = getIntInput(0, 1);
    if (userInput == 1) {
        printf("Inserisci il valore dei life points che ogni giocatore avrà: ");
        lifePoints = getIntInput(2, 10000);
    }

    initializeNumberOfPlayers(&numberOfPlayers);
    PLAYER *players = initializePlayers(&numberOfPlayers, &lifePoints);
    CARDS *deck = buildBaseDeck();

    // Main game phase
    do
    {
        gamePhase(players, deck, &numberOfPlayers);
    }
    while (numberOfPlayers > 1);
    
    free(players);
    free(deck);

    return 0;
}

void gamePhase(PLAYER *players, CARDS *deck, int *numberOfPlayers)
{
    int indexPlayer = 0;
    int phase = 1;

    deckShuffle(deck);
    dealCardsToPlayers(players, deck, numberOfPlayers);
    selectFirstPlayerToPlay(players, numberOfPlayers);

    while (phase > 0)
    {
        removeDeadPlayers(players, numberOfPlayers);

        // Se dopo la rimozione abbiamo un solo player la partitai si conclude
        if (*numberOfPlayers == 1)
        {
            printf("Partita terminata, vincitore: %s", players[0].name);
            return;
        }

        // Se il giocatore corrente è morto faccio il reset dell'index (se necessario)
        if (players[indexPlayer].lifePoints == 0)
        {
            resetIndex(&indexPlayer, numberOfPlayers);
            continue;
        }

        displayGameField(players, numberOfPlayers);
        printf("Turno del giocatore: %s \n\n", players[indexPlayer].name);
        players[indexPlayer].round++;

        puts("Risoluzione effetto carta scoperta:");
        applyCardEffect(players[indexPlayer].visibleCard, players, &indexPlayer, numberOfPlayers, 1);

        // Se il giocatore corrente è morto faccio il reset dell'index (se necessario)
        if (players[indexPlayer].lifePoints == 0)
        {
            resetIndex(&indexPlayer, numberOfPlayers);
            continue;
        }

        if (players[indexPlayer].hiddenCard->isCardVisible == 1)
        {
            puts("La carta coperta ed il suo effetto sono già stati applicati");
            printf("Inserisci 1 per cedere il turno al giocatore successivo: ");
            int userInput = getIntInput(1, 1);
            puts("");
            printf("%s turno completato\n", players[indexPlayer].name);
            
            // Se tutti i giocatori hanno giocato il proprio turno si conclude la fase di gioco
            int completedRounds = 0;
            for (int i = 0; i < *numberOfPlayers; i++)
            {
                if (players[i].round == 1)
                    completedRounds++;
            }

            if (completedRounds == *numberOfPlayers)
            {
                phase = 0;
                puts("Fase finita, inizio di una nuova fase...\n");
            }

            indexPlayer = (indexPlayer + 1) % (*numberOfPlayers);
            continue;
        }

        puts("È possibile vedere la carta coperta ed opzionalmente scoprirla per applicarne l'effetto.\n");
        printf("Vuoi vedere la carta coperta? [Si=1/No=0]: ");
        int userInput = getIntInput(0, 1);
        puts("");

        if (userInput == 1)
        {
            printf("Tipo carta e valore: %s %s", players[indexPlayer].hiddenCard->cardType, players[indexPlayer].hiddenCard->cardValue);
            int hasCardEffect = displayCardEffect(players[indexPlayer].hiddenCard);
            if (hasCardEffect)
            {
                printf("Si vuole applicarne l'effetto? [Si=1/No=0]: ");
                int showHiddenCard = getIntInput(0, 1);
                
                // Se il giocatore ha un solo punto vita chiediamo se è sicuro di voler applicare l'effetto della carta
                if (players[indexPlayer].lifePoints == 1 && showHiddenCard == 1)
                {
                    switch (*players[indexPlayer].hiddenCard->cardValue)
                    {
                        case '1':
                        case 'J':
                        case 'Q':
                            printf("L'azione porta ad azzerare le vite del giocatore corrente. Sei sicuro di voler procedere? [Si=1/No=0]: ");
                            showHiddenCard = getIntInput(0, 1);
                            break;
                        default:
                            break;
                    }
                }
                
                if (showHiddenCard == 1)
                {
                    players[indexPlayer].hiddenCard->isCardVisible = 1;
                    applyCardEffect(players[indexPlayer].hiddenCard, players, &indexPlayer, numberOfPlayers, 0);
                }
            }
            else    // Se non ha alcun effetto la rendiamo visibile di default 
                players[indexPlayer].hiddenCard->isCardVisible = 1;
        }

        printf("Inserisci 1 per cedere il turno al giocatore successivo: ");
        userInput = getIntInput(1, 1);
        puts("\n");
        printf("%s turno completato\n", players[indexPlayer].name);
        
        // Se tutti i giocatori hanno giocato il proprio turno si conclude la fase di gioco
        int completedRounds = 0;
        for (int i = 0; i < *numberOfPlayers; i++)
        {
            if (players[i].round == 1)
                completedRounds++;
        }

        if (completedRounds == *numberOfPlayers)
        {
            phase = 0;
            puts("Fase finita, inizio di una nuova fase...\n");
        }

        // Se il giocatore corrente è morto faccio il reset dell'index (se necessario)
        if (players[indexPlayer].lifePoints == 0)
        {
            resetIndex(&indexPlayer, numberOfPlayers);
            continue;
        }
        else
            indexPlayer = (indexPlayer + 1) % (*numberOfPlayers);
    }
}

void initializeNumberOfPlayers(int *numberOfPlayers)
{
    printf("Inserire il numero di giocatori [2-20]: ");
    scanf("%d", &*numberOfPlayers);
    puts("");

    if (*numberOfPlayers < MIN_PLAYERS || *numberOfPlayers > MAX_PLAYERS)
    {
        puts("Numero di giocatori non valido, inserire un numero di giocatori compreso tra 2 e 20");
        puts("");
        initializeNumberOfPlayers(numberOfPlayers);
    }
}

PLAYER* initializePlayers(int *numberOfPlayers, int *lifePoints)
{
    PLAYER *players = malloc(*numberOfPlayers * sizeof(PLAYER));
    if (players == NULL) {
        puts("Errore: Allocazione della memoria fallita!");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < *numberOfPlayers; i++) {
        players[i].lifePoints = *lifePoints;
        players[i].round = 0;
        sprintf(players[i].name, "Player %d", i + 1);
    }

    return players;
}

CARDS* buildBaseDeck() 
{
    CARDS *deck = malloc(TOTAL_NUMBER_CARDS * sizeof(CARDS));
    if (deck == NULL) {
        puts("Errore: Allocazione della memoria fallita!");
        exit(EXIT_FAILURE);
    }

    char *cardTypeNames[TYPES] = { "Fiori", "Picche", "Quadri", "Cuori"};
    char *cardValuesNames[VALUES] = { "1", "2", "3", "4", "5", "6", "7", "J", "K", "Q"};

    int deckIndex = 0;
    for (int type = 0; type < TYPES; type++) {
        for (int value = 0; value < VALUES; value++) {
            strcpy(deck[deckIndex].cardType, cardTypeNames[type]);
            strcpy(deck[deckIndex].cardValue, cardValuesNames[value]);
            deckIndex++;
        }
    }
    return deck;
}

void deckShuffle(CARDS *deck)
{
    srand(time(NULL));
    int numberOfCardsToProcess = (TOTAL_NUMBER_CARDS - 1);

    // Primo mescolamento del deck
    while (numberOfCardsToProcess > 0) {
        int randomCard = rand() % (numberOfCardsToProcess + 1);
        CARDS temp = deck[numberOfCardsToProcess];
        deck[numberOfCardsToProcess] = deck[randomCard];
        deck[randomCard] = temp;
        --numberOfCardsToProcess;
    }

    // Secondo mescolamento del deck
    for (int i = 1; i < TOTAL_NUMBER_CARDS; i++) 
    {
        // Controllo se le carte adiacenti siano dello stesso tipo o dello stesso valore
        if (strcmp(deck[i].cardType, deck[i - 1].cardType) == 0 || strcmp(deck[i].cardValue, deck[i - 1].cardValue) == 0) 
        { 
            for (int j = i + 1; j < TOTAL_NUMBER_CARDS; j++) 
            {
                // Cerco un'altra carta di un tipo e valore diverso
                if (strcmp(deck[j].cardType, deck[i - 1].cardType) != 0 && strcmp(deck[j].cardValue, deck[i - 1].cardValue) != 0) 
                {
                    CARDS temp = deck[i];
                    deck[i] = deck[j];
                    deck[j] = temp;
                    break;
                }
            }
        }
    }
}

void dealCardsToPlayers(PLAYER *players, CARDS *deck, int *numberOfPlayers)
{
    int deckIndex = 0;
    for (int i = 0; i < *numberOfPlayers; i++) 
    {
        players[i].visibleCard[0] = deck[deckIndex];
        players[i].hiddenCard[0] = deck[++deckIndex];
        players[i].hiddenCard->isCardVisible = 0;
        deckIndex++;
    }
}

void selectFirstPlayerToPlay(PLAYER *players, int *numberOfPlayers)
{
    srand(time(NULL));
    int randomPlayer = rand() % (*numberOfPlayers + 1);

    PLAYER *originalPlayers = malloc(*numberOfPlayers * sizeof(PLAYER));
    if (originalPlayers == NULL) {
        puts("Errore: Allocazione della memoria fallita!");
        exit(EXIT_FAILURE);
    }

    memcpy(originalPlayers, players, *numberOfPlayers * sizeof(PLAYER));

    // Prendo un giocatore casuale in base al numero di giocatori
    // Ordino l'array in ascending
    for (int i = 0; i < *numberOfPlayers; i++) 
    {
        int nextPlayer = (randomPlayer + i) % *numberOfPlayers;
        players[i] = originalPlayers[nextPlayer];
        players[i].round = 0;
        printf("%d: %s\n", i + 1, players[i].name);
    }

    free(originalPlayers);
}

int displayCardEffect(CARDS *targetCard)
{
    int hasCardEffect = 1;
    puts("");

    switch (*targetCard->cardValue)
    {
        case '1':
            puts("Effetto: il giocatore in possesso della carta perde 1 life point, che verrà lasciato sul campo di gioco");
            break;

        case '7':
            puts("Effetto: il giocatore in possesso della carta forza il giocatore successivo a scoprire la sua carta coperta e ad applicarne l'effetto");
            break;

        case 'J':
            puts("Effetto: il giocatore in possesso della carta deve dare 1 life point al giocatore precedente");
            break;

        case 'K':
            puts("Effetto: il giocatore in possesso della carta, riceve tutti i punti vita lasciati sul campo di gioco");
            break;

        case 'Q':
            puts("Effetto: il giocatore in possesso della carta deve dare 1 life point al secondo giocatore successivo");
            break;

        default:
            puts("La carta non ha nessun effetto speciale");
            hasCardEffect = 0;
            break;
    }
    puts("");

    return hasCardEffect;
}

void applyCardEffect(CARDS *targetCard, PLAYER *players, int *indexPlayerTurn, int *numberOfPlayers, int printCardInfo)
{
    puts("");

    if (printCardInfo)
        printf("Tipo carta e valore: %s %s\n", targetCard->cardType, targetCard->cardValue);

    int currentPlayer = *indexPlayerTurn;
    int totalPlayers = *numberOfPlayers;

    switch (*targetCard->cardValue)
    {
        case '1':
            printf("%s perde 1 life point\n", players[currentPlayer].name);
            players[currentPlayer].lifePoints--;
            gameLifePoints++;
            break;

        case '7':
            int nextPlayer = getWrappedIndex(currentPlayer, 1, totalPlayers);
            if (players[nextPlayer].hiddenCard->isCardVisible == 1)
            {
                printf("La carta coperta del giocatore %s risulta essere già scoperta ed attivata", players[nextPlayer].name);
            }
            else 
            {
                printf("La carta coperta del giocatore successivo verrà scoperta ed il suo effetto verrà applicato");
                players[nextPlayer].hiddenCard->isCardVisible = 1;
                applyCardEffect(players[nextPlayer].hiddenCard, players, &nextPlayer, numberOfPlayers, 1);
            }
            break;

        case 'J':
            int previousPlayer = getWrappedIndex(currentPlayer, -1, totalPlayers);
            printf("%s deve dare 1 life point al giocatore %s\n", players[currentPlayer].name, players[previousPlayer].name);
            players[currentPlayer].lifePoints--;
            players[previousPlayer].lifePoints++;
            break;

        case 'K':
            printf("%s prende %d punti dal campo di gioco\n", players[currentPlayer].name, gameLifePoints);
            players[currentPlayer].lifePoints += gameLifePoints;
            gameLifePoints = 0;
            break;

        case 'Q':
            int targetPlayer = getWrappedIndex(currentPlayer, 2, totalPlayers);
            printf("%s deve dare 1 life point al giocatore %s\n", players[currentPlayer].name, players[targetPlayer].name);
            players[currentPlayer].lifePoints--;
            players[targetPlayer].lifePoints++;
            break;

        default:
            puts("La carta non ha nessun effetto speciale");
            break;
    }

    puts("");
}

void displayGameField(PLAYER *players, int *numberOfPlayers) 
{
    puts("");
    printf("\n===================== CAMPO DI GIOCO ==================\n\n");
    printf("        ************************************           \n");
    printf("        *          Table points %d          *           \n", gameLifePoints);
    printf("        ************************************          \n\n");
    for (int i = 0; i < *numberOfPlayers; i++) 
    {
        if (players[i].hiddenCard->isCardVisible == 1)
        {
            printf("| %s | Life points %d | Carte: [%s %s] [%s %s]\n", players[i].name, players[i].lifePoints, players[i].visibleCard->cardType, players[i].visibleCard->cardValue, players[i].hiddenCard->cardType, players[i].hiddenCard->cardValue);
        }
        else
        {
            printf("| %s | Life points %d | Carte: [%s %s] [%s]\n", players[i].name, players[i].lifePoints, players[i].visibleCard->cardType, players[i].visibleCard->cardValue, "coperta");
        }
    }
    printf("\n=======================================================\n\n");
}

void removeDeadPlayers(PLAYER *players, int *numberOfPlayers) 
{
    int newCount = 0;
    for (int i = 0; i < *numberOfPlayers; i++) 
    {
        if (players[i].lifePoints > 0) 
        {
            // Muovo il giocatore alla prossima posizione
            players[newCount++] = players[i];
        }
    }
    *numberOfPlayers = newCount;
}

int getIntInput(int min, int max) 
{
    int input = -1;
    while (scanf("%d", &input) != 1 || input < min || input > max) 
    {
        while (getchar() != '\n');
        printf("Input non valido. Riprovare (%d-%d): ", min, max);
    }
    return input;
}

int getWrappedIndex(int baseIndex, int offset, int numberOfPlayers) 
{
    // Reset dell'indice in caso di risultato negativo
    int result = (baseIndex + offset) % numberOfPlayers;
    if (result < 0)
        result += numberOfPlayers;
    return result;
}

/* 
Se l'indice del giocatore senza life points > numero di giocatori rimasti -> diminuisco l'indice di 1
Se l'indice del giocatore senza life points == (numero di giocatori rimasti - 1) -> reset dell'indice
Se l'indice del giocatore senza life points < numero di giocatori rimasti -> non faccio nulla, perché il giocatore successivo 'scende' di posizione
*/
int resetIndex(int *indexPlayer, int *numberOfPlayers)
{
    if (*indexPlayer > *numberOfPlayers)
        *indexPlayer--;
    else if (*indexPlayer == (*numberOfPlayers - 1))
        *indexPlayer = (*indexPlayer + 1) % (*numberOfPlayers);
}