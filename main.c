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
PLAYER* initializePlayers(int *numberOfPlayers, int lifePoints);
void buildBaseDeck(CARDS *deck);
void deckShuffle(CARDS *deck, int numberOfCards);
void dealCardsToPlayers(PLAYER *players, CARDS *deck, int *numberOfPlayers);
void selectFirstPlayerToPlay(PLAYER *players, int *numberOfPlayers);
void displayGameField(PLAYER *players, int *numberOfPlayers);
void gamePhase(PLAYER *players, CARDS *deck, int *numberOfPlayers);
void removeDeadPlayers(PLAYER *players, int *numberOfPlayers);

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
int displayCardEffect(CARDS *targetCard);
void applyCardEffect(CARDS *targetCard, PLAYER *players, int *indexPlayerTurn, int *numberOfPlayers);
int getIntInput(int min, int max);
int getWrappedIndex(int baseIndex, int offset, int numberOfPlayers);

int main(void)
{
    int numberOfPlayers = 0;
    puts("");
    puts("Hi, welcome to the funcy cards game.\n\nThe number of players to play with is between 2 and 20");
    puts("Each player has 2 life points, the last one remaining will be the winner"); 
    printf("If you want increase the life points of the players enter 1, otherwise 0 to continue: "); 
    int userInput = getIntInput(0, 1);
    int lifePoints = 2;
    if (userInput == 1) {
        printf("Enter the life point value each player will have: ");
        lifePoints = getIntInput(1, 10000);
    }
    welcomeToTheGame(&numberOfPlayers);

    PLAYER *players = initializePlayers(&numberOfPlayers, lifePoints);
    CARDS deck[TOTAL_NUMBER_CARDS];
    memcpy(deck, basedeck, sizeof(deck)); // Copy basedeck array to deck array
    buildBaseDeck(deck);

    do
    {
        gamePhase(players, deck, &numberOfPlayers);
    }
    while (numberOfPlayers > 1);
    
    return 0;
}

void gamePhase(PLAYER *players, CARDS *deck, int *numberOfPlayers)
{
    int indexPlayer = 0;
    int phase = 1;

    deckShuffle(deck, TOTAL_NUMBER_CARDS);
    dealCardsToPlayers(players, deck, numberOfPlayers);
    selectFirstPlayerToPlay(players, numberOfPlayers);

    while (phase > 0)
    {
        removeDeadPlayers(players, numberOfPlayers);
        // Check if only one player is alive
        int alivePlayers = checkPlayersLifePoints(players, numberOfPlayers);
        if (alivePlayers == 1)
        {
            // Declare the winner
            for (int i = 0; i < *numberOfPlayers; i++)
            {
                if (players[i].lifePoints > 0)
                {
                    printf("Game ended, the winner is the player %s\n", players[i].name);
                    return;
                }
            }
        }

        // Skip dead players
        if (players[indexPlayer].lifePoints == 0)
        {
            indexPlayer = (indexPlayer + 1) % (*numberOfPlayers);
            continue;
        }

        displayGameField(players, numberOfPlayers);
        printf("Turn for player: %s \n", players[indexPlayer].name);
        players[indexPlayer].round++;

        printf("Effect of your visible card:");
        applyCardEffect(players[indexPlayer].visibleCard, players, &indexPlayer, numberOfPlayers);

        if (players[indexPlayer].hiddenCard->isCardVisible == 1)
        {
            puts("Hidden card already visible, going to the next player...");
            indexPlayer = (indexPlayer + 1) % (*numberOfPlayers);
            continue;
        }

        puts("You can see your hidden card and optionally show it to apply the effect.");
        printf("Enter 1 to see the hidden card or 0 to finish the turn: ");
        int userInput = getIntInput(0, 1);

        if (userInput == 1)
        {
            // Reveal and optionally apply hidden card
            printf("Card Type and Value: %s %s\n", players[indexPlayer].hiddenCard->cardType, players[indexPlayer].hiddenCard->cardValue);
            int hasCardEffect = displayCardEffect(players[indexPlayer].hiddenCard);
            if (hasCardEffect)
            {
                printf("Enter 1 to apply the effect of the card or 0 to finish the turn: ");
                int showHiddenCard = getIntInput(0, 1);
                if (showHiddenCard == 1)
                {
                    applyCardEffect(players[indexPlayer].hiddenCard, players, &indexPlayer, numberOfPlayers);
                    removeDeadPlayers(players, numberOfPlayers);
                    players[indexPlayer].hiddenCard->isCardVisible = 1;
                    displayGameField(players, numberOfPlayers);
                }
            }
            
        }

        // Check if all players had one round
        int completedRounds = 0;
        for (int i = 0; i < *numberOfPlayers; i++)
        {
            if (players[i].round == 1)
                completedRounds++;
        }

        if (completedRounds == *numberOfPlayers)
        {
            phase = 0;  // End current phase
            puts("Phase finished, starting another phase...");
            completedRounds = 0;
        }

        indexPlayer = (indexPlayer + 1) % (*numberOfPlayers);
    }
}

void welcomeToTheGame(int *numberOfPlayers)
{
    printf("Please insert the number of players: ");
    scanf("%d", &*numberOfPlayers);
    puts("");

    if (*numberOfPlayers < MIN_PLAYERS || *numberOfPlayers > MAX_PLAYERS)
    {
        puts("Wrong number of players, please enter a number of players between 2 and 20");
        puts("");
        welcomeToTheGame(numberOfPlayers);
    }
}

