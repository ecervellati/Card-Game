#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/** @brief Numero minimo di giocatori */
#define MIN_PLAYERS 2

/** @brief Numero massimo di giocatori */
#define MAX_PLAYERS 20

/** @brief Valore minimo life points */
#define MIN_LIFE_POINTS 2

/** @brief Valore massimo life points */
#define MAX_LIFE_POINTS 100

/** @brief Numero rappresentante i vari tipi di carte (Fiori, Cuori ecc.) */
#define CARD_TYPES 4

/** @brief Numero rappresentante i valori che le carte possono assumere */
#define CARD_VALUES 10

/** @brief Numero carte presenti nel mazzo */
#define TOTAL_NUMBER_CARDS 40

/** @brief Valore di input da parte del giocatore  */
#define YES_INPUT 1

/** @brief Valore di input da parte del giocatore  */
#define NO_INPUT 0

/** @brief Size utile per il tipo di carta */
#define SIZE_CARD_TYPE 10

/** @brief Size utile per il valore della carta */
#define SIZE_CARD_VALUE 3

/** @brief Size utile per il nome il giocatore */
#define SIZE_NAME 50

/** @brief Struttura per rappresentare una carta */
typedef struct
{
    char cardType[SIZE_CARD_TYPE]; /**< Tipo carta: Fiori, Picche, ecc. */
    char cardValue[SIZE_CARD_VALUE]; /**< Valore carta: 1-7, J, Q, K */
    int isCardVisible; /**< Indica se la carta è scoperta */
} CARDS;

/** @brief Struttura per rappresentare un giocatore */
typedef struct
{
    char name[SIZE_NAME]; /**< Nome del giocatore */
    CARDS visibleCard[1]; /**< Carta scoperta */
    CARDS hiddenCard[1]; /**< Carta coperta */
    int lifePoints; /**< Punti vita */
    int round; /**< Indica se il giocatore ha completato il turno in una fase */
} PLAYER;

/** @brief Life points presenti nel campo di gioco */
int gameLifePoints = 0;

/**
 * @brief Richiede all'utente di inserire il numero di giocatori.
 * @param numberOfPlayers Puntatore alla variabile dove salvare il numero inserito.
*/
void initializeNumberOfPlayers(int *numberOfPlayers);

/**
 * @brief Inizializza i giocatori con nome e life points.
 * @param numberOfPlayers Numero di giocatori.
 * @param lifePoints Life points iniziali per ciascun giocatore.
 * @return Puntatore all'array dei giocatori.
*/
PLAYER* initializePlayers(int *numberOfPlayers, int *lifePoints);

/**
 * @brief Costruisce il mazzo di carte base non mescolato.
 * 
 * Composizione mazzo di carte:
 *  - 10 carte di Fiori (da 1 a 7, con J,Q K).
 *  - 10 carte di Picche (da 1 a 7, con J,Q K).
 *  - 10 carte di Quadri (da 1 a 7, con J,Q K).
 *  - 10 carte di Cuori (da 1 a 7, con J,Q K).
 * 
 * @return Puntatore al mazzo di carte.
*/
CARDS* buildBaseDeck();

/**
 * @brief Mescola il mazzo di carte. 
 * Dopo un primo mescolamento ne fa un secondo, dove controlla se le carte adiacenti siano dello stesso tipo o dello stesso valore.
 * Se lo sono cerca un'altra carta di valore e tipo diverso.
 * @param deck Puntatore al mazzo di carte.
*/
void deckShuffle(CARDS *deck);

/**
 * @brief Distribuisce le carte ai giocatori.
 * @param players Puntatore ai giocatori.
 * @param deck Mazzo di carte.
 * @param numberOfPlayers Numero di giocatori.
*/
void dealCardsToPlayers(PLAYER *players, CARDS *deck, int *numberOfPlayers);

/**
 * @brief Seleziona casualmente il primo giocatore che deve iniziare. Riordina i giocatori in ascending.
 * @param players Puntatore ai giocatori.
 * @param numberOfPlayers Numero di giocatori.
*/
void selectFirstPlayerToPlay(PLAYER *players, int *numberOfPlayers);

