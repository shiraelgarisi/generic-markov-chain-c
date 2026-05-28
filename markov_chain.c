#include "markov_chain.h"
/**
 * Get random number between 0 and max_number [0, max_number).
 * @param max_number
 * @return Random number
 */
int get_random_number(int max_number)
{
    return rand() % max_number;
}

Node* get_node_from_database(MarkovChain *markov_chain, void *data_ptr)
{
    if (markov_chain == NULL || markov_chain -> database == NULL || data_ptr == NULL)
    {
        return NULL;
    }
    Node *current = markov_chain -> database -> first;

    while (current != NULL)
    {
        const MarkovNode *markov_node = current -> data;

        if (markov_chain->comp_func(markov_node->data, data_ptr) == 0)
        {
            return current;
        }
        current = current -> next;
    }
    return NULL;
}

Node* add_to_database(MarkovChain *markov_chain, void *data_ptr)
{
    Node* prv_node = get_node_from_database(markov_chain, data_ptr);
    if (prv_node != NULL)
    {
        return prv_node;
    }
    MarkovNode *new_markov_node = malloc(sizeof(MarkovNode));
    if (!new_markov_node)
    {
        printf(ALLOCATION_ERROR_MESSAGE);
        return NULL;
    }
    void *word = markov_chain -> copy_func(data_ptr);
    if (word == NULL)
    {
        free(new_markov_node);
        printf(ALLOCATION_ERROR_MESSAGE);
        return NULL;
    }
    new_markov_node -> data = word;
    new_markov_node -> frequency_list = NULL;
    new_markov_node->frequency_list_size = 0;
    if (add(markov_chain -> database, new_markov_node) == 1)
    {
        markov_chain->free_data(word);
        free(new_markov_node);
        printf(ALLOCATION_ERROR_MESSAGE);
        return NULL;
    }
    Node* result = markov_chain -> database -> last;
    return result;
}

int add_node_to_frequency_list(MarkovNode *first_node, MarkovNode *second_node)
{

    for (int i = 0 ; i < first_node -> frequency_list_size ; i++)
    {
        MarkovNodeFrequency *frequency_list = &first_node -> frequency_list[i];
        if (frequency_list -> markov_node == second_node)
        {
            frequency_list -> frequency ++;
            return 0;
        }
    }
    if (first_node -> frequency_list_size == 0)
    {
        first_node -> frequency_list = malloc(sizeof(MarkovNodeFrequency));
        if (!first_node -> frequency_list)
        {
            printf(ALLOCATION_ERROR_MESSAGE);
            return 1;
        }
        first_node -> frequency_list[0].markov_node = second_node;
        first_node -> frequency_list[0].frequency = 1;
        first_node -> frequency_list_size = 1;
        return 0;
    }

    int old_size = first_node -> frequency_list_size;
    int new_size = old_size + 1;
    MarkovNodeFrequency *tmp = realloc(first_node -> frequency_list, new_size * sizeof(MarkovNodeFrequency));
    if (!tmp)
    {
        printf(ALLOCATION_ERROR_MESSAGE);
        return 1;
    }
    first_node -> frequency_list = tmp;
    first_node -> frequency_list[old_size].markov_node = second_node;
    first_node -> frequency_list[old_size].frequency = 1;
    first_node -> frequency_list_size = new_size;
    return 0;
}

void free_database(MarkovChain ** ptr_chain)
{
    LinkedList *curr_list = (*ptr_chain)->database;
    Node *curr = curr_list -> first;
    while (curr != NULL)
    {
        Node *next = curr -> next;
        MarkovNode *markov_node = curr -> data;
        if ((*ptr_chain)->free_data != NULL)
        {
            (*ptr_chain)->free_data(markov_node->data);
        }
        free(markov_node -> frequency_list);
        free(markov_node);
        free(curr);
        curr = next;
    }
    free(curr_list);
    free(*ptr_chain);
    *ptr_chain = NULL;
}

MarkovNode* get_first_random_node(MarkovChain *markov_chain)
{
    if (markov_chain == NULL || markov_chain->database == NULL)
    {
        return NULL;
    }
    const int max_num = markov_chain->database->size;
    const Node *rand_word = NULL;
    const MarkovNode *markov_node = NULL;
    do
    {
        int rand_num = get_random_number(max_num);
        rand_word = markov_chain->database->first;
        for (int i = 0 ; i < rand_num ; i++ )
        {
            rand_word = rand_word -> next;
        }
        markov_node = rand_word -> data;
    }
    while (markov_chain->is_last(markov_node->data));
 return rand_word -> data;
}

MarkovNode* get_next_random_node(MarkovNode *cur_markov_node)
{
    int max_num = 0;
    for (int i = 0; i < cur_markov_node -> frequency_list_size; i++)
    {
        max_num += cur_markov_node -> frequency_list[i].frequency;
    }
    int rand_num = get_random_number(max_num);
    int cumulative = 0;
    for (int i = 0; i < cur_markov_node -> frequency_list_size; i++)
    {
        cumulative += cur_markov_node -> frequency_list[i].frequency;
        if (rand_num < cumulative)
        {
            return cur_markov_node -> frequency_list[i].markov_node;
        }
    }
    return cur_markov_node -> frequency_list[cur_markov_node -> frequency_list_size - 1].markov_node;
}

void generate_random_sequence(MarkovChain *markov_chain, MarkovNode *first_node, int max_length)
{
    if (markov_chain == NULL || first_node == NULL || max_length <= 0)
    {
        return;
    }
    int words_counter = 0;
    MarkovNode *curr_node = first_node;
    while ( words_counter < max_length)
    {
        markov_chain -> print_func(curr_node -> data);
        words_counter ++;
        if (markov_chain -> is_last(curr_node -> data) || words_counter == max_length)
        {
            break;
        }
        printf(" ");
        MarkovNode *next = get_next_random_node(curr_node);
        if (!next)
        {
            break;
        }
        curr_node = next;
    }
    printf("\n");
}

