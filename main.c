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
} CARDS;

// static means it must be not modified
static CARDS basedeck[TOTAL_NUMBER_CARDS] = { 0 };

typedef struct
{
    char name[SIZE_NAME];
    CARDS visibleCard[1];
    CARDS hiddenCard[1];
    int lifePoints;
    int round;
} PLAYER;

int gameLifePoints = 0;

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
*/
int checkPlayersLifePoints(PLAYER *players, int *numberOfPlayers); // Il gioco termina quando rimane solo un giocatore al termine di una fase e gli altri sono stati tutti eliminati.

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
void applyCardEffect(CARDS *targetCard, PLAYER *players, int *indexPlayerTurn);

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
    
    int indexPlayer = 0;
    while (numberOfPlayers > 0)
    {
        // check players life points
        int result = checkPlayersLifePoints(players, &numberOfPlayers);
        if (result == 1)
        {
            PLAYER winnerPlayer;
            for (int i; i < numberOfPlayers; i++)
            {
                if (players[i].lifePoints > 0)
                {
                    winnerPlayer = players[i];
                }
            }
            printf("Game ended, the winner is the player %s\n", winnerPlayer.name);
            break;
        }

        // If player doesn't have life points go to the next player
        if (players[indexPlayer].lifePoints == 0)
        {
            numberOfPlayers--;
            indexPlayer++;
            continue;
        }
        
        printf("Turn for player: %s \n", players[indexPlayer].name);
        players[indexPlayer].round++;
        puts("Press enter to see the effect of your visible card"); 
        char enter = 0;
        while (enter != '\r' && enter != '\n') { enter = getchar(); }
        puts("");

        // show visible card effect
        applyCardEffect(players[indexPlayer].visibleCard, players, &indexPlayer);
        puts("");

        // check if the hidden card is already visible 
        puts("You can see your hidden card and optionally show it to apply the effect."); 

        int userInput = -1;
        puts("Enter 1 to see the hidden card or 0 to finish the turn: "); 
        scanf("%d", &userInput);
        switch (userInput)
        {
            case 1:
                // see hidden card
                printf("Card Type and value: %s %s\n", players[indexPlayer].hiddenCard->cardType, players[indexPlayer].hiddenCard->cardValue);
                int showHiddenCard = -1;
                puts("Enter 1 to apply the effect of the card or 0 to finish the turn: "); 
                scanf("%d", &showHiddenCard);
                if (showHiddenCard == 1)
                {
                    applyCardEffect(players[indexPlayer].hiddenCard, players, &indexPlayer);
                }
                puts("");
                break;
            case 0:
                // next player turn
                break;
            default:
                // wrong answer
                break;
        }

        indexPlayer++;
    }

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
            players[i].visibleCard[j] = deck[deckIndex];
            players[i].hiddenCard[j] = deck[deckIndex++];
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

void applyCardEffect(CARDS *targetCard, PLAYER *players, int *indexPlayerTurn)
{
    printf("Card Type and value: %s %s\n", *targetCard->cardType, *targetCard->cardValue);
    switch (*targetCard->cardValue)
    {
        /* Carta 1: il giocatore in possesso della carta perde 1 punto vita, che verr`a lasciata sul campo di gioco 
        (le vite sul campo di gioco non vengono resettate a ogni fase, quindi rimangono anche per le successive se non sono state riscosse).*/
        case '1':
            printf("Player %s lost 1 life points\n", *players[*indexPlayerTurn].name);
            players[*indexPlayerTurn].lifePoints--; 
            gameLifePoints++;
            break;
        //Carta 7: il giocatore in possesso della carta forza il giocatore successivo a scoprire la sua carta coperta e ad applicarne l’effetto.
        case '7':
            printf("The next player's hidden card will be show and the effect will applyed\n", *players[*indexPlayerTurn].name);
            applyCardEffect(players[*indexPlayerTurn + 1].hiddenCard, players, (indexPlayerTurn + 1));
            break;
        //Carta J: il giocatore in possesso della carta deve dare 1 punto vita al giocatore precedente.
        case 'J':
            printf("Player %s have to give 1 life point to the previous player\n", *players[*indexPlayerTurn].name);
            players[*indexPlayerTurn].lifePoints--; 
            players[*indexPlayerTurn - 1].lifePoints++; 
            break;
        //Carta K: il giocatore in possesso della carta, riceve tutti i punti vita lasciati sul campo digioco fino a quel momento.
        case 'K':
            printf("Player %s take all game life points\n", *players[*indexPlayerTurn].name);
            players[*indexPlayerTurn].lifePoints += gameLifePoints;
            gameLifePoints = 0; // rest game life points 
            break;
        //Carta Q: il giocatore in possesso della carta deve dare 1 punto vita al secondo giocatoresuccessivo (cio`e, saltando un giocatore).
        case 'Q':
            printf("Player %s have to give 1 life point to the player %s\n", *players[*indexPlayerTurn].name, *players[*indexPlayerTurn + 2].name);
            players[*indexPlayerTurn].lifePoints--; 
            players[*indexPlayerTurn + 2].lifePoints++; 
            break;
        default:
            puts("The given card doens't have any special effect to apply");
            break;
    }
}

int checkPlayersLifePoints(PLAYER *players, int *numberOfPlayers)
{
    int losers = 0;
    int winners = 0;

    for (int i; i < *numberOfPlayers; i++)
    {
        if (players[i].lifePoints == 0)
        {
            losers++;
        }
        else
        {
            winners++;
        }
    }

    if (winners == 1) // If remain only one player the game finishes
    {
        return 1;
    }
    else // The game continue
    {
        return 0;
    }
}