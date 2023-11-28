/*
 Description: Gladiator! Battle against the computer, who will prevail in the battlefield?
 Programmed by: Sean Denzel Yu Robenta - S23C
 Last modified: November 28, 2023
 Version: v0.0.1
 Acknowledgements:
    - Passing whole arrays as a pointer in C. (response from greggo)
        - https://stackoverflow.com/questions/13730786/c-pass-int-array-pointer-as-parameter-into-a-function

    - scanf invalid input handler
        - https://stackoverflow.com/questions/40551037/scanf-not-working-on-invalid-input
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#define MAX_WEAPONS         4
#define MAX_ARMOR           4
#define MAX_ITEM_STRLEN     32

#define GAME_STATE_MENU     1
#define GAME_STATE_STATS    2
#define GAME_STATE_SETUP    3
#define GAME_STATE_PREGAME  4
#define GAME_STATE_RUNNING  5
#define GAME_STATE_POSTGAME 6
#define GAME_STATE_EXIT     7
#define GAME_STATE_DEVMODE  8

#define GAME_ACTION_ATTACK  1
#define GAME_ACTION_BLOCK   2
#define GAME_ACTION_CHARGE  3

#define GAME_END_VICTORY    0
#define GAME_END_DEFEAT     1
#define GAME_END_DRAW       2

#define GAME_HEALTH_CONST   100
#define GAME_ATK_CONST      10
#define GAME_DEF_CONST      10
#define GAME_SPD_CONST      10

#define DIFFICULTY_INVALID  -1
#define DIFFICULTY_EASY     1
#define DIFFICULTY_HARD     2

#define INVALID_ITEM_ID     -1

void screenMainMenu(int* gameState, int * gameCls);
void screenDifficulty(int *gameState, int *gameDifficulty, int *gameCls);
void screenMatchSetup(
    int *gameState, 
    int *gameDifficulty,
    int *gameCls,
    
    int *playerWeapon,
    int *playerArmor,
    int *playerHealth,

    char (*weaponNames)[][MAX_ITEM_STRLEN],
    int (*weaponDamage)[MAX_WEAPONS],
    int (*weaponSpeed)[MAX_WEAPONS],

    char (*armorNames)[][MAX_ITEM_STRLEN],
    int (*armorDefense)[MAX_WEAPONS],
    int (*armorSpeed)[MAX_WEAPONS],
    int (*armorHealth)[MAX_WEAPONS],

    int *enemyWeapon,
    int *enemyArmor,
    int *enemyHealth
);
void screenMainGame(
    int *gameState, 
    int *gameDifficulty,
    int *gameCls,
    
    int *playerHealth,
    int *playerWeapon, 
    int *playerArmor,
    int *playerCharged,
    int *playerLastMove,
    int *playerLastDamage,

    int *enemyHealth,
    int *enemyWeapon,
    int *enemyArmor,
    int *enemyCharged,
    int *enemyLastMove,
    int *enemyLastDamage,

    char (*weaponNames)[][MAX_ITEM_STRLEN],
    int (*weaponDamage)[MAX_WEAPONS],
    int (*weaponSpeed)[MAX_WEAPONS],

    char (*armorNames)[][MAX_ITEM_STRLEN],
    int (*armorDefense)[MAX_WEAPONS],
    int (*armorSpeed)[MAX_WEAPONS],
    int (*armorHealth)[MAX_WEAPONS],

    int *totalDamageDealt,
    int *totalDamageReceived
);
void screenStats(int * gameState, int * gameCls);
void devMode(
    int* gameState,
    int* gameCls,
    
    char (*weaponNames)[][MAX_ITEM_STRLEN],
    int (*weaponDamage)[MAX_WEAPONS],
    int (*weaponSpeed)[MAX_WEAPONS],

    char (*armorNames)[][MAX_ITEM_STRLEN],
    int (*armorDefense)[MAX_WEAPONS],
    int (*armorSpeed)[MAX_WEAPONS],
    int (*armorHealth)[MAX_WEAPONS]
);
void postGame(
    int * gameState,
    int * gameCls,
    int result
);

int processDamage(int *targetHealth, int targetDefense, int attackerDamage);
int getEnemyDecisionAI(
    int playerHealth,
    int playerWeaponDamage, 
    int playerArmorDefense,
    int playerCharged,

    int enemyHealth,
    int enemyWeaponDamage,
    int enemyArmorDefense,
    int enemyCharged
);


void saveStats(
    int gameResult,
    int *totalDamageDealt,
    int *totalDamageReceived
);
void displayHealth(int health);
void print_center(char * msg);
void printLine();

int main()
{
    srand(time(NULL));

    // -- Here starts the variable initializations.
    char weaponNames[MAX_WEAPONS][MAX_ITEM_STRLEN] = { "Fist (No Weapon)", "Dagger",  "Katana",     "Broad Sword" };
    int weaponDamage[MAX_WEAPONS]                  = {                  0,       15,        20,                40 };
    int weaponSpeed[MAX_WEAPONS]                   = {                  10,       3,         0,                -5 };

    char armorNames[MAX_ARMOR][MAX_ITEM_STRLEN]    = { "Bare (No Armor)",  "Mythril", "Chain Mail", "Adamantite"  };
    int armorDefense[MAX_ARMOR]                    = {                 0,          3,            8,            18 };
    int armorSpeed[MAX_ARMOR]                      = {                10,          3,            0,            -4 };
    int armorHealth[MAX_ARMOR]                     = {                 0,          0,            5,            10 };

    // -- Adding the given default damage, speed values.
    for (int i = 0; i < MAX_WEAPONS; i++)
    {
        weaponDamage[i] += GAME_ATK_CONST;
        weaponSpeed[i] += GAME_SPD_CONST;
    }

    // -- Adding the given default defense value.
    for (int i = 0; i < MAX_ARMOR; i++)
        armorDefense[i] += GAME_DEF_CONST;
    
    // -- Game control variables.
    int 
        gameState       = GAME_STATE_MENU,
        gameDifficulty  = DIFFICULTY_INVALID,
        gameCls         = 1
    ;

    // -- Player control variables.
    int
        playerHealth  = GAME_HEALTH_CONST,
        playerWeapon  = INVALID_ITEM_ID,
        playerArmor   = INVALID_ITEM_ID,
        playerCharged = 0,
        playerLastMove = -1,
        playerLastDamage
    ;

    // -- Enemy control variables.
    int
        enemyHealth  = GAME_HEALTH_CONST,
        enemyWeapon  = INVALID_ITEM_ID,
        enemyArmor   = INVALID_ITEM_ID,
        enemyCharged = 0,
        enemyLastMove = -1,
        enemyLastDamage = 0
    ;

    // -- Statistics variables.
    int 
        totalDamageDealt = 0,
        totalDamageReceived = 0
    ;

    // ----------------------- [STATE MANAGEMENT] ------------------------ //
    // - This code is entirely managed by a mini "state management" system.
    // - The codebase is divided  into  multiple  states  as defined by the
    // - STATE_  macros.  With this, each state is assigned a function, and
    // - for  each  function,  the  address  of gameState must be passed to
    // - control the code's state throughout its runtime.
    // ------------------------------------------------------------------- //
    while (gameState != GAME_STATE_EXIT)
    {
        if(gameState == GAME_STATE_MENU) 
        {
            playerLastMove  = -1;
            enemyLastMove   = -1;
            playerWeapon    = INVALID_ITEM_ID;
            playerArmor     = INVALID_ITEM_ID;
            screenMainMenu(&gameState, &gameCls); 
        }

        else if(gameState == GAME_STATE_PREGAME)
            screenDifficulty(&gameState, &gameDifficulty, &gameCls);
        
        else if(gameState == GAME_STATE_SETUP)
            screenMatchSetup(
                &gameState, 
                &gameDifficulty,
                &gameCls,
                &playerWeapon, 
                &playerArmor, 
                &playerHealth,
                &weaponNames, 
                &weaponDamage, 
                &weaponSpeed,
                &armorNames, 
                &armorDefense, 
                &armorSpeed, 
                &armorHealth,
                &enemyWeapon,
                &enemyArmor,
                &enemyHealth
            );
        
        else if(gameState == GAME_STATE_RUNNING)
            screenMainGame(
                &gameState, 
                &gameDifficulty,
                &gameCls,
                &playerHealth,
                &playerWeapon, 
                &playerArmor, 
                &playerCharged,
                &playerLastMove,
                &playerLastDamage,
                &enemyHealth,
                &enemyWeapon,
                &enemyArmor,
                &enemyCharged,
                &enemyLastMove,
                &enemyLastDamage,
                &weaponNames, 
                &weaponDamage, 
                &weaponSpeed,
                &armorNames, 
                &armorDefense, 
                &armorSpeed, 
                &armorHealth,
                &totalDamageDealt,
                &totalDamageReceived
            );
        
        else if(gameState == GAME_STATE_STATS)
            screenStats(&gameState, &gameCls);
        
        else if(gameState == GAME_STATE_DEVMODE)
            devMode(
                &gameState,
                &gameCls,
                &weaponNames, 
                &weaponDamage, 
                &weaponSpeed,
                &armorNames, 
                &armorDefense, 
                &armorSpeed, 
                &armorHealth
            );   
        
        else if(gameState == GAME_STATE_POSTGAME)
            gameState = GAME_STATE_MENU;
        
        else gameState = GAME_STATE_EXIT;
    }

    return 0;
}

/* 
    screenDifficulty - Displays the difficulty selection screen and processes user input.

        Modifies the gameDifficulty pointer based on user input:
            - 1: Easy
            - 2: Hard
        Modifies the gameState pointer to GAME_STATE_SETUP if input is within valid range.
    
    Precondition: 
        - gameState must be a valid pointer to an integer representing the current game state.
        - gameDifficulty must be a valid pointer to an integer representing the game difficulty.
        - gameCls must be a valid pointer to an integer representing the game class.
    @param 
        gameState: Pointer to an integer representing the current game state.
        gameDifficulty: Pointer to an integer representing the selected game difficulty.
        gameCls: Pointer to an integer representing the game class.
    
*/
void screenDifficulty(int *gameState, int *gameDifficulty, int *gameCls)
{
    if(*gameCls) system("cls");
    printLine();
    printf("\n\n");

    print_center("[SELECT DIFFICULTY]"); printf("\n\n\033[1;33m");

    print_center("Easy"); printf("\033[1;0m\n");
    print_center("He's not the sharpest tool in the shed,"); printf("\n");
    print_center("but he can get by. (Random Moves)"); printf("\n\n");
    print_center("Press [1] to select this difficulty."); printf("\n\n");

    print_center("-------------");
    printf("\n\n\033[1;31m");

    print_center("Hard"); printf("\033[1;0m\n");
    print_center("She's an experienced fighter, she knows"); printf("\n");
    print_center("what she's doing. (AI Opponent)"); printf("\n\n");
    print_center("Press [2] to select this difficulty."); printf("\n\n");

    printLine();
    printf("\n> ");

    int input = 0, c;
    if(scanf("%d", &input) != -1) 
        while((c = getchar()) != EOF && c != '\n');

    *gameDifficulty = input;
    if(!(1 < input && input > 2))
        *gameState = GAME_STATE_SETUP;
}