PLAYER* initializePlayers(int *numberOfPlayers, int lifePoints)
{
    PLAYER *players = malloc(*numberOfPlayers * sizeof(PLAYER));
    if (players == NULL) {
        puts("Error: Memory allocation failed!");
        exit(1);
    }

    for (int i = 0; i < *numberOfPlayers; i++) {
        players[i].lifePoints = lifePoints;
        players[i].round = 0;
        sprintf(players[i].name, "Player %d", i + 1);
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

    for (int i = 0; i < numberOfCards; i++) {
        printf("Card: %s %s\n", deck[i].cardType, deck[i].cardValue);
    }
}

void dealCardsToPlayers(PLAYER *players, CARDS *deck, int *numberOfPlayers)
{
    int deckIndex = 0;
    for (int i = 0; i < *numberOfPlayers; i++) 
    {
        printf("Player: %s \n", players[i].name);
        players[i].visibleCard[0] = deck[deckIndex];
        players[i].hiddenCard[0] = deck[++deckIndex];
        players[i].hiddenCard->isCardVisible = 0;
        deckIndex++;
        printf("Card visible: %s %s\n", players[i].visibleCard[0].cardType, players[i].visibleCard[0].cardValue);
        printf("Card hidden: %s %s\n", players[i].hiddenCard[0].cardType, players[i].hiddenCard[0].cardValue);
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

int displayCardEffect(CARDS *targetCard)
{
    int hasCardEffect = 1;
    puts("");

    switch (*targetCard->cardValue)
    {
        case '1':
            puts("Effect: The player in possession of the card lost 1 life points");
            break;

        case '7':
            puts("Effect: The next player's hidden card will be show and the effect will applyed");
            break;

        case 'J':
            puts("Effect: The player in possession of the card give 1 life point to the previous player");
            break;

        case 'K':
            puts("Effect: The player in possession of the card takes all game field life points");
            break;

        case 'Q':
            puts("Effect: The player in possession of the card must give 1 life point to the second player next");
            break;

        default:
            puts("The given card doesn't have any special effect to apply");
            hasCardEffect = 0;
            break;
    }
    puts("");

    return hasCardEffect;
}

void applyCardEffect(CARDS *targetCard, PLAYER *players, int *indexPlayerTurn, int *numberOfPlayers)
{
    puts("");
    printf("Card Type and value: %s %s\n", targetCard->cardType, targetCard->cardValue);

    int currentPlayer = *indexPlayerTurn;
    int totalPlayers = *numberOfPlayers;

    switch (*targetCard->cardValue)
    {
        case '1':
            printf("%s lost 1 life point\n", players[currentPlayer].name);
            players[currentPlayer].lifePoints--;
            gameLifePoints++;
            break;

        case '7':
            int nextPlayer = getWrappedIndex(currentPlayer, 1, totalPlayers);
            printf("The next player's hidden card will be shown and the effect applied\n");
            applyCardEffect(players[nextPlayer].hiddenCard, players, &nextPlayer, numberOfPlayers);
            break;

        case 'J':
            int previousPlayer = getWrappedIndex(currentPlayer, -1, totalPlayers);
            printf("%s has to give 1 life point to %s\n", players[currentPlayer].name, players[previousPlayer].name);
            players[currentPlayer].lifePoints--;
            players[previousPlayer].lifePoints++;
            break;

        case 'K':
            printf("%s takes all game life points (%d)\n", players[currentPlayer].name, gameLifePoints);
            players[currentPlayer].lifePoints += gameLifePoints;
            gameLifePoints = 0;
            break;

        case 'Q':
            int targetPlayer = getWrappedIndex(currentPlayer, 2, totalPlayers);
            printf("%s has to give 1 life point to %s\n", players[currentPlayer].name, players[targetPlayer].name);
            players[currentPlayer].lifePoints--;
            players[targetPlayer].lifePoints++;
            break;

        default:
            puts("This card has no special effect.");
            break;
    }

    puts("");
}

int checkPlayersLifePoints(PLAYER *players, int *numberOfPlayers)
{
    int losers = 0;
    int winners = 0;

    for (int i = 0; i < *numberOfPlayers; i++)
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

void displayGameField(PLAYER *players, int *numberOfPlayers) 
{
    puts("");
    printf("==== CARD TABLE ====\n\n");
    for (int i = 0; i < *numberOfPlayers; i++) 
    {
        if (players[i].hiddenCard->isCardVisible == 1)
        {
            printf("%s: [%s %s] [%s %s]\n", players[i].name, players[i].visibleCard->cardType, players[i].visibleCard->cardValue, players[i].hiddenCard->cardType, players[i].hiddenCard->cardValue);
        }
        else
        {
            printf("%s: [%s %s] [%s]\n", players[i].name, players[i].visibleCard->cardType, players[i].visibleCard->cardValue, "hidden");
        }
    }
    printf("\n====================\n");
}

void removeDeadPlayers(PLAYER *players, int *numberOfPlayers) 
{
    int newCount = 0;
    for (int i = 0; i < *numberOfPlayers; i++) 
    {
        if (players[i].lifePoints > 0) 
        {
            // Keep this player, move it to the new position
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
        printf("Invalid input. Try again (%d-%d): ", min, max);
    }
    return input;
}

int getWrappedIndex(int baseIndex, int offset, int numberOfPlayers) 
{
    int result = (baseIndex + offset) % numberOfPlayers;
    if (result < 0)
        result += numberOfPlayers;
    return result;
}