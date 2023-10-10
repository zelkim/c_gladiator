#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define MAX_WEAPONS         4
#define MAX_ARMOR           4
#define MAX_ITEM_STRLEN     32

#define GAME_STATE_MENU     1
#define GAME_STATE_STATS    2
#define GAME_STATE_SETUP    3
#define GAME_STATE_PREGAME  4
#define GAME_STATE_RUNNING  5
#define GAME_STATE_EXIT     6

#define INVALID_ITEM_ID     -1

void printLine();
int screenMainMenu();
int screenMatchSetup(
    int *gameState, 
    int *playerWeapon,
    int *playerArmor,

    char (*weaponNames)[][MAX_ITEM_STRLEN],
    int (*weaponDamage)[MAX_WEAPONS],
    int (*weaponSpeed)[MAX_WEAPONS],

    char (*armorNames)[][MAX_ITEM_STRLEN],
    int (*armorDefense)[MAX_WEAPONS],
    int (*armorSpeed)[MAX_WEAPONS],
    int (*armorHealth)[MAX_WEAPONS]
);

int main()
{

    char weaponNames[MAX_WEAPONS][MAX_ITEM_STRLEN] = { "Fist (No Weapon)", "Dagger", "Katana", "Broad Sword" };
    int weaponDamage[MAX_WEAPONS] =                  {  0,                 15,       20,       40            };
    int weaponSpeed[MAX_WEAPONS] =                   {  10,                3,        0,        -5            };

    char armorNames[MAX_ARMOR][MAX_ITEM_STRLEN] =    { "Bare (No Armor)", "Mythril", "Chain Mail", "Adamantite"};
    int armorDefense[MAX_ARMOR] =                    { 0,                 3,         8,            18          };
    int armorSpeed[MAX_ARMOR] =                      { 10,                3,         0,            -4          };
    int armorHealth[MAX_ARMOR] =                     { 0,                 0,         5,            10          };

    int 
        gameState    = GAME_STATE_MENU,
        playerWeapon = INVALID_ITEM_ID,
        playerArmor  = INVALID_ITEM_ID
    ;

    while (gameState != GAME_STATE_EXIT)
    {
        if(gameState == GAME_STATE_MENU)
        {
            playerWeapon = INVALID_ITEM_ID;
            playerArmor  = INVALID_ITEM_ID;
            screenMainMenu(&gameState);
        }
        else if(gameState == GAME_STATE_SETUP)
        {
            screenMatchSetup(
                &gameState,
                &playerWeapon,
                &playerArmor,
                &weaponNames,
                &weaponDamage,
                &weaponSpeed,
                &armorNames,
                &armorDefense,
                &armorSpeed,
                &armorHealth
            );
        }
        else gameState = GAME_STATE_EXIT;
    }

    return 0;
}

