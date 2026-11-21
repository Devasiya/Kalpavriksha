#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "players_data.h"

#define MAX_TEAMS 10
#define MAX_NAME 50

typedef enum
{
    BATSMAN = 1,
    BOWLER,
    ALL_ROUNDER
} Role;

typedef struct ODIPlayer
{
    int playerId;
    char name[MAX_NAME + 1];
    char teamName[MAX_NAME + 1];
    Role role;

    int totalRuns;
    float battingAverage;
    float strikeRate;
    int wickets;
    float economyRate;
    float performanceIndex;

    struct ODIPlayer *nextTeam;
    struct ODIPlayer *nextRole;
} ODIPlayer;

typedef struct
{
    int teamId;
    char name[MAX_NAME + 1];

    int totalPlayers;
    float averageBattingStrikeRate;

    float strikeRateSum;
    int battingPlayerCount;

    ODIPlayer *playerHead;
    ODIPlayer *roleHeads[3];
} Team;

typedef struct
{
    ODIPlayer *player;
    int teamIndex;
} HeapNode;

typedef struct
{
    HeapNode nodes[MAX_TEAMS];
    int size;
} MinHeap;

/* Global team array */
Team teamList[MAX_TEAMS];
int totalTeams = 0;


// Utility functions

Role getRoleFromString(const char *role)
{
    if (strcmp(role, "Batsman") == 0)
        return BATSMAN;

    if (strcmp(role, "Bowler") == 0)
        return BOWLER;

    return ALL_ROUNDER;
}

const char *getRoleName(Role role)
{
    switch (role)
    {
        case BATSMAN:
            return "Batsman";

        case BOWLER:
            return "Bowler";

        case ALL_ROUNDER:
            return "All-Rounder";

        default:
            return "Unknown";
    }
}

int getRoleIndex(Role role)
{
    return (int)role - 1;
}

Role inputRole(void)
{
    int choice;

    printf("\n1. Batsman\n");
    printf("2. Bowler\n");
    printf("3. All-Rounder\n");
    printf("Enter role: ");

    scanf("%d", &choice);

    if (choice < 1 || choice > 3)
        return 0;

    return (Role)choice;
}

float calculatePerformanceIndex(ODIPlayer *player)
{
    switch (player->role)
    {
        case BATSMAN:
            return (player->battingAverage * player->strikeRate) / 100.0f;

        case BOWLER:
            return (player->wickets * 2) + (100.0f - player->economyRate);

        case ALL_ROUNDER:
            return ((player->battingAverage * player->strikeRate) / 100.0f)
                   + (player->wickets * 2);

        default:
            return 0.0f;
    }
}


//Team searching - Binary Search O(log t)


Team *searchTeamById(int teamId)
{
    int low = 0;
    int high = totalTeams - 1;

    while (low <= high)
    {
        int mid = low + (high - low) / 2;

        if (teamList[mid].teamId == teamId)
            return &teamList[mid];

        if (teamList[mid].teamId < teamId)
            low = mid + 1;
        else
            high = mid - 1;
    }

    return NULL;
}

Team *searchTeamByName(const char *name)
{
    for (int i = 0; i < totalTeams; i++)
    {
        if (strcmp(teamList[i].name, name) == 0)
            return &teamList[i];
    }

    return NULL;
}


//Player comparison
//Higher Performance Index comes first

int playerComesBefore(ODIPlayer *a, ODIPlayer *b)
{
    if (a->performanceIndex != b->performanceIndex)
        return a->performanceIndex > b->performanceIndex;

    return a->playerId < b->playerId;
}


//Linked list insertion

void insertPlayer(Team *team, ODIPlayer *player)
{
    /* Insert into team's main player list */
    player->nextTeam = team->playerHead;
    team->playerHead = player;

    team->totalPlayers++;

    /* Maintain average strike rate for batsmen
       and all-rounders only */
    if (player->role == BATSMAN || player->role == ALL_ROUNDER)
    {
        team->strikeRateSum += player->strikeRate;
        team->battingPlayerCount++;

        team->averageBattingStrikeRate =
            team->strikeRateSum / team->battingPlayerCount;
    }

    /* Insert into role-specific sorted linked list */
    int index = getRoleIndex(player->role);
    ODIPlayer **current = &team->roleHeads[index];

    while (*current != NULL && playerComesBefore(*current, player))
    {
        current = &(*current)->nextRole;
    }

    player->nextRole = *current;
    *current = player;
}


//Duplicate Player ID checking