/* 
    screenMainMenu - Displays the main menu for the game and processes user input.

    Acts as the "router" for the state management system put into place:
        - 1: Start Game
        - 2: Statistics
        - 3: Exit
        - 4: Enter developer mode

    Precondition: 
        - gameState must be a valid pointer to an integer representing the current game state.
        - gameCls must be a valid pointer to an integer representing the game class.
    @param 
        gameState: Pointer to an integer representing the current game state.
        gameCls: Pointer to an integer representing the game class.

*/
void screenMainMenu(int* gameState, int * gameCls)
{
    if(*gameCls) system("cls");
    printLine();
    printf("%s\n\n%s\n%s\n%s\n",
        "Welcome, Gladiator! Are you ready to go to war?",
        "1.) Start Game",
        "2.) Statistics",
        "3.) Exit"
    );
    printf("\n");
    printLine();
    printf("Type [4] to enter developer mode.");
    printf("\n\n> ");

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

    if(input == 1) *gameState = GAME_STATE_PREGAME;
    else if(input == 2) *gameState = GAME_STATE_STATS;
    else if(input == 3) *gameState = GAME_STATE_EXIT;
    else if(input == 4) 
    {
        if(*gameCls) system("cls");
        printf("Gladiator [Version 0.0.1]\n(c)Sean Denzel Robenta. De La Salle University.\n");
        *gameState = GAME_STATE_DEVMODE;
    }
}

