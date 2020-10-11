#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// Candidates have name and vote count
typedef struct
{
    string name;
    int votes;
} candidate;

// Array of candidates
candidate candidates[MAX];

// Number of candidates
int candidate_count;

// Function prototypes
bool vote(string name);
void print_winner(void);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: plurality [candidate ...]\n");
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
        candidates[i].name = argv[i + 1];
        candidates[i].votes = 0;
    }

    int voter_count = get_int("Number of voters: ");

    // Loop over all voters
    for (int i = 0; i < voter_count; i++)
    {
        string name = get_string("Vote: ");

        // Check for invalid vote
        if (!vote(name))
        {
            printf("Invalid vote.\n");
        }
    }

    // Display winner of election
    print_winner();
}

// Update vote totals given a new vote
bool vote(string name)
{
    // For i from 0 to candidate_count - 1
    for (int i = 0; i < candidate_count; i++)
    {
        // If name equals candidates[i]'s name
        if (strcmp(candidates[i].name, name) == 0)
        {
            // increase candidtates[i]'s votes by 1
            candidates[i].votes++;
            // return true
            return true;
        }
    }
    // If name is not found in candidates, return false
    return false;
}

// Print the winner (or winners) of the election
void print_winner(void)
{
    // Set max_votes to the first candidate's votes
    int max_votes = candidates[0].votes;
    // For i from 1 to candidate_count - 1
    for (int i = 1; i < candidate_count; i++)
    {
        // If candidates[i]'s votes is greater than max_votes,
        // set max_votes to its value
        if (candidates[i].votes > max_votes)
        {
            max_votes = candidates[i].votes;
        }
    }

    // For i from 1 to candidate_count - 1
    for (int i = 0; i < candidate_count; i++)
    {
        // If candidates[i]'s votes is equal to max_votes,
        // print out candidates[i]'s name and a new line.
        if (candidates[i].votes == max_votes)
        {
            printf("%s\n", candidates[i].name);
        }
    }
    return;
}