int screenMainMenu(int* gameState)
{
    system("cls");
    printLine();
    printf("%s\n\n%s\n%s\n%s\n\n> ",
        "Welcome, Gladiator! Are you ready to go to war?",
        "1.) Start Game",
        "2.) Statistics",
        "3.) Exit"
    );

    int input = 0, c;

    //                               [INVALID INPUT HANDLER]
    // The while loop after this scanf  operation  handles if an input does not conform to
    // what  is asked,  in this case,  an integer (%d).  If an invalid in put is received,
    // scanf breaks and prematurely concludes as it detects a newline, and the non-integer
    // character is trying to be read by scanf over and over  again  as it is stuck in the
    // somewhere in the input stream.  This fix  works  as  getchar()  catches the invalid
    // input that  is stuck  in the inputstream and removes it as it is processed with the
    // functions.

    if(scanf("%d", &input) != -1)
        while((c = getchar()) != EOF && c != '\n');

    if(input == 1) *gameState = GAME_STATE_SETUP;
    else if(input == 2) *gameState = GAME_STATE_STATS;
    else if(input == 3) *gameState = GAME_STATE_EXIT;
    
    // else...
    return 0;
}
int screenMatchSetup(
    int *gameState, 
    int *playerWeapon,
    int *playerArmor,
    char (*weaponNames)[][MAX_ITEM_STRLEN],
    int (*weaponDamage)[MAX_WEAPONS],
    int (*weaponSpeed)[MAX_WEAPONS],

    char (*armorNames)[][MAX_ITEM_STRLEN],
    int (*armorDefense)[MAX_WEAPONS],
    int (*armorSpeed)[MAX_WEAPONS],
    int (*armorHealth)[MAX_WEAPONS]
) {
    system("cls");
    printLine();
    printf("%s\n%s\n",
        "Oh ho ho, so you think you have what it takes?",
        "Select the items for your loadout:"
    );

    if(*playerArmor != -1 || *playerWeapon != -1)
    {
        printLine();
        char hp_str[8];
        sprintf(hp_str, " (+%d)", (*armorHealth)[*playerArmor]);
        printf("HP: 100%s | ATK: %d | DEF: %d | SPD: %d",
            (((*armorHealth)[*playerArmor] == INVALID_ITEM_ID || (*armorHealth)[*playerArmor] == 0) ? "" : hp_str),
            (*playerWeapon == INVALID_ITEM_ID ? 0 : (*weaponDamage)[*playerWeapon]),
            (*playerArmor == INVALID_ITEM_ID ? 0 : (*armorDefense)[*playerArmor]),

            (*playerArmor == INVALID_ITEM_ID ? 0 : (*armorSpeed)[*playerArmor])
            + 
            (*playerWeapon == INVALID_ITEM_ID ? 0 : (*weaponSpeed)[*playerWeapon])
        );
        printf("\n\n");

        printf("Current Loadout:\n");
        if(*playerWeapon != -1)
        {
            printf("Weapon: %s > ATK: %d ", 
                (*weaponNames)[*playerWeapon], 
                (*weaponDamage)[*playerWeapon]
            );
            if((*weaponSpeed)[*playerWeapon] != 0)
            {
                printf("(+%d SPD)", (*weaponSpeed)[*playerWeapon]); 
            }
            printf("\n");
        }
        if(*playerArmor != -1)
        {
            printf("Armor: %s > DEF: %d ",
                (*armorNames)[*playerArmor],
                (*armorDefense)[*playerArmor]
            );
            if((*armorHealth)[*playerArmor] != 0)
            {
                printf("(+%d HP) ", (*armorHealth)[*playerArmor]); 
            }
            if((*armorSpeed)[*playerArmor] != 0)
            {
                printf("(+%d SPD)", (*armorSpeed)[*playerArmor]);
            }
            printf("\n");
        }
    }

    printLine();
    for(int w = 0; w < MAX_WEAPONS; w++)
    {
        int 
            wsel = (*playerWeapon == w),
            asel = (*playerArmor == w)
        ;

        printf("%s%d.%s %s", wsel ? "[" : "",  w + 1, wsel ? "]" : "", (*weaponNames)[w]);
        printf("\t");
        
        if(strlen((*weaponNames)[w]) < 12 && !(wsel && (strlen((*weaponNames)[w]) + 4 > 12)))
            printf("\t");

        // if(wsel && (strlen((*weaponNames)[w]) + 4 > 16))

        printf(" || ");
        printf("%s%d.%s %s", asel ? "[" : "",  w + 5, asel ? "]" : "", (*armorNames)[w]);
        printf("\n");
    }

    if(*playerArmor != -1 && *playerWeapon != -1)
    {
        printLine();
        printf("%s%s",
        "It seems that you're ready, G na ba pare G?\n",
        "(type \"G\" to proceed, you can still change your loadout)\n\n> ");
    }
    else
    {
        printf("\n> ");
    }

    char choice, c; 
    int weapon_choice = INVALID_ITEM_ID;
    if(scanf(" %c", &choice) != -1)
        while((c = getchar()) != EOF && c != '\n');

    if(choice >= 48 && choice <= 59)
    {
        sscanf(&choice, "%d", &weapon_choice);

        if((weapon_choice) >= 1 && 4 >= weapon_choice) 
        {
            *playerWeapon = weapon_choice - 1;
        }
        else if(weapon_choice >= 5 && weapon_choice <= 8) 
        {
            *playerArmor = weapon_choice - 5;
        }
    }
    else {
        if(choice == 'G' || choice == 'g')
        {
            *gameState = GAME_STATE_MENU;
        }
    }

    return 0;
}

void printLine() 
{
    for (int i = 0; i < 50; i++) 
        printf("=");
    printf("\n");
}