int playerIdExists(int id)
{
    for (int i = 0; i < totalTeams; i++)
    {
        ODIPlayer *current = teamList[i].playerHead;

        while (current != NULL)
        {
            if (current->playerId == id)
                return 1;

            current = current->nextTeam;
        }
    }

    return 0;
}


//Initialization

void initializeTeams(void)
{
    totalTeams = teamCount;

    for (int i = 0; i < totalTeams; i++)
    {
        teamList[i].teamId = i + 1;

        strcpy(teamList[i].name, teams[i]);

        teamList[i].totalPlayers = 0;
        teamList[i].averageBattingStrikeRate = 0.0f;

        teamList[i].strikeRateSum = 0.0f;
        teamList[i].battingPlayerCount = 0;

        teamList[i].playerHead = NULL;

        for (int j = 0; j < 3; j++)
            teamList[i].roleHeads[j] = NULL;
    }

    for (int i = 0; i < playerCount; i++)
    {
        Team *team = searchTeamByName(players[i].team);

        if (team == NULL)
            continue;

        ODIPlayer *newPlayer = malloc(sizeof(ODIPlayer));

        if (newPlayer == NULL)
        {
            printf("Memory allocation failed.\n");
            exit(1);
        }

        newPlayer->playerId = players[i].id;
        strcpy(newPlayer->name, players[i].name);
        strcpy(newPlayer->teamName, players[i].team);

        newPlayer->role = getRoleFromString(players[i].role);

        newPlayer->totalRuns = players[i].totalRuns;
        newPlayer->battingAverage = players[i].battingAverage;
        newPlayer->strikeRate = players[i].strikeRate;
        newPlayer->wickets = players[i].wickets;
        newPlayer->economyRate = players[i].economyRate;

        newPlayer->performanceIndex =
            calculatePerformanceIndex(newPlayer);

        newPlayer->nextTeam = NULL;
        newPlayer->nextRole = NULL;

        insertPlayer(team, newPlayer);
    }

    printf("Initialization completed successfully.\n");
    printf("Teams initialized: %d\n", totalTeams);
    printf("Players initialized: %d\n", playerCount);
}


//Display helpers

void printPlayerHeader(void)
{
    printf("\n");
    printf("---------------------------------------------------------------------------------------------\n");
    printf("%-6s %-25s %-13s %-8s %-8s %-8s %-7s %-7s %-10s\n",
           "ID", "Name", "Role", "Runs", "Avg", "SR", "Wkts", "Eco", "Perf.Index");
    printf("---------------------------------------------------------------------------------------------\n");
}

void printPlayerRow(ODIPlayer *player)
{
    printf("%-6d %-25s %-13s %-8d %-8.2f %-8.2f %-7d %-7.2f %-10.2f\n",
           player->playerId,
           player->name,
           getRoleName(player->role),
           player->totalRuns,
           player->battingAverage,
           player->strikeRate,
           player->wickets,
           player->economyRate,
           player->performanceIndex);
}

void printPlayerRowWithTeam(ODIPlayer *player)
{
    printf("%-6d %-23s %-15s %-13s %-8d %-8.2f %-8.2f %-7d %-7.2f %-10.2f\n",
           player->playerId,
           player->name,
           player->teamName,
           getRoleName(player->role),
           player->totalRuns,
           player->battingAverage,
           player->strikeRate,
           player->wickets,
           player->economyRate,
           player->performanceIndex);
}


//1. Add Player to Team

void addPlayerToTeam(void)
{
    int teamId;

    printf("\nEnter Team ID to add player: ");
    scanf("%d", &teamId);

    Team *team = searchTeamById(teamId);

    if (team == NULL)
    {
        printf("Invalid Team ID.\n");
        return;
    }

    ODIPlayer *newPlayer = malloc(sizeof(ODIPlayer));

    if (newPlayer == NULL)
    {
        printf("Memory allocation failed.\n");
        return;
    }

    printf("\nEnter Player Details:\n");

    printf("Player ID: ");
    scanf("%d", &newPlayer->playerId);

    if (newPlayer->playerId < 1 || newPlayer->playerId > 1000)
    {
        printf("Player ID must be between 1 and 1000.\n");
        free(newPlayer);
        return;
    }

    if (playerIdExists(newPlayer->playerId))
    {
        printf("Player ID already exists.\n");
        free(newPlayer);
        return;
    }

    printf("Name: ");
    scanf(" %50[^\n]", newPlayer->name);

    newPlayer->role = inputRole();

    if (newPlayer->role == 0)
    {
        printf("Invalid role.\n");
        free(newPlayer);
        return;
    }

    printf("Total Runs: ");
    scanf("%d", &newPlayer->totalRuns);

    printf("Batting Average: ");
    scanf("%f", &newPlayer->battingAverage);

    printf("Strike Rate: ");
    scanf("%f", &newPlayer->strikeRate);

    printf("Wickets: ");
    scanf("%d", &newPlayer->wickets);

    printf("Economy Rate: ");
    scanf("%f", &newPlayer->economyRate);

    strcpy(newPlayer->teamName, team->name);

    newPlayer->performanceIndex =
        calculatePerformanceIndex(newPlayer);

    newPlayer->nextTeam = NULL;
    newPlayer->nextRole = NULL;

    insertPlayer(team, newPlayer);

    printf("\nPlayer added successfully to Team %s!\n", team->name);
}


