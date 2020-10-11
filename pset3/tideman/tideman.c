#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
// Consider candidates [ A, B ].
// There are 7 voters prefer A over B and 2 prefer B over A.
// preferences = [ [ 0, 7 ], [2, 0] ]
int preferences[MAX][MAX] = {{0}};

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
} pair;

// Array of candidates
string candidates[MAX];
// Consider candidates: A, B, C.
// Possible pairs are: A: [ [A, B], [A, C] ], B: [ [B, C] ]
// Thus, pairs = (3 - 1) + (3 - 2)
// Therefore, given n candidates, we have (n - 1) + (n - 2) + (n - 3) + ... + 1 pairs
// (n - 1) + (n - 2) + (n - 3) + ... + 1 = n(n - 1) / 2
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[j] is voter's jth preference
        // Consider candidates: A, B, C.
        // Voter votes: Rank 1: B, Rank 2: C, Rank 3: A.
        // ranks = [1, 2, 0].
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    // Check if name is in candidates array
    // For i from 0 to candidates count - 1
    for (int i = 0; i < candidate_count; i++)
    {
        // If found, set ranks[rank] to i and return true
        if (strcmp(name, candidates[i]) == 0)
        {
            ranks[rank] = i;
            return true;
        }
    }
    // If name is not found in candidates array, return false
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    // Consider a voter voted: Rank 1: B, Rank 2: C, Rank 3: A.
    // ranks = [ 1, 2, 0 ].
    // Preference of C will be [1, 0, 0].
    // We can loop through ranks array,
    // C = ranks[1],
    // Loop through ranks array from current rank + 1 to end
    // to find all the candidates whose ranks are below current candidate C, and add 1 to
    // each candidate index in preference of current candidate C.
    for (int i = 0; i < candidate_count; i++)
    {
        int curr_candidate = ranks[i];
        for (int j = i + 1; j < candidate_count; j++)
        {
            preferences[curr_candidate][ranks[j]]++;
        }
    }
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{

    // Loop though preference array
    // For i from 0 to candidates count - 1
    for (int i = 0; i < candidate_count; i++)
    {
        // For j from 0 to candidates count - 1
        for (int j = 0; j < candidate_count; j++)
        {
            // Consider candidate A: 0, 2, 1, candidate B: 1, 0, 2,
            // thus preference array = [ [0, 2, 1], [1, 0, 2] ]
            // i = 0; j = 0;
            // preferences[i][j] = preferences[0][0] = 0, preferences[j][i] = preferences[0][0] = 0.
            // i = 0; j = 1;
            // preferences[i][j] = preferences[0][1] = 2, there is 2 voters prefer candidate A over candidate B,
            // preferences[j][i] = preferences[1][0] = 1, there is 1 voters prefer candidate B over candidate A.
            // 2 > 1, thus { winner: candidate A = i = 0, loser: candidate B = j = 1 }.

            // If preferences[i][j] > preferences[j][i],
            if (preferences[i][j] > preferences[j][i])
            {
                // set pairs[pair_count]'s winner to i and pair[pair_count]'s loser to j.
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                // pair_count++
                pair_count++;
            }
        }
    }
}

// Merge part of merge sort
void merge(int start, int mid, int end)
{
    // Set size of low half array to mid.
    int low_half_size = mid - start + 1;
    // Initialize low half array.
    pair low_half[low_half_size];
    // Set size of high half array to end - mid + 1.
    int high_half_size = end - mid;
    // Initialize high half array.
    pair high_half[high_half_size];

    // Set k to start
    int k = start;
    // Set low half index i to 0.
    int i = 0;
    // Set high half index j to 0.
    int j = 0;

    // Copy pairs into low half and high half array respectively.
    while (k <= mid)
    {
        low_half[i].winner = pairs[k].winner;
        low_half[i].loser = pairs[k].loser;
        i++;
        k++;
    }
    while (k <= end)
    {
        high_half[j].winner = pairs[k].winner;
        high_half[j].loser = pairs[k].loser;
        j++;
        k++;
    }

    // Reset k, i, j.
    k = start;
    i = 0;
    j = 0;

    // While i < low half array size and j < high half array size
    while (i < low_half_size && j < high_half_size)
    {
        // Calculate low_half ith's margin
        int l_winner = low_half[i].winner;
        int l_loser = low_half[i].loser;
        int l_margin = preferences[l_winner][l_loser] - preferences[l_loser][l_winner];

        // Calculate high_half jth's margin
        int h_winner = high_half[j].winner;
        int h_loser = high_half[j].loser;
        int h_margin = preferences[h_winner][h_loser] - preferences[h_loser][h_winner];

        // If low half's ith's margin is greater then high half's jth's margin,
        if (l_margin > h_margin)
        {
            // Set pairs[k] to low half's ith item.
            pairs[k].winner = low_half[i].winner;
            pairs[k].loser = low_half[i].loser;
            // Increase i by 1.
            i++;
        }
        // Else
        else
        {
            // Set pairs[k] to high half's jth item.
            pairs[k].winner = high_half[j].winner;
            pairs[k].loser = high_half[j].loser;
            // Increase j by 1.
            j++;
        }
        // Increase k by 1.
        k++;
    }

    // While i < low half array size
    while (i < low_half_size)
    {
        // Set pairs[k] to low half's ith item.
        pairs[k].winner = low_half[i].winner;
        pairs[k].loser = low_half[i].loser;
        // Increase i by 1.
        i++;
        // Increase k by 1.
        k++;
    }

    // While j < high half array size
    while (j < high_half_size)
    {
        // Set pairs[k] to high half's jth item.
        pairs[k].winner = high_half[j].winner;
        pairs[k].loser = high_half[j].loser;
        // Increase j by 1.
        j++;
        // Increase k by 1.
        k++;
    }
}

