#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MIN_PLAYERS 2
#define MAX_PLAYERS 5

#define MAX_LIFE_POINTS 2

#define MAX_CARDS_TO_GIVE 2

#define PLAYERS_TURN 1

#define NUM_TYPE_CARDS 4
#define MAX_VALUE_CARDS 10
#define TOTAL_NUMBER_CARDS 40

struct baseDeckModel {
    struct baseCardModel flowersCards [MAX_VALUE_CARDS];
    struct baseCardModel piccheCards [MAX_VALUE_CARDS];
    struct baseCardModel quadriCards [MAX_VALUE_CARDS];
    struct baseCardModel heartsCards [MAX_VALUE_CARDS];
};

struct baseCardModel {
    char typeCard;
    int valueCard; 
};

// Game development main methods
/*
1. Mescolare il mazzo
2. Distribuire ad ogni giocatore 2 carte: una coperta e una scoperta.
3. Scegliere casualmente il primo giocatore della fase.
*/
void welcomeToTheGame();
struct baseDeckModel buildBaseDeck();
struct baseDeckModel *deckShuffle(struct baseDeckModel *deck);
void distributeCards();
void selectFirstPlayerToPlay();

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
void assignsTurnsToPlayers();
void applyUncoveredCardEffect();
void showHiddenCard(); // Give player the choice to apply or not the effect
void checkThePlayersWhoLost(); // Il gioco termina quando rimane solo un giocatore al termine di una fase e gli altri sono stati tutti eliminati.

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
void applyGenericCardEffects();

int main(void)
{
    struct baseDeckModel *deck;
    int *numberOfPlayers = 0;

    puts("Hi, welcome to the funcy cards game.\n Number of players to play the game 2 to 20");
    puts("Every player has 2 life points, the last one that will reamin It will be the winner");
    
    welcomeToTheGame(*numberOfPlayers);
    *deck = buildBaseDeck();
    deckShuffle(deck);
    return 0;
}

void welcomeToTheGame(int *numberOfPlayers)
{
    puts("Please insert the number of players: ");
    scanf("%d", &numberOfPlayers);

    if (numberOfPlayers < MIN_PLAYERS || numberOfPlayers > MAX_PLAYERS)
    {
        puts("Wrong number of players, please enter a number of players between 2 to 20");
        welcomeToTheGame(numberOfPlayers);
    }
}

struct baseDeckModel buildBaseDeck() {
    struct baseDeckModel deck;

    for (int i = 0; i < MAX_VALUE_CARDS; i++) {
        deck.flowersCards[i].typeCard = 'F';    //FLOWERS
        deck.flowersCards[i].valueCard = i + 1;

        deck.piccheCards[i].typeCard = 'P';     //PICCHE
        deck.piccheCards[i].valueCard = i + 1;

        deck.quadriCards[i].typeCard = 'Q';     //QUADRI
        deck.quadriCards[i].valueCard = i + 1;

        deck.heartsCards[i].typeCard = 'H';     //HEARTS
        deck.heartsCards[i].valueCard = i + 1;
    }

    return deck;
}

struct baseDeckModel *deckShuffle(struct baseDeckModel *deck)
{
    srand(time(NULL));   // Initialization, should only be called once.
    int numberOfCardsToProcess = TOTAL_NUMBER_CARDS;
    int randomValue = rand()%TOTAL_NUMBER_CARDS;      // Returns a pseudo-random integer between 0 and RAND_MAX.
    int previousCard = NULL;
    int selectedCard;
    struct baseCardModel flowers = *deck->flowersCards;
    struct baseCardModel picche = *deck->piccheCards;
    struct baseCardModel quadri = *deck->quadriCards;
    struct baseCardModel hearts = *deck->heartsCards;

    while (numberOfCardsToProcess > 0) {
        
        --numberOfCardsToProcess;
    }
}