//2. Display Players of a Specific Team

void displayPlayersOfTeam(void)
{
    int teamId;

    printf("\nEnter Team ID: ");
    scanf("%d", &teamId);

    Team *team = searchTeamById(teamId);

    if (team == NULL)
    {
        printf("Invalid Team ID.\n");
        return;
    }

    printf("\nPlayers of Team %s:\n", team->name);

    printPlayerHeader();

    ODIPlayer *current = team->playerHead;

    while (current != NULL)
    {
        printPlayerRow(current);
        current = current->nextTeam;
    }

    printf("---------------------------------------------------------------------------------------------\n");
    printf("Total Players: %d\n", team->totalPlayers);
    printf("Average Batting Strike Rate: %.2f\n",
           team->averageBattingStrikeRate);
}


//3. Sort Teams by Average Batting Strike Rate

void mergeTeams(Team **arr, Team **temp, int left, int mid, int right)
{
    int i = left;
    int j = mid + 1;
    int k = left;

    while (i <= mid && j <= right)
    {
        if (arr[i]->averageBattingStrikeRate >=
            arr[j]->averageBattingStrikeRate)
        {
            temp[k++] = arr[i++];
        }
        else
        {
            temp[k++] = arr[j++];
        }
    }

    while (i <= mid)
        temp[k++] = arr[i++];

    while (j <= right)
        temp[k++] = arr[j++];

    for (i = left; i <= right; i++)
        arr[i] = temp[i];
}

void mergeSortTeams(Team **arr, Team **temp, int left, int right)
{
    if (left >= right)
        return;

    int mid = left + (right - left) / 2;

    mergeSortTeams(arr, temp, left, mid);
    mergeSortTeams(arr, temp, mid + 1, right);

    mergeTeams(arr, temp, left, mid, right);
}

void displayTeamsByStrikeRate(void)
{
    Team *sorted[MAX_TEAMS];
    Team *temp[MAX_TEAMS];

    for (int i = 0; i < totalTeams; i++)
        sorted[i] = &teamList[i];

    mergeSortTeams(sorted, temp, 0, totalTeams - 1);

    printf("\nTeams Sorted by Average Batting Strike Rate\n");
    printf("------------------------------------------------------------------\n");
    printf("%-8s %-20s %-20s %-15s\n",
           "ID", "Team Name", "Avg Batting SR", "Total Players");
    printf("------------------------------------------------------------------\n");

    for (int i = 0; i < totalTeams; i++)
    {
        printf("%-8d %-20s %-20.2f %-15d\n",
               sorted[i]->teamId,
               sorted[i]->name,
               sorted[i]->averageBattingStrikeRate,
               sorted[i]->totalPlayers);
    }

    printf("------------------------------------------------------------------\n");
}


//4. Display Top K Players of a Team by Role
//O(K) time after role-specific sorted list is maintained

void displayTopKPlayers(void)
{
    int teamId, k;

    printf("\nEnter Team ID: ");
    scanf("%d", &teamId);

    Team *team = searchTeamById(teamId);

    if (team == NULL)
    {
        printf("Invalid Team ID.\n");
        return;
    }

    Role role = inputRole();

    if (role == 0)
    {
        printf("Invalid role.\n");
        return;
    }

    printf("Enter number of players (K): ");
    scanf("%d", &k);

    if (k <= 0)
    {
        printf("K must be greater than zero.\n");
        return;
    }

    int index = getRoleIndex(role);
    ODIPlayer *current = team->roleHeads[index];

    printf("\nTop %d %s(s) of Team %s:\n",
           k, getRoleName(role), team->name);

    printPlayerHeader();

    int displayed = 0;

    while (current != NULL && displayed < k)
    {
        printPlayerRow(current);

        current = current->nextRole;
        displayed++;
    }

    if (displayed == 0)
        printf("No players found for this role.\n");

    printf("---------------------------------------------------------------------------------------------\n");
}


//Heap functions for K-way merge