void merge_sort(int start, int end)
{
    // Base case: if there is only one element in pairs array, return
    if (start < end)
    {
        // Set mid to (start + end) / 2
        int mid = (start + end) / 2;
        // Divide pairs into two subarrays [start...mid] and [mid + 1...end]
        merge_sort(start, mid);
        merge_sort(mid + 1, end);
        // merge the two subarrays.
        merge(start, mid, end);
    }
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs()
{
    merge_sort(0, pair_count - 1);
}

// BFS

// BFS's queue
int queue[MAX];
// Set queue_index to track the length of the queue.
int queue_index = 0;

// Push a vertex into the queue.
void enqueue(int vertex)
{
    queue[queue_index] = vertex;
    queue_index++;
}

// Remove the first vertex from the queue.
int dequeue()
{
    int first_vertex = queue[0];
    // Shift all other vertex in the queue array one position
    // to the left
    for (int i = 1; i < queue_index; i++)
    {
        queue[i - 1] = queue[i];
    }
    // Decrease queue_index by 1, cause first vertex is removed.
    queue_index--;

    return first_vertex;
}

// Check if queue is empty
bool queue_is_empty()
{
    return queue_index == 0;
}

// Do breath first search which receive two parameters:
// source vertex, target vertex, to find out if there is a
// chain from the source vertex to the target vertex.
bool do_bfs(source, target)
{
    // Enqueue source vertex.
    enqueue(source);

    // Traverse the graph
    // While queue array is not empty
    while (!queue_is_empty())
    {
        // dequeue a vertex and assign it to u
        int u = dequeue();

        // Loop over locked uth's array to find out its neighbor
        for (int i = 0; i < candidate_count; i++)
        {
            if (locked[u][i])
            {
                // If its neighbour is equal to the target vertex, then a
                // chain is formed, return true.
                if (i == target)
                {
                    return true;
                }
                // Otherwise enqueue neighbours
                else
                {
                    enqueue(i);
                }
            }
        }
    }
    // return false
    return false;
}

// Check for circle
bool is_circle(int i, int winner, int loser)
{
    if (i > 0)
    {
        // Loop over previous pairs to find out if current pair's loser
        // has been previous pair's winner.
        // If so, then it might create a circle, thus traverse the graph
        // starting from the previous pair's winner to find out whether
        // it forms a chain from the previous pair's winner
        // to the current pair's winner.
        for (int j = 0; j < i; j++)
        {
            if (pairs[j].winner == loser)
            {
                return do_bfs(pairs[j].winner, winner);
            }
        }
    }
    return false;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    // Consider pairs array after sorting:
    // [ {winner: A, loser: margin: 5 }, {winner: C, loser: A, margin: 3}, {w: B, l: C, m: 1} ]
    // locked array: Adjacency matrix
    /*        j
              0   1   2
       i   0  F   T   F

           1  F   F   T

           2  T   F   F
    */

    // Loop over pairs to create adjacency matrix
    for (int i = 0; i < pair_count; i++)
    {

        int locked_i = pairs[i].winner;
        int locked_j = pairs[i].loser;
        // If there is not a cycle,
        // Set locked[pair ith winner][pair ith loser] to true.
        if (is_circle(i, locked_i, locked_j))
        {
            locked[locked_i][locked_j] = false;
            continue;
        }
        locked[locked_i][locked_j] = true;
    }
}

// Print the winner of the election
void print_winner(void)
{
    // intialize an array "source" with values true
    // to track if a candidate does not have a predecessor.
    bool source[MAX];
    for (int i = 0; i < candidate_count; i++)
    {
        source[i] = true;
    }

    // Loop over locked array's row
    for (int i = 0; i < candidate_count; i++)
    {
        // Loop over locked array's column
        for (int j = 0; j < candidate_count; j++)
        {
            // if locked[i][j] == true, then the candidate has a predecessor,
            // set source[j] to false
            if (locked[i][j])
            {
                source[j] = false;
            }
        }
    }

    // Loop over source array
    for (int i = 0; i < candidate_count; i++)
    {
        // If source[i] == true,
        // print out current candidate's name.
        if (source[i] == 1)
        {
            printf("%s\n", candidates[i]);
        }
    }
}