/**
 * @brief Visualizza il campo di gioco corrente.
 * @param players Puntatore ai giocatori.
 * @param numberOfPlayers Numero di giocatori.
*/
void displayGameField(PLAYER *players, int *numberOfPlayers);

/**
 * @brief Visualizza la mano del giocatore corrente.
 * @param players Puntatore ai giocatori.
 * @param indexPlayerTurn Indice del giocatore corrente.
*/
void displayPlayerField(PLAYER *players, int *indexPlayerTurn);

/**
 * @brief Gestisce l'intera fase di gioco fino alla fine della fase o fine partita.
 * 
 * Descrizione fase preliminare all'inizio di ogni fase:
 *  - Mescolamento del mazzo.
 *  - Distribuzione carte ai giocatori.
 *  - Selezione casuale del primo giocatore ad iniziare il turno: 
 *      - Ordinamento dei giocatori a partire dal primo giocatore selezionato.
 *      - Reset dei turni di tutti i giocatori.
 * 
 * Descrizione del flusso principale di ogni fase:
 *  - Rimozione di eventuali giocatori rimasti senza life points.
 *  - Controllo se nella fase è rimasto un solo giocatore, in caso positivo la partita termina.
 *  - Gestione del turno del giocatore corrente.
 *  - Applicazione effetto carta scoperta.
 *  - Gestione della carta coperta da parte del giocatore.
 *  - Alla fine di ogni turno, verifica se tutti i giocatori hanno completato almeno un turno.
 * 
 * @param players Puntatore ai giocatori.
 * @param deck Mazzo di carte.
 * @param numberOfPlayers Numero di giocatori.
*/
void gamePhase(PLAYER *players, CARDS *deck, int *numberOfPlayers);

/**
 * @brief Rimuove i giocatori che hanno 0 punti vita. Riordina l'array players in base al numero di giocatori rimasti.
 * @param players Puntatore ai giocatori.
 * @param numberOfPlayers Puntatore al numero aggiornato di giocatori.
*/
void removeDeadPlayers(PLAYER *players, int *numberOfPlayers);

/**
 * @brief Stampa l'effetto della carta se presente.
 * @param targetCard Carta da analizzare.
 * @return 1 se la carta ha un effetto speciale, altrimenti 0.
*/
int displayCardEffect(CARDS *targetCard);

/**
 * @brief Applica l'effetto della carta.
 * @param targetCard Carta di cui applicare l'effetto.
 * @param players Puntatore ai giocatori.
 * @param indexPlayerTurn Indice del giocatore corrente.
 * @param numberOfPlayers Numero di giocatori.
 * @param printCardInfo Se 1, stampa le informazioni della carta (tipo e valore).
*/
void applyCardEffect(CARDS *targetCard, PLAYER *players, int *indexPlayerTurn, int *numberOfPlayers, int printCardInfo);

/**
 * @brief Richiede in input un intero compreso tra un minimo e un massimo.
 * @param min Valore minimo.
 * @param max Valore massimo.
 * @return Il valore intero inserito dall'utente.
*/
int getIntInput(int min, int max);

/**
 * @brief Calcola l'indice corretto considerando un offset. Utile in fase di applicazione effetto carta.
 * @param baseIndex Indice di partenza.
 * @param offset Numero di posizioni in cui spostarsi.
 * @param numberOfPlayers Numero di giocatori.
 * @return Indice aggiornato.
*/
int getWrappedIndex(int baseIndex, int offset, int numberOfPlayers);

/**
 * @brief Aggiusta l'indice del giocatore successivo dopo l'eliminazione di un giocatore morto.
 * @param indexPlayer Puntatore all'indice da aggiornare.
 * @param numberOfPlayers Numero corrente di giocatori.
 * @return L'indice aggiornato.
*/
int resetIndex(int *indexPlayer, int *numberOfPlayers);