int heapComesBefore(HeapNode a, HeapNode b)
{
    return playerComesBefore(a.player, b.player);
}

void heapPush(MinHeap *heap, HeapNode node)
{
    int i = heap->size++;

    while (i > 0)
    {
        int parent = (i - 1) / 2;

        if (heapComesBefore(heap->nodes[parent], node))
            break;

        heap->nodes[i] = heap->nodes[parent];
        i = parent;
    }

    heap->nodes[i] = node;
}

HeapNode heapPop(MinHeap *heap)
{
    HeapNode result = heap->nodes[0];
    HeapNode last = heap->nodes[--heap->size];

    if (heap->size == 0)
        return result;

    int i = 0;

    while (1)
    {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int smallest = i;

        if (left < heap->size &&
            heapComesBefore(heap->nodes[left], last))
        {
            smallest = left;
        }

        if (right < heap->size &&
            heapComesBefore(heap->nodes[right], 
                           heap->nodes[smallest]))
        {
            smallest = right;
        }

        if (smallest == i)
            break;

        heap->nodes[i] = heap->nodes[smallest];
        i = smallest;
    }

    heap->nodes[i] = last;

    return result;
}


//5. Display All Players Across All Teams by Role
//K-way merge of 10 sorted role lists
//O(N log t)

void displayAllPlayersByRole(void)
{
    Role role = inputRole();

    if (role == 0)
    {
        printf("Invalid role.\n");
        return;
    }

    int index = getRoleIndex(role);

    MinHeap heap;
    heap.size = 0;

    /* Add first player from every team's sorted role list */
    for (int i = 0; i < totalTeams; i++)
    {
        ODIPlayer *head = teamList[i].roleHeads[index];

        if (head != NULL)
        {
            HeapNode node;
            node.player = head;
            node.teamIndex = i;

            heapPush(&heap, node);
        }
    }

    printf("\nAll %s(s) Across Teams Sorted by Performance Index:\n",
           getRoleName(role));

    printf("--------------------------------------------------------------------------------------------------------------\n");
    printf("%-6s %-23s %-15s %-13s %-8s %-8s %-8s %-7s %-7s %-10s\n",
           "ID", "Name", "Team", "Role", "Runs", "Avg", "SR", "Wkts", "Eco", "Perf.Index");
    printf("--------------------------------------------------------------------------------------------------------------\n");

    int totalDisplayed = 0;

    while (heap.size > 0)
    {
        HeapNode top = heapPop(&heap);

        printPlayerRowWithTeam(top.player);
        totalDisplayed++;

        /* Advance in the sorted list of the team that was popped */
        ODIPlayer *next = top.player->nextRole;

        if (next != NULL)
        {
            HeapNode node;
            node.player = next;
            node.teamIndex = top.teamIndex;

            heapPush(&heap, node);
        }
    }

    if (totalDisplayed == 0)
        printf("No players found for this role.\n");

    printf("--------------------------------------------------------------------------------------------------------------\n");
    printf("Total Players Displayed: %d\n", totalDisplayed);
}

//Menu

void displayMenu(void)
{
    printf("\n");
    printf("==============================================================================\n");
    printf("              ICC ODI PLAYER PERFORMANCE ANALYZER\n");
    printf("==============================================================================\n");
    printf("1. Add Player to Team\n");
    printf("2. Display Players of a Specific Team\n");
    printf("3. Display Teams by Average Batting Strike Rate\n");
    printf("4. Display Top K Players of a Specific Team by Role\n");
    printf("5. Display All Players of Specific Role Across All Teams\n");
    printf("6. Exit\n");
    printf("==============================================================================\n");
    printf("Enter your choice: ");
}

//Free dynamically allocated players


void freeAllPlayers(void)
{
    for (int i = 0; i < totalTeams; i++)
    {
        ODIPlayer *current = teamList[i].playerHead;

        while (current != NULL)
        {
            ODIPlayer *next = current->nextTeam;
            free(current);
            current = next;
        }

        teamList[i].playerHead = NULL;
    }
}


int main(void)
{
    initializeTeams();

    int choice;

    do
    {
        displayMenu();
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                addPlayerToTeam();
                break;

            case 2:
                displayPlayersOfTeam();
                break;

            case 3:
                displayTeamsByStrikeRate();
                break;

            case 4:
                displayTopKPlayers();
                break;

            case 5:
                displayAllPlayersByRole();
                break;

            case 6:
                printf("\nExiting ICC ODI Player Performance Analyzer.\n");
                freeAllPlayers();
                break;

            default:
                printf("Invalid choice. Please try again.\n");
        }

    } while (choice != 6);

    return 0;
}