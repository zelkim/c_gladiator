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
#define GAME_STATE_POSTGAME 6
#define GAME_STATE_EXIT     7

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

#define INVALID_ITEM_ID     -1

void printLine();
int screenMainMenu();
int screenMatchSetup(
    int *gameState, 
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
int screenMainGame(
    int *gameState, 
    
    int *playerHealth,
    int *playerWeapon, 
    int *playerArmor,
    int *playerCharged,
    int *playerLastMove,

    int *enemyHealth,
    int *enemyWeapon,
    int *enemyArmor,
    int *enemyCharged,
    int *enemyLastMove,

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

int processDamage(int *targetHealth, int targetDefense, int attackerDamage);
void displayHealth(int health);

void print_center(char * msg);
void saveStats(
    int gameResult,
    int *totalDamageDealt,
    int *totalDamageReceived
);

void postGame(
    int result,
    char* playerWeapon,
    int* playerArmor,
    char* enemyWeapon,
    int* enemyArmor
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

    for (int i = 0; i < MAX_WEAPONS; i++)
    {
        weaponDamage[i] += GAME_ATK_CONST;
        weaponSpeed[i] += GAME_SPD_CONST;
    }

    for (int i = 0; i < MAX_ARMOR; i++)
    {
        armorDefense[i] += GAME_DEF_CONST;
    }
    

    int gameState    = GAME_STATE_MENU;

    int
        playerHealth  = GAME_HEALTH_CONST,
        playerWeapon  = INVALID_ITEM_ID,
        playerArmor   = INVALID_ITEM_ID,
        playerCharged = 0,
        playerLastMove = -1
    ;

    int
        enemyHealth  = GAME_HEALTH_CONST,
        enemyWeapon  = INVALID_ITEM_ID,
        enemyArmor   = INVALID_ITEM_ID,
        enemyCharged = 0,
        enemyLastMove = -1
    ;

    int 
        totalDamageDealt = 0,
        totalDamageReceived = 0
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
        }
        else if(gameState == GAME_STATE_RUNNING)
        {
            screenMainGame(
                &gameState, 
                &playerHealth,
                &playerWeapon, 
                &playerArmor, 
                &playerCharged,
                &playerLastMove,
                &enemyHealth,
                &enemyWeapon,
                &enemyArmor,
                &enemyCharged,
                &enemyLastMove,
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

    *playerHealth = GAME_HEALTH_CONST + (*armorHealth)[*playerArmor];

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
    else printf("\n> ");

    char choice, c; 
    int weapon_choice = INVALID_ITEM_ID;
    if(scanf(" %c", &choice) != -1)
        while((c = getchar()) != EOF && c != '\n');

    if(choice >= 48 && choice <= 59)
    {
        sscanf(&choice, "%d", &weapon_choice);

        if((weapon_choice) >= 1 && 4 >= weapon_choice) 
            *playerWeapon = weapon_choice - 1;
        
        else if(weapon_choice >= 5 && weapon_choice <= 8) 
            *playerArmor = weapon_choice - 5;
        
    }
    else if(choice == 'G' || choice == 'g') {
        *enemyArmor = *playerArmor;
        *enemyWeapon = *playerWeapon;
        *enemyHealth = GAME_HEALTH_CONST + (*armorHealth)[*enemyArmor];
        *gameState = GAME_STATE_RUNNING;
    }

    return 0;
}   

int screenMainGame(
    int *gameState, 
    
    int *playerHealth,
    int *playerWeapon, 
    int *playerArmor,
    int *playerCharged,
    int *playerLastMove,

    int *enemyHealth,
    int *enemyWeapon,
    int *enemyArmor,
    int *enemyCharged,
    int *enemyLastMove,

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
    // system("cls");
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

    int totalPlayerSpeed = (*armorSpeed)[*playerArmor] + (*weaponSpeed)[*playerWeapon];
    int totalEnemySpeed = (*armorSpeed)[*enemyArmor] + (*weaponSpeed)[*enemyWeapon];

    if(*enemyLastMove == -1) printf("> Get ready for battle!\n");
    else
    {
        if(((*playerCharged) ? totalPlayerSpeed * 2 : totalPlayerSpeed) < ((*enemyCharged) ? totalEnemySpeed * 2 : totalEnemySpeed))
        {
            if(*enemyLastMove == GAME_ACTION_ATTACK) 
                printf("> Enemy used attack!\n");
            
            else if(*enemyLastMove == GAME_ACTION_BLOCK) 
                printf("> The enemy blocked your attack!\n");
            
            else if(*enemyLastMove == GAME_ACTION_CHARGE) 
                printf("> The enemy is going in for a charge!\n");

            if(*playerLastMove == GAME_ACTION_ATTACK) 
                printf("> You used attack!\n");
            
            else if(*playerLastMove == GAME_ACTION_BLOCK) 
                printf("> You blocked the enemy attack!\n");
            
            else if(*playerLastMove == GAME_ACTION_CHARGE) 
                printf("> You're going in for a charge!\n");        
        }
        else
        {
            if(*playerLastMove == GAME_ACTION_ATTACK) 
                printf("> You used attack!\n");
            
            else if(*playerLastMove == GAME_ACTION_BLOCK) 
                printf("> You blocked the enemy attack!\n");
            
            else if(*playerLastMove == GAME_ACTION_CHARGE) 
                printf("> You're going in for a charge!\n");

            if(*enemyLastMove == GAME_ACTION_ATTACK) 
                printf("> Enemy used attack!\n");
            
            else if(*enemyLastMove == GAME_ACTION_BLOCK) 
                printf("> The enemy blocked your attack!\n");
            
            else if(*enemyLastMove == GAME_ACTION_CHARGE) 
                printf("> The enemy is going in for a charge!\n");
        }
    }
    printLine();
    print_center("Choose an action:");
    printf("\n");
    print_center("1.] Attack     2.] Block     3.] Charge");
    printf("\n");
    printLine();
    printf("\n> ");

    int action = 0, c;

    int enemy_action = getEnemyDecisionAI(
        *playerHealth,
        (*weaponDamage)[*playerWeapon], 
        (*armorDefense)[*playerArmor],
        *playerCharged,

        *enemyHealth,
        (*weaponDamage)[*enemyWeapon],
        (*armorDefense)[*enemyArmor],
        *enemyCharged
    );

    scanf("%d", &action);
        while((c = getchar()) != EOF && c != '\n');

    *playerLastMove = action;
    *enemyLastMove = enemy_action;

    printf("damage pre-amp: %d\n", (*weaponDamage)[*playerWeapon]);
    int 
        player_damage_amp =                    (*playerCharged) ? (*weaponDamage)[*playerWeapon] * 2 : (*weaponDamage)[*playerWeapon],
        player_armor_amp  =       (action == GAME_ACTION_BLOCK) ?  (*armorDefense)[*playerArmor] * 2 : (*armorDefense)[*playerArmor],
        enemy_damage_amp  =                     (*enemyCharged) ?  (*weaponDamage)[*enemyWeapon] * 2 : (*weaponDamage)[*enemyWeapon],
        enemy_armor_amp   = (enemy_action == GAME_ACTION_BLOCK) ?   (*armorDefense)[*enemyArmor] * 2 : (*armorDefense)[*enemyArmor]
    ;

    *playerCharged = 0;
    *enemyCharged = 0;
    
    printf("damage post-amp: %d\n", player_damage_amp);

    if(action == GAME_ACTION_ATTACK)
        *totalDamageDealt += processDamage(&(*enemyHealth), enemy_armor_amp, player_damage_amp);

    if(enemy_action == GAME_ACTION_ATTACK)
        *totalDamageReceived += processDamage(&(*playerHealth), player_armor_amp, enemy_damage_amp);

    if(action == GAME_ACTION_CHARGE)
        *playerCharged = 1;

    if(enemy_action == GAME_ACTION_CHARGE)
        *enemyCharged = 1;

    int result = -1;
    if(*playerHealth < 1 && *enemyHealth < 1)
    {
        if(((*playerCharged) ? totalPlayerSpeed * 2 : totalPlayerSpeed) > ((*enemyCharged) ? totalEnemySpeed * 2 : totalEnemySpeed))
            result = GAME_END_VICTORY;
            
        else if(((*playerCharged) ? totalPlayerSpeed * 2 : totalPlayerSpeed) < ((*enemyCharged) ? totalEnemySpeed * 2 : totalEnemySpeed))
            result = GAME_END_DEFEAT;

        else
            result = GAME_END_DRAW;
    }
    else if(*playerHealth < 1 || *enemyHealth < 1)
        result = (*playerHealth < 1) ? GAME_END_DEFEAT : GAME_END_VICTORY;

    if(result != -1)
    {
        *gameState = GAME_STATE_POSTGAME;
        postGame(
            result,
            (*weaponNames)[*playerWeapon],
            playerArmor,
            (*weaponNames)[*enemyWeapon],
            enemyArmor
        );
        saveStats(result, totalDamageDealt, totalDamageReceived);
    }
    return 0;
}

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

    decision = ((rand() % 99) > 59) ? GAME_ACTION_ATTACK : (enemyCharged) ? GAME_ACTION_ATTACK :  GAME_ACTION_CHARGE;

    if(playerHealth + playerArmorDefense < enemyWeaponDamage * 2) {
        decision = GAME_ACTION_CHARGE;
    }

    if(enemyCharged) {
        decision = GAME_ACTION_ATTACK;
    }

    if(enemyHealth + enemyArmorDefense > playerWeaponDamage) {
        decision = ((rand() % 100) > 30) ? GAME_ACTION_BLOCK : GAME_ACTION_ATTACK;

        if(enemyCharged) {
            decision = GAME_ACTION_ATTACK;
        }
    }

    if(playerCharged && (enemyHealth + enemyArmorDefense < playerWeaponDamage * 2))
        decision = GAME_ACTION_BLOCK; 

    if(playerCharged) {
        decision = GAME_ACTION_BLOCK;

        if(enemyCharged)
            decision = GAME_ACTION_ATTACK;
    }

    return decision;
}

void postGame(
    int result,
    char* playerWeapon,
    int* playerArmor,
    char* enemyWeapon,
    int* enemyArmor
)
{
    system("cls");
    printLine();
    if(result == GAME_END_DRAW)
    {
        print_center("[DRAW]");
        printf("\n");
        print_center("Did both win, or both lose?");
    }
    if(result == GAME_END_DEFEAT)
    {
        print_center("[DEFEAT]");
        printf("\n");
        print_center("Tough fight, you'll get them next time.");
    }
    if(result == GAME_END_VICTORY)
    {
        print_center("[VICTORY]");
        printf("\n");
        print_center("Good job, you got them! Keep it up, warrior.");
    }

    printf("\n\n");
    // printf("STATS [YOU | ENEMY]\n:");
    // printf("DMG Dealt: \t%d | %d");
    // printf("DMG Absorbed: \t%d | %d");
    // printf("");
    printLine();


}

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
    stats = fopen ("gladiator_stats.txt", "w+");
    fscanf(stats, "%d %d %d %d %d %d",
        &games,
        &games_won,
        &games_draw,
        &games_lost,
        &damage_dealt,
        &damage_received
    );
    fprintf(stats, "%d %d %d %d %d %d",
        games + 1,
        gameResult == GAME_END_VICTORY ? games_won + 1 : games_won,
        gameResult == GAME_END_DRAW    ? games_draw + 1 : games_draw,
        gameResult == GAME_END_DEFEAT  ? games_lost + 1 : games_lost,
        damage_dealt + *totalDamageDealt,
        damage_received + *totalDamageReceived
    );
    *totalDamageDealt = 0;
    *totalDamageReceived = 0;
}

void displayHealth(int health)
{
    for(int i = 0; i < health / 5; i++)
        printf("%c", 178);

    for(int i = 0; i < (100 - health) / 5; i++)
        printf("%c", 176);

    printf(" %d%%", health);
}

void print_center(char * message)
{
    for(int i = 0; i < (50 - strlen(message)) / 2; i++)
        printf(" ");

    printf("%s", message);

    for(int i = 0; i < (50 - strlen(message)) / 2; i++)
        printf(" ");
}

void printLine() 
{
    for (int i = 0; i < 50; i++) 
        printf("=");
    printf("\n");
}