int main(void)
{
    int numberOfPlayers = 0;
    int lifePoints = 2;

    puts("");
    puts("Ciao, benvenuto al gioco di carte senza nome.\n\nIl numero di giocatori dev'essere compreso tra 2-20");
    puts("Ogni giocatore ha 2 life points, vince chi rimane in vita per ultimo");
    printf("Vuoi cambiare i life points dei giocatori? [Si=1/No=0]: "); 

    int userInput = getIntInput(NO_INPUT, YES_INPUT);
    if (userInput == 1) {
        printf("Inserisci il valore dei life points che ogni giocatore avrà: ");
        lifePoints = getIntInput(MIN_LIFE_POINTS, MAX_LIFE_POINTS);
    }

    initializeNumberOfPlayers(&numberOfPlayers);
    PLAYER *players = initializePlayers(&numberOfPlayers, &lifePoints);
    CARDS *deck = buildBaseDeck();

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

        // Se dopo la rimozione abbiamo un solo player la partita si conclude
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
        displayPlayerField(players, &indexPlayer);

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
            int userInput = getIntInput(YES_INPUT, YES_INPUT);
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
        int userInput = getIntInput(NO_INPUT, YES_INPUT);
        puts("");

        if (userInput == YES_INPUT)
        {
            printf("Tipo carta e valore: %s %s", players[indexPlayer].hiddenCard->cardType, players[indexPlayer].hiddenCard->cardValue);
            int hasCardEffect = displayCardEffect(players[indexPlayer].hiddenCard);
            if (hasCardEffect)
            {
                printf("Si vuole applicarne l'effetto? [Si=1/No=0]: ");
                int showHiddenCard = getIntInput(NO_INPUT, YES_INPUT);
                
                // Se il giocatore ha un solo punto vita chiediamo se è sicuro di voler applicare l'effetto della carta
                if (players[indexPlayer].lifePoints == 1 && showHiddenCard == 1)
                {
                    switch (*players[indexPlayer].hiddenCard->cardValue)
                    {
                        case '1':
                        case 'J':
                        case 'Q':
                            printf("L'azione porta ad azzerare le vite del giocatore corrente. Sei sicuro di voler procedere? [Si=1/No=0]: ");
                            showHiddenCard = getIntInput(NO_INPUT, YES_INPUT);
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
        userInput = getIntInput(YES_INPUT, YES_INPUT);
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

    char *cardTypeNames[CARD_TYPES] = { "Fiori", "Picche", "Quadri", "Cuori"};
    char *cardValuesNames[CARD_VALUES] = { "1", "2", "3", "4", "5", "6", "7", "J", "K", "Q"};

    int deckIndex = 0;
    for (int type = 0; type < CARD_TYPES; type++) {
        for (int value = 0; value < CARD_VALUES; value++) {
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
            printf("%s dà 1 life point al giocatore %s\n", players[currentPlayer].name, players[previousPlayer].name);
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
            printf("%s dà 1 life point al giocatore %s\n", players[currentPlayer].name, players[targetPlayer].name);
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

void displayPlayerField(PLAYER *players, int *indexPlayerTurn) 
{
    printf("\n=================== MANO DEL GIOCATORE ================\n\n");
        if (players[*indexPlayerTurn].hiddenCard->isCardVisible == 1)
        {
            printf("| %s | Life points %d | Carte: [%s %s] [%s %s]\n", players[*indexPlayerTurn].name, players[*indexPlayerTurn].lifePoints, players[*indexPlayerTurn].visibleCard->cardType, players[*indexPlayerTurn].visibleCard->cardValue, players[*indexPlayerTurn].hiddenCard->cardType, players[*indexPlayerTurn].hiddenCard->cardValue);
        }
        else
        {
            printf("| %s | Life points %d | Carte: [%s %s] [%s]\n", players[*indexPlayerTurn].name, players[*indexPlayerTurn].lifePoints, players[*indexPlayerTurn].visibleCard->cardType, players[*indexPlayerTurn].visibleCard->cardValue, "coperta");
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

int resetIndex(int *indexPlayer, int *numberOfPlayers)
{
    if (*indexPlayer > *numberOfPlayers)
        *indexPlayer--;
    else if (*indexPlayer == (*numberOfPlayers - 1))
        *indexPlayer = (*indexPlayer + 1) % (*numberOfPlayers);
}