/*
    processDamage(int *targetHealth, int targetDefense, int attackerDamage)

    This function processes the damage of the player. There is two scenarios that may
    happen, either a critical hit, that will ignore the target's Defense attribute, or
    a normal hit, where the damage will be deducted by the amount of defense the target
    has, if the defense is more than the damage, the damage will be 0.

    @param int * targetHealth - this pointer argument receives the address of the variable
                                to be deducted by the computed damage.

    @param int targetDefense  - this argument receives the DEF attribute of the target.

    @param int attackerDamage - the BASE amount of damage the attacker dealt to the player.

*/
void screenMatchSetup(
    int *gameState, 
    int *gameDifficulty,
    int *gameCls,

    int *playerWeapon,
    int *playerArmor,
    int *playerHealth,

    char (*weaponNames)[][MAX_ITEM_STRLEN],
    int (*weaponDamage)[MAX_WEAPONS],
    int (*weaponSpeed)[MAX_WEAPONS],

    char (*armorNames)[][MAX_ITEM_STRLEN],
    int (*armorDefense)[MAX_WEAPONS],
    int (*armorSpeed)[MAX_WEAPONS],
    int (*armorHealth)[MAX_WEAPONS],

    int *enemyWeapon,
    int *enemyArmor,
    int *enemyHealth
) {
    if(*gameCls) system("cls");
    printLine();

    if(*playerWeapon != INVALID_ITEM_ID && *playerArmor == INVALID_ITEM_ID)
        printf("Good weapon choice! Now, \033[1;33mget some armor\033[1;0m with that.");

    else if(*playerWeapon == INVALID_ITEM_ID && *playerArmor != INVALID_ITEM_ID)
        printf("Interesting choice of armor, no judgement though. Now \033[1;33mget a weapon\033[1;0m.");

    else if(*playerWeapon != INVALID_ITEM_ID && *playerArmor != INVALID_ITEM_ID)
        printf("\033[1;31mLet's get fighting!\033[1;0m");

    else
        printf("Oh hoo hoo! So you think you have what it takes?");

    printf("\n\nSelect the items for your loadout:\n");

    // -- Code displaying current loadout, to make the player consider their choices.
    if(*playerArmor != -1 || *playerWeapon != -1)
    {
        printLine();
        char hp_str[32];
        sprintf(hp_str, " (%s%d)", (*armorHealth)[*playerArmor] > -1 ? "+" : "", (*armorHealth)[*playerArmor]);
        printf("HP: 100%s | ATK: %d | DEF: %d | SPD: %d",

            // - The ternary operators here are used to check if a player has selected a weapon or armor,
            // - and if not, it will display 0 on the ATK or DEF respectively. This is done to prevent the
            // - program from accessing an invalid array index of INVALID_ITEM_ID (-1).
            (((*armorHealth)[*playerArmor] == INVALID_ITEM_ID || (*armorHealth)[*playerArmor] == 0) ? "" : hp_str),
            (*playerWeapon == INVALID_ITEM_ID ? 0 : (*weaponDamage)[*playerWeapon]),
            (*playerArmor  == INVALID_ITEM_ID ? 0 : (*armorDefense)[*playerArmor]),
            (*playerArmor  == INVALID_ITEM_ID ? 0 : (*armorSpeed)[*playerArmor]) + (*playerWeapon == INVALID_ITEM_ID ? 0 : (*weaponSpeed)[*playerWeapon])
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
                // -- Ternary operators were used here and forth to check for positive and negative values.
                // -- If the value is positive, add a + sign. If negative, dont add a plus sign as it is
                // -- automatically displayed.
                printf("%s", (*weaponSpeed)[*playerWeapon] > -1 ? "(+" : "(");
                printf("%d SPD)", (*weaponSpeed)[*playerWeapon]); 
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
                printf("%s", (*armorHealth)[*playerArmor] > -1 ? "(+" : "(");
                printf("%d HP) ", (*armorHealth)[*playerArmor]); 
            }
            if((*armorSpeed)[*playerArmor] != 0)
            {
                printf("%s", (*armorSpeed)[*playerArmor] > -1 ? "(+" : "(");
                printf("%d SPD)", (*armorSpeed)[*playerArmor]);
            }
            printf("\n");
        }
    }

    *playerHealth = GAME_HEALTH_CONST + (*armorHealth)[*playerArmor];

    printLine();

    // -- Algorithm to display both the armor and weapons in a column.
    for(int w = 0; w < MAX_WEAPONS; w++)
    {
        int // -- variables to check if the in the current iteration index is selected.
            wsel = (*playerWeapon == w),
            asel = (*playerArmor == w)
        ;

        if(wsel) // -- If selected...
            printf("\033[1;30m[%d]. %s\033[1;0m\t", w + 1, (*weaponNames)[w]);
        else     // -- If not selected...
            printf("%d]. %s\t", w + 1, (*weaponNames)[w]);

        // -- Using strlen to check the length of the weapon name, if it is less than 12 characters,
        // -- add an extra tab. Also an additional checker for when selected, with consideration to
        // -- 4 extra characters that were added to it.
        if(strlen((*weaponNames)[w]) < 12 && !(wsel && (strlen((*weaponNames)[w]) + 4 > 12)))
            printf("\t");

        printf(" || ");

        // -- similar logic as to the weapon printf.
        if(asel)
            printf("\033[1;30m[%d]. %s\033[1;0m\t", w + 5, (*armorNames)[w]);        
        else
            printf("%d]. %s\t", w + 5, (*armorNames)[w]);
        
        printf("\n");
    }

    // -- Display this message when both an armor and weapon is selected.
    if(*playerArmor != -1 && *playerWeapon != -1)
    {
        printLine();
        printf("%s%s",
        "It seems that you're ready, G na ba pare G?\n",
        "(type \"G\" to proceed, you can still change your loadout)\n\n> ");
    }
    else printf("\n> ");

    char choice, c; 
    int weapon_choice = INVALID_ITEM_ID;

    // -- Using char to allow the use of both letters and numbers in one input check.
    if(scanf(" %c", &choice) != -1)
        while((c = getchar()) != EOF && c != '\n');

    // -- ASCII codes of numbers 1 to 8.
    if(choice >= 48 && choice <= 59)
    {
        // -- Using sscanf to convert the input char into a usable int.
        sscanf(&choice, "%d", &weapon_choice);

        if((weapon_choice) >= 1 && 4 >= weapon_choice) 
            *playerWeapon = weapon_choice - 1;
        
        else if(weapon_choice >= 5 && weapon_choice <= 8) 
            *playerArmor = weapon_choice - 5;
        
    }
    // -- Game starter.
    else if((choice == 'G' || choice == 'g') && (*playerArmor != -1 && *playerWeapon != -1)) {
        if(*gameDifficulty == DIFFICULTY_EASY)
        {
            *enemyArmor = rand() % MAX_ARMOR;
            *enemyWeapon = rand() % MAX_WEAPONS;
        }
        else if(*gameDifficulty == DIFFICULTY_HARD)
        {
            *enemyArmor = *playerArmor;
            *enemyWeapon = *playerWeapon;
        }
        *enemyHealth = GAME_HEALTH_CONST + (*armorHealth)[*enemyArmor];
        *gameState = GAME_STATE_RUNNING;
    }
}   
/*
    screenMainGame(...)

    This is a recursive function that handles everything that goes on during a match.

    Process (Parts are indicated in the code):
        1. Display player and enemy health information.
        2. Variables initialization. Variables here will be used to for computations
           conducted for every match turn.
        3. Action Log: Most recent actions that were conducted by each player.
        4. Player input: Player gets to choose between 3 options of actions
        5. Action Logic: The backbone of the game's allowed actions and damage are
           found here.
        6. Endgame Logic: Here is where the winner of each match is determined.

*/
void screenMainGame(
    int *gameState, 
    int *gameDifficulty,
    int *gameCls,
    
    int *playerHealth,
    int *playerWeapon, 
    int *playerArmor,
    int *playerCharged,
    int *playerLastMove,
    int *playerLastDamage,

    int *enemyHealth,
    int *enemyWeapon,
    int *enemyArmor,
    int *enemyCharged,
    int *enemyLastMove,
    int *enemyLastDamage,

    char (*weaponNames)[][MAX_ITEM_STRLEN],
    int (*weaponDamage)[MAX_WEAPONS],
    int (*weaponSpeed)[MAX_WEAPONS],

    char (*armorNames)[][MAX_ITEM_STRLEN],
    int (*armorDefense)[MAX_WEAPONS],
    int (*armorSpeed)[MAX_WEAPONS],
    int (*armorHealth)[MAX_WEAPONS],

    int *totalDamageDealt,
    int *totalDamageReceived
)
{
    // [1] ---
    if(*gameCls) system("cls");
    printLine();
    printf("[BATTLE]\n");
    printf("  YOU: ");
    displayHealth(*playerHealth);
    printf("\n| %s | %s |", (*weaponNames)[*playerWeapon], (*armorNames)[*playerArmor]);
    printf("\n");
    printf("ENEMY: ");
    displayHealth(*enemyHealth);
    printf("\n| %s | %s |", (*weaponNames)[*enemyWeapon], (*armorNames)[*enemyArmor]);
    printf("\n\n");
    // --- [1]

    // [2] --- This is placed here as it will be used in the action log, so it needs to be initialized earlier.
    int totalPlayerSpeed = (*playerCharged) ? ((*armorSpeed)[*playerArmor] + (*weaponSpeed)[*playerWeapon]) * 2 : (*armorSpeed)[*playerArmor] + (*weaponSpeed)[*playerWeapon];
    int totalEnemySpeed = (*enemyCharged) ? ((*armorSpeed)[*enemyArmor] + (*weaponSpeed)[*enemyWeapon]) * 2 : (*armorSpeed)[*enemyArmor] + (*weaponSpeed)[*enemyWeapon];
    
    *playerCharged = 0; // - after every round, just set charged status to 0.
    *enemyCharged = 0;
    // --- [2]

    // [3] --- These are mostly vanity features, hence the inability to properly
    //     --- automate and had to be coded manually.
    if(*enemyLastMove == -1) printf("> Get ready for battle!\n");
    else
    {
        char eCrit[32] = "", pCrit[32] = "";
        if(*playerLastDamage == (*weaponDamage)[*playerWeapon] * 2 || *playerLastDamage == (*weaponDamage)[*playerWeapon])
            sprintf(pCrit, "\033[1;31m (CRITICAL HIT!)\033[1;0m");

        if(*enemyLastDamage == (*weaponDamage)[*enemyWeapon] * 2 || *enemyLastDamage == (*weaponDamage)[*enemyWeapon])
            sprintf(eCrit, "\033[1;31m (CRITICAL HIT!)\033[1;0m");
        
        if(totalPlayerSpeed < totalEnemySpeed)
        {
            if(*enemyLastMove == GAME_ACTION_ATTACK) 
                printf("> Enemy used attack!%s\n", eCrit);
            
            else if(*enemyLastMove == GAME_ACTION_BLOCK) 
                printf("> The enemy blocked your attack!\n");
            
            else if(*enemyLastMove == GAME_ACTION_CHARGE) 
                printf("> The enemy is going in for a charge!\n");

            if(*playerLastMove == GAME_ACTION_ATTACK) 
                printf("> You used attack!%s\n", pCrit);
            
            else if(*playerLastMove == GAME_ACTION_BLOCK) 
                printf("> You blocked the enemy attack!\n");
            
            else if(*playerLastMove == GAME_ACTION_CHARGE) 
                printf("> You're going in for a charge!\n");        
        }
        else
        {
            if(*playerLastMove == GAME_ACTION_ATTACK) 
                printf("> You used attack!%s\n", pCrit);
            
            else if(*playerLastMove == GAME_ACTION_BLOCK) 
                printf("> You blocked the enemy attack!\n");
            
            else if(*playerLastMove == GAME_ACTION_CHARGE) 
                printf("> You're going in for a charge!\n");

            if(*enemyLastMove == GAME_ACTION_ATTACK) 
                printf("> Enemy used attack!%s\n", eCrit);
            
            else if(*enemyLastMove == GAME_ACTION_BLOCK) 
                printf("> The enemy blocked your attack!\n");
            
            else if(*enemyLastMove == GAME_ACTION_CHARGE) 
                printf("> The enemy is going in for a charge!\n");
        }
    }
    // --- [3]

    // [4] ---
    printLine();
    if(!(*playerLastMove >= 1 && *playerLastMove <= 3) && *playerLastMove != -1)
        printf("\033[1;31mERROR: Invalid action! Please try again.\033[1;0m\n\n");

    print_center("Choose an action:");
    printf("\n");
    print_center("1.] Attack     2.] Block     3.] Charge");
    printf("\n");
    printLine();
    printf("\n> ");

    int c, action = -1;
    if(scanf(" %d", &action) != -1)
        while((c = getchar()) != EOF && c != '\n');
    // --- [4]

    // [5] --- deciding enemy action depending on the game difficulty. If in easy mode, just
    //     --- randomize. If hard, use the decision maker function.
    int enemy_action = 
        (*gameDifficulty == DIFFICULTY_EASY) 
        ? (rand() % 3) + 1 : getEnemyDecisionAI(
        *playerHealth,
        (*weaponDamage)[*playerWeapon], 
        (*armorDefense)[*playerArmor],
        *playerCharged,

        *enemyHealth,
        (*weaponDamage)[*enemyWeapon],
        (*armorDefense)[*enemyArmor],
        *enemyCharged
    );
    *playerLastMove = action;

    // -- These handle all the custom conditions that were required, allowing for a more seamless
    // -- initialization into a variable.
    int 
        player_damage_amp =                    (*playerCharged) ? (*weaponDamage)[*playerWeapon] * 2 : (*weaponDamage)[*playerWeapon],
        player_armor_amp  =       (action == GAME_ACTION_BLOCK) ?  (*armorDefense)[*playerArmor] * 2 : (*armorDefense)[*playerArmor],
        enemy_damage_amp  =                     (*enemyCharged) ?  (*weaponDamage)[*enemyWeapon] * 2 : (*weaponDamage)[*enemyWeapon],
        enemy_armor_amp   = (enemy_action == GAME_ACTION_BLOCK) ?   (*armorDefense)[*enemyArmor] * 2 : (*armorDefense)[*enemyArmor]
    ;

    // -- checker for a valid move.
    if(*playerLastMove >= 1 && *playerLastMove <= 3)
    {
        *enemyLastMove = enemy_action;

        // -- Dealing damage.
        if(action == GAME_ACTION_ATTACK)
            *totalDamageDealt += *playerLastDamage = processDamage(&(*enemyHealth), enemy_armor_amp, player_damage_amp);

        if(enemy_action == GAME_ACTION_ATTACK)
            *totalDamageReceived += *enemyLastDamage = processDamage(&(*playerHealth), player_armor_amp, enemy_damage_amp);

        // - We only set the value of this variable here as our "_amp" calculations will automatically do it.
        if(action == GAME_ACTION_CHARGE)
            *playerCharged = 1;

        if(enemy_action == GAME_ACTION_CHARGE)
            *enemyCharged = 1;
        // --- [5]

        // [6] --- Speed only matters when determining winning conditions. Here is the logic
        //     --- to it.
        int result = -1;
        if(*playerHealth < 1 && *enemyHealth < 1)
        {
            if(totalPlayerSpeed > totalEnemySpeed)
                result = GAME_END_VICTORY;
                
            else if(totalPlayerSpeed < totalEnemySpeed)
                result = GAME_END_DEFEAT;

            else
                result = GAME_END_DRAW;
        }
        else if(*playerHealth < 1 || *enemyHealth < 1)
            result = (*playerHealth < 1) ? GAME_END_DEFEAT : GAME_END_VICTORY;

        if(result != -1)
        {
            *gameState = GAME_STATE_POSTGAME;
            postGame(gameState, gameCls, result);
            saveStats(result, totalDamageDealt, totalDamageReceived);
        }
    }
    else 
        *playerLastMove = INVALID_ITEM_ID;
    // --- [6]
}

