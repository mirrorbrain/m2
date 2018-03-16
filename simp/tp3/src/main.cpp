#include "rc4.hpp"
#include "aes.hpp"
#include "card.hpp"
#include "terminal.hpp"
#include <cstdio>
#include <cstring>
#include <cmath>

#include <array>
#include <vector>

/*
 *** PREMIERE PARTIE (TP 3) ***
 ***
 * Impératif *
 * L'objectif de la première partie du TP est de réussir à prendre la main sur la carte target qui est dans l'état verrouillé.
 * Pour cela il faut retrouver le code PIN mais attention il y a un nombre d'essais limités.
 * Pour information, le code PIN est tiré aléatoirement à chaque création d'un objet de type carte.
 * L'idée est d'exploiter une faiblesse de l'implémentation du test de vérification de code PIN à l'aide du terminal pirate et de sa fonction measurePinResponse().
 * Il est aussi possible de tricher pour ceux qui en ont le temps, vous pouvez passer quelques minutes à chercher comment (cette astuce ne serait pas utilisable si on avait une vraie carte).
 *
 * Optionnel *
 * Le fichier rc4.cpp fourni une implémentation de RC4.
 * Si vous en avez le temps, vous pouvez calculer les distributions des premiers octets de suite chiffrante pour retrouver le graphiques vus en cours. Si vous voulez aller plus loin vous pouvez regarder les distributions conditionnelles : quelle biais a-t-on sur l'octet n+1 connaissant l'octet n ?
 *
 * Il peut aussi être intéressant de regarder l'implémentation de l'AES. En effet c'est une implémentation en table qui est la cible typique d'attaques sur le temps d'exécution. Il est assez peu probable qu'une attaque puisse être effectuée avec un PC actuel comme cible mais il peut être bon de le vérifier. Il faut pour cela regarder si le temps de calcul dépend des valeurs d'indices dans les accès aux tables (Te0, Te1, Te2 et Te3).
 *
 *
 *** DEUXIÈME PARTIE (TP 4) ***
 ***
 * Impératif *
 * La carte étant maintenant déverrouillée l'objectif est de retrouver la clef secrète AES générée elle aussi à la création de l'objet. Ici aussi il est possible de tricher mais le but est d'effectuer une attaque DPA en utilisant le terminal pirate et sa fonction measureAES(). Pour cela utilisez la formule vue en cours.
 *
 * Optionnel *
 * S'il vous reste du temps, effectuez une attaque de type CPA (voire template si vous êtes très efficaces).
 *
 * Vous pouvez aussi chercher à attaquer de "vraies" mesures de consommation. Vous avez pour cela le fichier AES_SW.bin qui contient des mesures et le header "bin_reader.hpp" qui contient le code permettant de les lire.
 *
 *
 *** TROISIEME PARTIE (TP 5) ***
 ***
 * Impératif *
 * Effectuez l'attaque en fautes de Piret et Quisquater pour retrouver une seconde fois la clef de chiffrement.
 * La fonction faultyAES de la carte émule une injection de faute.
 *
 */

myerror_t recoverPIN(Terminal *pirate, Card *target) {
    myerror_t res = ERROR_UNEXPECTED;

    /* Code pour tricher */
    srand(time(NULL));
    printf("En trichant: ");
    for (unsigned i = 0; i < 4; i++)
        printf("%d ", rand() % 10);
    printf("\n");
    /* fin du code pour tricher */

    uint8_t		i = 0, j = 0, k = 0, l = 0, pin[4] = { 0, 0, 0, 0 };
    uint64_t	comp_time, best_time = 0;
    for (uint8_t t = 0; t < 10 && res != NO_ERROR; t++) {
        pin[0]	= t;
        res		= pirate->measurePinResponse(&comp_time, pin);

        if (comp_time > best_time) {
            best_time	= comp_time;
            i			= t;
        }
    }
    pin[0] = i;
    for (uint8_t t = 1; t < 10 && res != NO_ERROR; t++) {
        pin[1]	= t;
        res		= pirate->measurePinResponse(&comp_time, pin);

        if (comp_time > best_time) {
            best_time	= comp_time;
            j			= t;
        }
    }
    pin[1] = j;
    for (uint8_t t = 1; t < 10 && res != NO_ERROR; t++) {
        pin[2]	= t;
        res		= pirate->measurePinResponse(&comp_time, pin);

        if (comp_time > best_time) {
            best_time	= comp_time;
            k			= t;
        }
    }
    pin[2] = k;
    for (uint8_t t = 1; t < 10 && res != NO_ERROR; t++) {
        pin[3]	= t;
        res		= target->unlock(pin);

        l = t;
    }

    printf("pin = %d%d%d%d (res = %d)\n", i, j, k, l, res);

    return res;
}

/****************************************/
/********* SIDE-CHANNEL ATTACKS *********/
/****************************************/

/* Test key guess */
bool testKey(Terminal *pirate, uint8_t guess[16]) {
    uint8_t plaintext[16], ciphertext[16];

    for (unsigned i = 0; i < 16; i++)
        plaintext[i] = 0;
    pirate->computeAES(plaintext, ciphertext);
    AES::decrypt(guess, ciphertext, plaintext);
    for (unsigned i = 0; i < 16; i++) {
        if (plaintext[i]) {
            printf("Attack failure.\n");
            return false;
        }
        if (i == 15) {
            printf("Key recovered: ");
            for (unsigned j = 0; j < 16; j++)
                printf("%02x ", guess[j]);
            printf("\n");
        }
    }
    return true;
}

