#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MIN_PLAYERS 2
#define MAX_PLAYERS 5

#define MAX_LIFE_POINTS 2

#define MAX_CARDS_PLAYERS 2

#define PLAYERS_TURN 1

#define TYPES 4
#define VALUES 10
#define TOTAL_NUMBER_CARDS 40


typedef struct
{
    char cardType[10];
    char cardValue[3];
} CARDS;

// static means it must be not modified
static CARDS basedeck[TOTAL_NUMBER_CARDS] = { 0 };

typedef struct
{
    char name[50];
    CARDS deck[MAX_CARDS_PLAYERS];
    int lifePoints;
    int round;
} PLAYER;

// Game development main methods
/*
1. Mescolare il mazzo
2. Distribuire ad ogni giocatore 2 carte: una coperta e una scoperta.
3. Scegliere casualmente il primo giocatore della fase.
*/
void welcomeToTheGame(int *numberOfPlayers);
PLAYER* initializePlayers(int *numberOfPlayers);
void buildBaseDeck(CARDS *deck);
void deckShuffle(CARDS *deck, int numberOfCards);
void dealCardsToPlayers(PLAYER *players, CARDS *deck, int *numberOfPlayers);
void selectFirstPlayerToPlay(PLAYER *players, int *numberOfPlayers);

// Game development phase
/*
La fase si svolge dando a ciascun giocatore un turno. A partire dal primo giocatore, in ordine crescente
ad anello, il giocatore nel suo turno:
1. Deve risolvere l’effetto della propria carta scoperta (vedi effetto carte).
2. Pu`o visionare la propria carta coperta e opzionalmente scoprirla per applicarne l’effetto. Nel
caso fosse gi`a scoperta per effetto di altre carte, non sar`a possibile applicarne nuovamente il suo
effetto.
1
3. Il giocatore termina la propria giocata cedendo il turno al giocatore successivo. Alla fine del
turno di tutti i giocatori, la fase si conclude e chi non ha pi`u vite viene eliminato.
Cosa significa in ordine crescente ad anello?
• Supponiamo di avere 3 giocatori (G1, G2, G3)
– Se il primo giocatore `e G1, la fase seguir`a i turni di G1 → G2 → G3
– Se il primo giocatore `e G2, la fase seguir`a i turni di G2 → G3 → G1
– Se il primo giocatore `e G3, la fase seguir`a i turni di G3 → G1 → G2

void assignsTurnsToPlayers();
void applyUncoveredCardEffect();
void showHiddenCard(); // Give player the choice to apply or not the effect
void checkThePlayersWhoLost(); // Il gioco termina quando rimane solo un giocatore al termine di una fase e gli altri sono stati tutti eliminati.
*/

// Cards effect
/*
• Carte dal 2 al 6: non succede nulla.
• Carta 7: il giocatore in possesso della carta forza il giocatore successivo a scoprire la sua carta
coperta e ad applicarne l’effetto.
• Carta J: il giocatore in possesso della carta deve dare 1 punto vita al giocatore precedente.
• Carta Q: il giocatore in possesso della carta deve dare 1 punto vita al secondo giocatore
successivo (cio`e, saltando un giocatore).
• Carta 1: il giocatore in possesso della carta perde 1 punto vita, che verr`a lasciata sul campo
di gioco (le vite sul campo di gioco non vengono resettate a ogni fase, quindi rimangono anche
per le successive se non sono state riscosse).
• Carta K: il giocatore in possesso della carta, riceve tutti i punti vita lasciati sul campo di
gioco fino a quel momento.
*/
//void applyGenericCardEffects();

int main(void)
{
    int numberOfPlayers = 0;
    puts("Hi, welcome to the funcy cards game.\n Number of players to play the game 2 to 20");
    puts("Every player has 2 life points, the last one that will reamin It will be the winner"); 
    welcomeToTheGame(&numberOfPlayers);
    PLAYER *players = initializePlayers(&numberOfPlayers);

    CARDS deck[TOTAL_NUMBER_CARDS];
    memcpy(deck, basedeck, sizeof(deck)); // Copy basedeck array to deck array
    buildBaseDeck(deck);
    deckShuffle(deck, TOTAL_NUMBER_CARDS);
    dealCardsToPlayers(players, deck, &numberOfPlayers);
    selectFirstPlayerToPlay(players, &numberOfPlayers);

    return 0;
}