/*
    processDamage(int *targetHealth, int targetDefense, int attackerDamage)

    This function processes the damage of the player. There is two scenarios that may
    happen, either a critical hit, that will ignore the target's Defense attribute, or
    a normal hit, where the damage will be deducted by the amount of defense the target
    has, if the defense is more than the damage, the damage will be 0.

    @param int * targetHealth - this pointer argument receives the address of the variable
                                to be deducted by the computed damage.

    @param int targetDefense  - this argument receives the DEF attribute of the target.

    @param int attackerDamage - the BASE amount of damage the attacker dealt to the player.

*/

int processDamage(int *targetHealth, int targetDefense, int attackerDamage)
{
    int damage = 0;
    if((rand() % 100) < 15)
        damage = attackerDamage;

    else 
        damage = (attackerDamage < targetDefense) ? 0 : attackerDamage - targetDefense;

    *targetHealth -= damage;
    return damage;
}

/*
    getEnemyDecisionAI(int * gameState, int result)

    This function contains multiple custom conditions in consideration of the following
    factors: 
        - playerHealth,
        - playerWeaponDamage, 
        - playerArmorDefense,
        - playerCharged,

        - enemyHealth,
        - enemyWeaponDamage,
        - enemyArmorDefense,
        - enemyCharged.

    These game factors are known by each party in the game, and the enemy will make the
    most optimized decision off of that.

    The only random factor in this, is that if there is no special condition, the enemy
    decision will be a 60% chance to charge, and a 40% chance to attack. Though if the
    enemy is currently charged, they will attack. If the enemy is charged, but predicts
    that the next move will be critical to them, they will choose to block. 
*/