void attackDPA(Terminal *pirate, uint8_t foundKey[16]) {
    printf("DPA ATTACK\n");
    srand(time(NULL));
    uint8_t		plaintext[16], ciphertext[16], trace[16], guess[16];
    float		group_sums[16][256][2];
    unsigned	group_cards[16][256][2];
    /* Init arrays */
    for (unsigned b = 0; b < 16; b++)
        for (unsigned k = 0; k < 256; k++) {
            group_sums[b][k][0] = group_cards[b][k][0] = 0;
            group_sums[b][k][1] = group_cards[b][k][1] = 0;
        }

    /* Loop on queries */
    for (unsigned i = 0; i < 40; i++) {
        /* Generate random plaintext */
        for (unsigned j = 0; j < 16; j++)
            plaintext[j] = rand() & 0xFF;

        pirate->measureAES(plaintext, ciphertext, trace);

        /* Update tables */
        for (unsigned b = 0; b < 16; b++)
            for (unsigned k = 0; k < 256; k++) {
                unsigned index = AES::SB[k ^ plaintext[b]] & 1;
                group_cards[b][k][index]++;
                group_sums[b][k][index] += trace[b];
            }
    }

    for (unsigned b = 0; b < 16; b++) {
        /* Compute means */
        for (unsigned k = 0; k < 256; k++) {
            group_sums[b][k][0] /= group_cards[b][k][0];
            group_sums[b][k][1] /= group_cards[b][k][1];
        }
        /* Find max */
        float max = fabs(group_sums[0][0] - group_sums[0][1]);
        guess[b] = 0;
        for (unsigned k = 1; k < 256; k++) {
            float diff = fabs(group_sums[b][k][0] - group_sums[b][k][1]);
            if (diff > max) {
                max			= diff;
                guess[b]	= k;
            }
        }
    }

    /* Recover the key */
    if (testKey(pirate, guess))
        memcpy(foundKey, guess, 16);
}

/* Two pass correlation coefficient computation */
float correlationPearson(float X[256], float Y[256]) {
    /* First compute means */
    float Xmean = 0, Ymean = 0;

    for (unsigned i = 0; i < 256; i++) {
        Xmean	+= X[i];
        Ymean	+= Y[i];
    }
    Xmean	/= 256;
    Ymean	/= 256;
    /* Then the three sums */
    float xy = 0, x = 0, y = 0;
    for (unsigned i = 0; i < 256; i++) {
        xy	+= (X[i] - Xmean) * (Y[i] - Ymean);
        x	+= (X[i] - Xmean) * (X[i] - Xmean);
        y	+= (Y[i] - Ymean) * (Y[i] - Ymean);
    }
    /* Finally get the correlation coefficient */
    float res = xy / (sqrt(x) * sqrt(y));
    return res;
}

void attackCPA(Terminal *pirate, uint8_t foundKey[16]) {
    printf("CPA ATTACK\n");
    srand(time(NULL));
    uint8_t		plaintext[16], ciphertext[16], trace[16], guess[16];
    float		group_sums[16][256];
    unsigned	group_cards[16][256];
    /* Init arrays */
    for (unsigned b = 0; b < 16; b++)
        for (unsigned k = 0; k < 256; k++) {
            group_sums[b][k]	= 0;
            group_cards[b][k]	= 0;
        }

    /* Loop on queries */
    for (unsigned i = 0; i < 700; i++) {
        /* Generate random plaintext */
        for (unsigned j = 0; j < 16; j++)
            plaintext[j] = rand() & 0xFF;

        pirate->measureAES(plaintext, ciphertext, trace);

        /* Update tables */
        for (unsigned b = 0; b < 16; b++) {
            group_cards[b][plaintext[b]]++;
            group_sums[b][plaintext[b]] += trace[b];
        }
    }

    /* Now process these traces */
    float weights[256];
    for (unsigned b = 0; b < 16; b++) {
        float max = 0;
        guess[b] = 0;
        for (unsigned k = 0; k < 256; k++) {
            /* Construct the Hamming weight table */
            for (unsigned p = 0; p < 256; p++)
                // If key was k, then we expect to have this Hamming weight for plaintext p
                weights[p] = __builtin_popcount(AES::SB[k ^ p]);
            /* And correlates */
            float corr = correlationPearson(weights, group_sums[b]);
            if (corr > max) {
                max			= corr;
                guess[b]	= k;
            }
        }
    }

    /* Recover the key */
    if (testKey(pirate, guess))
        memcpy(foundKey, guess, 16);
}

void attackAES(Terminal *pirate, uint8_t foundKey[16]) {
    printf("AES ATTACK\n");
    myerror_t res = ERROR_UNEXPECTED;

    // Prepare ciphertexts
    uint8_t plaintext[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, tampertext[16], ciphertext[16];
    pirate->tamperAES(plaintext, tampertext);
    pirate->computeAES(plaintext, ciphertext);

    // SRinv
    AES.SRinv(tampertext);
    AES.SRinv(ciphertext);

    // SBinv
    for (uint8_t i = 0; i < 16; i++) {
        tampertext	= AES.SBinv[tampertext[i]];
        ciphertext	= AES.SBinv[ciphertext[i]];
    }
}

int main() {
    myerror_t res;

    printf("AES autotest result: %d\n", AES::test());
    printf("RC4 autotest result: %d\n", RC4::test());

    Card *		target	= new Card();
    Terminal *	pirate	= new Terminal();
    uint8_t		key[16];

    pirate->connect(target);

    /** PIN code recovering **/
    res = recoverPIN(pirate, target);

/*################################### TP 4 ###################################*/
    if (res == NO_ERROR) {
        /** DPA attack **/
        attackDPA(pirate, key);

        /** CPA attack **/
        attackCPA(pirate, key);

        /** AES attack **/
        attackAES(pirate, key);
    }
/*############################################################################*/

    return 0;
}