void welcomeToTheGame(int *numberOfPlayers)
{
    puts("Please insert the number of players: ");
    scanf("%d", &*numberOfPlayers);

    if (*numberOfPlayers < MIN_PLAYERS || *numberOfPlayers > MAX_PLAYERS)
    {
        puts("Wrong number of players, please enter a number of players between 2 and 20");
        welcomeToTheGame(numberOfPlayers);
    }
}

PLAYER* initializePlayers(int *numberOfPlayers)
{
    PLAYER *players = malloc(*numberOfPlayers * sizeof(PLAYER));
    if (players == NULL) {
        puts("Error: Memory allocation failed!");
        exit(1);
    }

    for (int i = 0; i < *numberOfPlayers; i++) {
        players[i].lifePoints = 2;
        sprintf(players[i].name, "Player %d", i + 1);
        //printf("%s \n", players[i].name);
    }

    return players;
}

void buildBaseDeck(CARDS *deck) 
{
    char *cardTypeNames[TYPES] = { "Flowers", "Picche", "Quadri", "Hearts"};
    char *cardValuesNames[VALUES] = { "1", "2", "3", "4", "5", "6", "7", "J", "K", "Q"};

    int deckIndex = 0;
    for (int type = 0; type < TYPES; type++) {
        for (int value = 0; value < VALUES; value++) {
            strcpy(deck[deckIndex].cardType, cardTypeNames[type]);
            strcpy(deck[deckIndex].cardValue, cardValuesNames[value]);
            deckIndex++;
        }
    }
}

void deckShuffle(CARDS *deck, int numberOfCards)
{
    srand(time(NULL));
    int numberOfCardsToProcess = (numberOfCards - 1);

    // First deck shuffle
    while (numberOfCardsToProcess > 0) {
        int randomCard = rand() % (numberOfCardsToProcess + 1);
        CARDS temp = deck[numberOfCardsToProcess];
        deck[numberOfCardsToProcess] = deck[randomCard];
        deck[randomCard] = temp;
        --numberOfCardsToProcess;
    }

    // Second deck shuffle
    for (int i = 1; i < numberOfCards; i++) 
    {
        // Check if adjacent cards types are equals
        if (strcmp(deck[i].cardType, deck[i - 1].cardType) == 0 || strcmp(deck[i].cardValue, deck[i - 1].cardValue) == 0) 
        { 
            for (int j = i + 1; j < numberOfCards; j++) 
            {
                // Find card with different type and value
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
        printf("Player: %s \n", players[i].name);
        for (int j = 0; j < MAX_CARDS_PLAYERS; j++)
        {
            printf("Card: %s %s\n", deck[deckIndex].cardType, deck[deckIndex].cardValue);
            players[i].deck[j] = deck[deckIndex];
            deckIndex++;
        }
    }
}

void selectFirstPlayerToPlay(PLAYER *players, int *numberOfPlayers)
{
    srand(time(NULL));
    int randomPlayer = rand() % (*numberOfPlayers + 1);

    PLAYER *originalPlayers = malloc(*numberOfPlayers * sizeof(PLAYER));
    if (players == NULL) {
        puts("Error: Memory allocation failed!");
        exit(1);
    }

    memcpy(originalPlayers, players, *numberOfPlayers * sizeof(PLAYER));

    // Order players ascending
    for (int i = 0; i < *numberOfPlayers; i++) 
    {
        int nextPlayer = (randomPlayer + i) % *numberOfPlayers;
        players[i] = originalPlayers[nextPlayer];
        //printf("Player %d: %s\n", i + 1, players[i].name);
    }

    free(originalPlayers);  // Cleanup
}