int getEnemyDecisionAI(
    int playerHealth,
    int playerWeaponDamage, 
    int playerArmorDefense,
    int playerCharged,

    int enemyHealth,
    int enemyWeaponDamage,
    int enemyArmorDefense,
    int enemyCharged
) {
    int 
        decision = -1;

    int randChargeOrAttack = ((rand() % 99) > 59) ? GAME_ACTION_ATTACK : (enemyCharged) ? GAME_ACTION_ATTACK :  GAME_ACTION_CHARGE;
    decision = randChargeOrAttack;

    if(playerHealth + playerArmorDefense < enemyWeaponDamage * 2) {
        decision = GAME_ACTION_CHARGE;
    }

    if(enemyCharged) {
        decision = GAME_ACTION_ATTACK;
    }

    if(enemyHealth + enemyArmorDefense > playerWeaponDamage) {
        decision = ((rand() % 100) > 30) ? GAME_ACTION_ATTACK : GAME_ACTION_BLOCK;

        if(enemyCharged) {
            decision = GAME_ACTION_ATTACK;
        }
    }

    if(playerCharged && (enemyHealth + enemyArmorDefense < playerWeaponDamage * 2))
        decision = GAME_ACTION_BLOCK; 

    if(playerCharged) {
        decision = randChargeOrAttack;

        if(enemyCharged)
            decision = GAME_ACTION_ATTACK;
    }

    return decision;
}

