/*
 * Le code contenu dans cette correction n'a pas pour but d'Ãªtre du code propre.
 * C'est du code d'attaque programmÃ© avec pour unique but d'Ãªtre fonctionnel.
 * La plupart des bonnes pratiques de codage ne sont donc pas respectÃ©es. (en particulier l'absence de codes d'erreur et l'utilisation de variables globales). Le but est de ne pas rendre le code encore plus difficile Ã  aprÃ©hender Ã  cause de la lourdeur des rÃ¨gles de programmation propre.
 *
 */


#include "rc4.hpp"
#include "aes.hpp"
#include "card.hpp"
#include "terminal.hpp"
#include <cstdio>
#include <cstring>
#include <cmath>

#include <array>
#include <vector>

myerror_t recoverPIN(Terminal &pirate);
void attackDPA(Terminal &pirate, uint8_t foundKey[16]);
void attackCPA(Terminal &pirate, uint8_t foundKey[16]);

int main()
{
        myerror_t res;

        Terminal pirate;
        Card card;
        uint8_t key[16];

        pirate.connect(&card);

        /** PIN code recovering **/

        res = recoverPIN(pirate);
        if ( res != NO_ERROR ) // PIN not found
        {
                return EXIT_FAILURE;
        }

        /** DPA attack **/

        attackDPA(pirate,key);

        /** CPA attack **/

        attackCPA(pirate,key);

        return 0;
}

/*************************************/
/********* PIN CODE RECOVERY *********/
/*************************************/

myerror_t recoverPIN(Terminal &pirate)
{
        myerror_t res = ERROR_UNEXPECTED;
        /* Code pour tricher */
        srand(time(NULL));
        printf("En trichant: ");
        for ( unsigned i = 0; i < 4; i++ )
                printf("%d ",rand() % 10);
        printf("\n");
        /* fin du code pour tricher */
        uint8_t pin[4] = {0,0,0,0};
        unsigned tries = 0;
        for ( unsigned index = 0; (index < 4) && (res != NO_ERROR); index++ )
        {
                unsigned max = 0;
                unsigned imax = 0;
                uint64_t diff_time;
                for ( unsigned i = 0; (i < 10) && (res != NO_ERROR); i++ )
                {
                        pin[index] = i;
                        res = pirate.measurePinResponse(&diff_time,pin);
                        tries++;
                        if ( diff_time > max )
                        {
                                max = diff_time;
                                imax = i;
                        }
                }
                if ( res != NO_ERROR )
                        pin[index] = imax;
        }
        if ( res != NO_ERROR )
                printf("PIN code not found after %d tries\n",tries);
        else
                printf("PIN code found: %d%d%d%d (%d tries)\n",pin[0],pin[1],pin[2],pin[3],tries);
        return res;
}

/****************************************/
/********* SIDE-CHANNEL ATTACKS *********/
/****************************************/

/* Test key guess */
bool testKey(Terminal &pirate, uint8_t guess[16])
{
        uint8_t plaintext[16], ciphertext[16];
        for ( unsigned i = 0; i < 16; i++ )
                plaintext[i] = 0;
        pirate.computeAES(plaintext,ciphertext);
        AES::decrypt(guess,ciphertext,plaintext);
        for ( unsigned i = 0; i < 16; i++ )
        {
                if ( plaintext[i] )
                {
                        printf("Attack failure.\n");
                        return false;
                }
                if ( i == 15 )
                {
                        printf("Key recovered: ");
                        for ( unsigned j = 0; j < 16; j++ )
                                printf("%02x ",guess[j]);
                        printf("\n");
                }
        }
        return true;
}