/*
    postGame(int * gameState, int result)

    This function displays the result of the most recently played game, either victory,
    draw, or defeat. It also prompts the player to either go back to the main screen by
    pressing anything, or view their statistics by entering the integer 1.

    @param int * gameState - a pointer argument where where the address of the game's
                             current state is passed into.

    @param int result - an argument that receives the result of the recently played game.
        parameter precondition: 
            the passed value is either of the following:
                - GAME_END_VICTORY    (0)
                - GAME_END_DEFEAT     (1)
                - GAME_END_DRAW       (2)
*/

void postGame(
    int * gameState,
    int * gameCls,
    int result
)
{
    if(*gameCls) system("cls");
    printLine();
    printf("\n");
    if(result == GAME_END_DRAW)
    {
        print_center("[DRAW]");
        printf("\n");
        print_center("Did both win, or both lose?");
    }
    else if(result == GAME_END_DEFEAT)
    {
        print_center("[DEFEAT]");
        printf("\n");
        print_center("Tough fight, you'll get them next time.");
    }
    else if(result == GAME_END_VICTORY)
    {
        print_center("[VICTORY]");
        printf("\n");
         print_center("Good job, you got them! Keep it up, warrior.");
    }
    else
    {
        print_center("[?????]");
        printf("\n");
        print_center("How did you even get here?");
    }

    printf("\n\n");
    print_center("Type anything to go back to the main menu.");
    printf("\n");
    print_center("Type [1] to view your statistics.");
    printf("\n\n");

    printLine();

    int choice = 0, c;
    if(scanf(" %d", &choice) != -1)
        while((c = getchar()) != EOF && c != '\n');

    if(choice == 1)
        *gameState = GAME_STATE_STATS;

    else
        *gameState = GAME_STATE_MENU;
}

/*
    saveStats(int gameResult, *totalDamageDealt, *totalDamageReceived)

    This function opens the "gladiator_stats.txt" file to save the player's statistics
    throughout their play of the game. Allowing the user to know their lifetime stats
    even after exiting the program. the fopen() function also automatically creates 
    the file in case of it not existing in the first place.

    @param int gameResult - an argument that receives the result of the recently played
                            game.
        parameter precondition: 
            the passed value is either of the following:
                - GAME_END_VICTORY    (0)
                - GAME_END_DEFEAT     (1)
                - GAME_END_DRAW       (2)

    @param int * totalDamageDealt  - a pointer argument containing the total amount of
                                     damage a player dealt in the duration of the most
                                     recently played game.

    @param int * totalDamageDealt  - a pointer argument containing the total amount of
                                     damage a player received in the duration of the 
                                     most recently played game.
*/

void saveStats(
    int gameResult,
    int *totalDamageDealt,
    int *totalDamageReceived
) {
    int 
        games = 0,
        games_won = 0,
        games_draw = 0,
        games_lost = 0,
        damage_dealt = 0,
        damage_received = 0
    ;

    FILE *stats;
    stats = fopen ("gladiator_stats.txt", "r");
    char saveToFile[128];

    fgets(saveToFile, 128, stats);
    fclose(stats);
    sscanf(saveToFile, "%d %d %d %d %d %d",
        &games,
        &games_won,
        &games_draw,
        &games_lost,
        &damage_dealt,
        &damage_received
    );
    
    games           = games + 1;
    games_won       = gameResult == GAME_END_VICTORY ? games_won + 1 : games_won;
    games_draw      = gameResult == GAME_END_DRAW    ? games_draw + 1 : games_draw;
    games_lost      = gameResult == GAME_END_DEFEAT  ? games_lost + 1 : games_lost;
    damage_dealt    = damage_dealt + *totalDamageDealt;
    damage_received = damage_received + *totalDamageReceived;

    sprintf(saveToFile, "%d %d %d %d %d %d",
        games,
        games_won,
        games_draw,
        games_lost,
        damage_dealt,
        damage_received
    );
    stats = fopen ("gladiator_stats.txt", "w");
    fputs(saveToFile, stats);

    *totalDamageDealt = 0;
    *totalDamageReceived = 0;

    fclose(stats);
}

/*
    screenStats(int * gameState)

    @param int * gameState - a pointer argument where where the address of the game's
                             current state is passed into.

    This function opens the "gladiator_stats.txt" file to load the player's statistics
    throughout their play of the game. Allowing the user to know their lifetime stats
    even after exiting the program. the fopen() function also automatically creates 
    the file in case of it not existing in the first place.
*/
void screenStats(int * gameState, int *gameCls) {
    int 
        games = 0,
        games_won = 0,
        games_draw = 0,
        games_lost = 0,
        damage_dealt = 0,
        damage_received = 0
    ;

    FILE *stats;
    stats = fopen ("gladiator_stats.txt", "r");
    char saveToFile[128];

    fgets(saveToFile, 128, stats);
    fclose(stats);
    sscanf(saveToFile, "%d %d %d %d %d %d",
        &games,
        &games_won,
        &games_draw,
        &games_lost,
        &damage_dealt,
        &damage_received
    );
    
    if(*gameCls) system("cls");
    printLine();
    printf("\n");
    print_center("[MY GLADIATOR STATISTICS]");
    printf("\n\n");
    printf(
        "%s: %7d\n%s: %7d\n%s: %7d\n%s: %7d\n%s: %7d\n%s: %7d",
        "            Games played", games,
        "               Games won", games_won,
        "             Games drawn", games_draw,
        "              Games lost", games_lost,
        "   Lifetime Damage Dealt", damage_dealt,
        "Lifetime Damage Received", damage_received 
    );
    printf("\n\n");
    print_center("[X] Back <<");
    printf("\n\n");

    char choice = 0, c;
    printLine();

    if(scanf(" %c", &choice) != -1)
        while((c = getchar()) != EOF && c != '\n');

    if(choice == 'X' || choice == 'x')
    {
        *gameState = GAME_STATE_MENU;
    }
}


/* 

    devMode()

    A command processor to modify game variables and other settings.
    The simple gist of how it is coded right now is that all commands
    must have a maximum argument count of 4. "cmd" will store the whole
    command, and used sscanf to parse every token. fgets was used to
    allow the use of spaces in the user input.

    cmd > param[0] > param[1] > param[2] > param[3]

    @param 
        gameState: Pointer to the game state integer.
        gameCls: Pointer to the game class integer.
        weaponNames: Pointer to an array of weapon names.
        weaponDamage: Pointer to an array of weapon damage values.
        weaponSpeed: Pointer to an array of weapon speed values.
        armorNames: Pointer to an array of armor names.
        armorDefense: Pointer to an array of armor defense values.
        armorSpeed: Pointer to an array of armor speed values.
        armorHealth: Pointer to an array of armor health values.
    @return 
        No explicit return value. Modifies the provided pointers and arrays in-place.
*/