void attackDPA(Terminal &pirate, uint8_t foundKey[16])
{
        printf("DPA ATTACK\n");
        srand(time(NULL));
        uint8_t plaintext[16], ciphertext[16], trace[16], guess[16];
        float group_sums[16][256][2];
        unsigned group_cards[16][256][2];
        /* Init arrays */
        for ( unsigned b = 0; b < 16; b++ )
                for ( unsigned k = 0; k < 256; k++ )
                {
                        group_sums[b][k][0] = group_cards[b][k][0] = 0;
                        group_sums[b][k][1] = group_cards[b][k][1] = 0;
                }

        /* Loop on queries */
        for ( unsigned i = 0; i < 40; i++ )
        {
                /* Generate random plaintext */
                for ( unsigned j = 0; j < 16; j++ )
                        plaintext[j] = rand() & 0xFF;

                pirate.measureAES(plaintext,ciphertext,trace);

                /* Update tables */
                for ( unsigned b = 0; b < 16; b++ )
                        for ( unsigned k = 0; k < 256; k++ )
                        {
                                unsigned index = AES::SB[k ^ plaintext[b]] & 1;
                                group_cards[b][k][index]++;
                                group_sums[b][k][index] += trace[b];
                        }
        }

        for ( unsigned b = 0; b < 16; b++ )
        {
                /* Compute means */
                for ( unsigned k = 0; k < 256; k++ )
                {
                        group_sums[b][k][0] /= group_cards[b][k][0];
                        group_sums[b][k][1] /= group_cards[b][k][1];
                }
                /* Find max */
                float max = fabs(group_sums[0][0]-group_sums[0][1]);
                guess[b] = 0;
                for ( unsigned k = 1; k < 256; k++ )
                {
                        float diff = fabs(group_sums[b][k][0]-group_sums[b][k][1]);
                        if ( diff > max )
                        {
                                max = diff;
                                guess[b] = k;
                        }
                }
        }

        /* Recover the key */
        if ( testKey(pirate,guess) )
        {
                memcpy(foundKey,guess,16);
        }
}

/* Two pass correlation coefficient computation */
float correlationPearson(float X[256], float Y[256])
{
        /* First compute means */
        float Xmean = 0, Ymean = 0;
        for ( unsigned i = 0; i < 256; i++ )
        {
                Xmean += X[i];
                Ymean += Y[i];
        }
        Xmean /= 256;
        Ymean /= 256;
        /* Then the three sums */
        float xy = 0, x = 0, y = 0;
        for ( unsigned i = 0; i < 256; i++ )
        {
                xy += (X[i]-Xmean)*(Y[i]-Ymean);
                x += (X[i]-Xmean)*(X[i]-Xmean);
                y += (Y[i]-Ymean)*(Y[i]-Ymean);
        }
        /* Finally get the correlation coefficient */
        float res = xy / (sqrt(x)*sqrt(y));
        return res;
}

void attackCPA(Terminal &pirate, uint8_t foundKey[16])
{
        printf("CPA ATTACK\n");
        srand(time(NULL));
        uint8_t plaintext[16], ciphertext[16], trace[16], guess[16];
        float group_sums[16][256];
        unsigned group_cards[16][256];
        /* Init arrays */
        for ( unsigned b = 0; b < 16; b++ )
                for ( unsigned k = 0; k < 256; k++ )
                {
                        group_sums[b][k] = 0;
                        group_cards[b][k] = 0;
                }

        /* Loop on queries */
        for ( unsigned i = 0; i < 700; i++ )
        {
                /* Generate random plaintext */
                for ( unsigned j = 0; j < 16; j++ )
                        plaintext[j] = rand() & 0xFF;

                pirate.measureAES(plaintext,ciphertext,trace);

                /* Update tables */
                for ( unsigned b = 0; b < 16; b++ )
                {
                        group_cards[b][plaintext[b]]++;
                        group_sums[b][plaintext[b]] += trace[b];
                }
        }

        /* Now process these traces */
        float weights[256];
        for ( unsigned b = 0; b < 16; b++ )
        {
                float max = 0;
                guess[b] = 0;
                for ( unsigned k = 0; k < 256; k++ )
                {
                        /* Construct the Hamming weight table */
                        for ( unsigned p = 0; p < 256; p++ )
                        {
                                // If key was k, then we expect to have this Hamming weight for plaintext p
                                weights[p] = __builtin_popcount(AES::SB[k ^ p]);
                        }
                        /* And correlates */
                        float corr = correlationPearson(weights,group_sums[b]);
                        if ( corr > max )
                        {
                                max = corr;
                                guess[b] = k;
                        }
                }
        }

        /* Recover the key */
        if ( testKey(pirate,guess) )
        {
                memcpy(foundKey,guess,16);
        }
}