void devMode(
    int* gameState,
    int* gameCls,
    
    char (*weaponNames)[][MAX_ITEM_STRLEN],
    int (*weaponDamage)[MAX_WEAPONS],
    int (*weaponSpeed)[MAX_WEAPONS],

    char (*armorNames)[][MAX_ITEM_STRLEN],
    int (*armorDefense)[MAX_WEAPONS],
    int (*armorSpeed)[MAX_WEAPONS],
    int (*armorHealth)[MAX_WEAPONS]
)
{
    printf("\ngladev> ");
    char input[256]; 
    fgets(input, 256, stdin);

    char cmd[64], param[4][64];

    // memset(cmd, 0, 64);
    // for (int i = 0; i < 4; i++) 
    //     memset(param[i], 0, 64);

    for(int i = 0; i < 64; i++) {
        cmd[i] = 0;
    }

    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 64; j++) {
            param[i][j] = 0;
        }
    }

    sscanf(input, "%s %s %s %s %s", cmd, param[0], param[1], param[2], param[3]);

    if(!strcmp(cmd, "?") || !strcmp(cmd, "help"))
    {
        printf("%s%s%s%s%s%s",
            "?, help \t- Show list of commands.\n",
            "(a)rmor \t- Manage game armor.\n",
            "(w)eapon \t- Manage game weapons.\n",
            "(s)ettings\t- Manage game settings.\n",
            "ver \t\t- Show game version.\n",
            "cls \t\t- Clear dev terminal.\n",
            "exit \t\t- Exit dev mode.\n"
    );
    }
    else if(!strcmp(cmd, "w") || !strcmp(cmd, "weap") || !strcmp(cmd, "wep") || !strcmp(cmd, "weapon"))
    {
        if(!strcmp(param[0], "list"))
        {
            printf("#\tWeapon\t\t\tDamage\tSpeed\n");
            for(int i = 0; i < MAX_WEAPONS; i++)
            {
                printf("%d.)\t%s \t%s%s%d \t%d \n", 
                    i, 
                    (*weaponNames)[i],
                    strlen((*weaponNames)[i]) < 8  && strlen((*weaponNames)[i]) < 12 ? "\t" : "",
                    strlen((*weaponNames)[i]) < 12 ? "\t" : "",
                    (*weaponDamage)[i],
                    (*weaponSpeed)[i]
                );
            }
            printf("\nUSE: (w)eapon set [damage/speed] (weaponid) (amount)\n\n");
        }
        if(!strcmp(param[0], "set"))
        {
            if(!strcmp(param[1], "damage"))
            {
                int weaponid = -1, amount = -1;
                sscanf(param[2], "%d", &weaponid);
                sscanf(param[3], "%d", &amount);

                if(weaponid == -1 || amount == -1)
                    printf("USAGE: (w)eapon set damage (weaponid) (amount)\n");

                else
                {
                    printf("Successfully set %s (%d) damage from %d to %d.\n", (*weaponNames)[weaponid], weaponid, (*weaponDamage)[weaponid], amount);
                    (*weaponDamage)[weaponid] = amount;
                }   
            }
            if(!strcmp(param[1], "speed"))
            {
                int weaponid = -1, amount = -1;
                sscanf(param[2], "%d", &weaponid);
                sscanf(param[3], "%d", &amount);

                if(weaponid == -1 || amount == -1)
                    printf("USAGE: (w)eapon set speed (weaponid) (amount)\n");

                else
                {
                    printf("Successfully set %s (%d) speed from %d to %d.\n", (*weaponNames)[weaponid], weaponid, (*weaponSpeed)[weaponid], amount);
                    (*weaponSpeed)[weaponid] = amount;
                }
            }
        }
        else 
            printf("(w)eapon - Manage game weapons.\n\tlist - List weapons.\n\tset - Set [damage, speed] of weapon.\n");
    }
    else if(!strcmp(cmd, "a") || !strcmp(cmd, "armor"))
    {
        if(!strcmp(param[0], "list"))
        {
            printf("#\tArmor\t\t\tDefense\tSpeed\tHealth\n");
            for(int i = 0; i < MAX_WEAPONS; i++)
            {
                printf("%d.)\t%s \t%s%s%d \t%d \t%d\n", 
                    i, 
                    (*armorNames)[i],
                    strlen((*armorNames)[i]) < 6  && strlen((*armorNames)[i]) < 12 ? "\t" : "",
                    strlen((*armorNames)[i]) < 12 ? "\t" : "",
                    (*armorDefense)[i],
                    (*armorSpeed)[i],
                    (*armorHealth)[i]
                );
            }
            printf("\nUSE: (a)rmor set [defense/speed/health] (armorid) (amount)\n\n");
        }
        else if(!strcmp(param[0], "set"))
        {
            if(!strcmp(param[1], "speed"))
            {
                int armorid = -1, amount = -1;
                sscanf(param[2], "%d", &armorid);
                sscanf(param[3], "%d", &amount);

                if(armorid == -1 || amount == -1)
                    printf("USAGE: (a)rmor set speed (armorid) (amount)\n");

                else
                {
                    printf("Successfully set %s (%d) speed from %d to %d.\n", (*armorNames)[armorid], armorid, (*armorSpeed)[armorid], amount);
                    (*armorSpeed)[armorid] = amount;
                }
            }
            else if(!strcmp(param[1], "defense"))
            {
                int armorid = -1, amount = -1;
                sscanf(param[2], "%d", &armorid);
                sscanf(param[3], "%d", &amount);

                if(armorid == -1 || amount == -1)
                    printf("USAGE: (a)rmor set defense (armorid) (amount)\n");

                else
                {
                    printf("Successfully set %s (%d) defense from %d to %d.\n", (*armorNames)[armorid], armorid, (*armorDefense)[armorid], amount);
                    (*armorDefense)[armorid] = amount;
                }
            }
            else if(!strcmp(param[1], "health"))
            {
                int armorid = -1, amount = -1;
                sscanf(param[2], "%d", &armorid);
                sscanf(param[3], "%d", &amount);

                if(armorid == -1 || amount == -1)
                    printf("USAGE: (a)rmor set health (armorid) (amount)\n");

                else
                {
                    printf("Successfully set %s (%d) health from %d to %d.\n", (*armorNames)[armorid], armorid, (*armorHealth)[armorid], amount);
                    (*armorHealth)[armorid] = amount;
                }
            }
            else printf("(a)rmor - Manage game armor.\n\tlist - List armor.\n\tset - Set [defense, speed, health] of armor.\n");
        }
        else printf("(a)rmor - Manage game armor.\n\tlist - List armor.\n\tset - Set [defense, speed, health] of armor.\n");
    }
    else if(!strcmp(cmd, "settings") || !strcmp(cmd, "s"))
    {
        if(!strcmp(param[0], "toggle"))
        {
            if(!strcmp(param[1], "clearscreen"))
            {
                *gameCls = !*gameCls;
                printf("You have toggled screen clearing %s.", *gameCls ? "on" : "off");
            }
            else
            printf("%s%s",
                "(s)ettings [list/toggle]\n",
                "\tclearscreen - enable/disable system cls throughout the game.\n"
            );
        }
        else
            printf("%s%s",
                "(s)ettings [list/toggle]\n",
                "\tclearscreen - enable/disable system cls throughout the game.\n"
            );
    }    
    else if(!strcmp(cmd, "ver"))
    {
        printf("Gladiator [Version 0.0.1]\n(c) Sean Denzel Robenta. De La Salle University.\n");
    }
    else if(!strcmp(cmd, "cls"))
    {
        system("cls");
        printf("Gladiator [Version 0.0.1]\n(c) Sean Denzel Robenta. De La Salle University.\n");
    }
    else if(!strcmp(cmd, "exit"))
    {
        *gameState = GAME_STATE_MENU;
    }
    else printf("command not found. Use 'help' or '?' for a list of commands.");
}

/* 
    displayHealth(int health)

    @param int health - the basis that will be used to display the health.

    This function displays a healthbar, dividing the health every 5 points.
    It also contains custom coloring to give the player a sense of feeling
    of the amount of health they have left.

    This function can also handle overhealth (health over 100) in case of
    armor buffs.
*/
void displayHealth(int health)
{
    if(health > 0)
        printf("\033[1;31m");

    if(health > 40)
        printf("\033[1;33m");

    if(health > 60)
        printf("\033[1;32m");

    for(int i = 0; i < health / 5; i++)
        printf("%c", 178);

    for(int i = 0; i < (100 - health) / 5; i++)
        printf("%c", 176);

    printf(" %d%%\033[1;0m", health);
}

/* 
    print_center()

    prints the message in the center of a 50 character length line.

    @param char * message
*/

void print_center(char * message)
{
    int i = 3;
    while(i-- != 0)
    {
        if(i == 0 || i == 2)
        {
            for(int d = 0; d < (50 - strlen(message)) / 2; d++)
                printf(" ");
        }
        else 
            printf("%s", message);
    }
}

/* 
    printLine()

    linyang singkwenta.
*/

void printLine() 
{
    for (int i = 0; i < 50; i++) 
        printf("=");
    printf("\n");
}