#include <string.h> // For strlen(), strcmp(), strcpy()
#include "markov_chain.h"

#define MAX(X, Y) (((X) < (Y)) ? (Y) : (X))

#define EMPTY -1
#define BOARD_SIZE 100
#define MAX_GENERATION_LENGTH 60

#define DICE_MAX 6
#define NUM_OF_TRANSITIONS 20

#define NUM_ARGS_ERROR "Usage: invalid number of arguments"

/**
 * represents the transitions by ladders and snakes in the game
 * each tuple (x,y) represents a ladder from x to if x<y or a snake otherwise
 */
const int transitions[][2] = {
    {13, 4},
    {85, 17},
    {95, 67},
    {97, 58},
    {66, 89},
    {87, 31},
    {57, 83},
    {91, 25},
    {28, 50},
    {35, 11},
    {8, 30},
    {41, 62},
    {81, 43},
    {69, 32},
    {20, 39},
    {33, 70},
    {79, 99},
    {23, 76},
    {15, 47},
    {61, 14}
};

/**
 * struct represents a Cell in the game board
 */
typedef struct Cell {
    int number; // Cell number 1-100
    int ladder_to; // cell which ladder leads to, if there is one
    int snake_to; // cell which snake leads to, if there is one
    //both ladder_to and snake_to should be -1 if the Cell doesn't have them
} Cell;

/**
 * allocates memory for cells on the board and initalizes them
 * @param cells Array of pointer to Cell, represents game board
 * @return EXIT_SUCCESS if successful, else EXIT_FAILURE
 */
int create_board(Cell *cells[BOARD_SIZE])
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        cells[i] = malloc(sizeof(Cell));
        if (cells[i] == NULL)
        {
            for (int j = 0; j < i; j++)
            {
                free(cells[j]);
            }
            printf(ALLOCATION_ERROR_MESSAGE);
            return EXIT_FAILURE;
        }
        *(cells[i]) = (Cell){i + 1, EMPTY, EMPTY};
    }

    for (int i = 0; i < NUM_OF_TRANSITIONS; i++)
    {
        int from = transitions[i][0];
        int to = transitions[i][1];
        if (from < to)
        {
            cells[from - 1]->ladder_to = to;
        } else
        {
            cells[from - 1]->snake_to = to;
        }
    }
    return EXIT_SUCCESS;
}

int add_cells_to_database(MarkovChain *markov_chain, Cell *cells[BOARD_SIZE])
{
    for (size_t i = 0; i < BOARD_SIZE; i++)
    {
        Node *tmp = add_to_database(markov_chain, cells[i]);
        if (tmp == NULL)
        {
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

int set_nodes_frequencies(MarkovChain *markov_chain, Cell *cells[BOARD_SIZE])
{
    MarkovNode *from_node = NULL, *to_node = NULL;
    size_t index_to;

    for (size_t i = 0; i < BOARD_SIZE; i++)
    {
        from_node = get_node_from_database(markov_chain, cells[i])->data;
        if (cells[i]->snake_to != EMPTY || cells[i]->ladder_to != EMPTY)
        {
            index_to = MAX(cells[i]->snake_to, cells[i]->ladder_to) - 1;
            to_node = get_node_from_database(markov_chain,
                                             cells[index_to])->data;
            int res = add_node_to_frequency_list(from_node, to_node);
            if (res == EXIT_FAILURE)
            {
                return EXIT_FAILURE;
            }
        }
        else
        {
            for (int j = 1; j <= DICE_MAX; j++)
            {
                index_to = ((Cell *) (from_node->data))->number + j - 1;
                if (index_to >= BOARD_SIZE)
                {
                    break;
                }
                to_node = get_node_from_database(markov_chain,
                                                 cells[index_to])->data;
                int res = add_node_to_frequency_list(from_node, to_node);
                if (res == EXIT_FAILURE)
                {
                    return EXIT_FAILURE;
                }
            }
        }
    }
    return EXIT_SUCCESS;
}

/**
 * fills database
 * @param markov_chain
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int fill_database_snakes(MarkovChain *markov_chain)
{
    Cell *cells[BOARD_SIZE];
    if (create_board(cells) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }
    if (add_cells_to_database(markov_chain, cells) == EXIT_FAILURE)
    {
        for (size_t i = 0; i < BOARD_SIZE; i++)
        {
            free(cells[i]);
        }
        return EXIT_FAILURE;
    }

    if(set_nodes_frequencies(markov_chain, cells) == EXIT_FAILURE)
    {
        for (size_t i = 0; i < BOARD_SIZE; i++)
        {
            free(cells[i]);
        }
        return EXIT_FAILURE;
    }

    // free temp arr
    for (size_t i = 0; i < BOARD_SIZE; i++)
    {
        free(cells[i]);
    }
    return EXIT_SUCCESS;
}

static void print_cell(void *data)
{
    Cell *cell = data;
    if (cell -> number == 100)
    {
        printf("[%d]", cell -> number);
        return;
    }
    if (cell -> ladder_to != EMPTY)
    {
        printf("[%d] -ladder to->", cell -> number);
        return;
    }
    if (cell -> snake_to != EMPTY)
    {
        printf("[%d] -snake to->", cell -> number);
        return;
    }
    printf("[%d] ->", cell -> number);
}

static int comp_cells(void *data1, void *data2)
{
    Cell *cell1 = data1;
    Cell *cell2 = data2;
    if (cell1->number > cell2->number)
    {
        return 1;
    }
    if (cell1->number < cell2->number)
    {
        return -1;
    }
    return 0;
}

static void* copy_cell(void *data)
{
    const Cell *cell = data;
    Cell *new_cell = malloc(sizeof(*cell));
    if (!new_cell)
    {
        return NULL;
    }
    new_cell -> number = cell -> number;
    new_cell -> ladder_to = cell -> ladder_to;
    new_cell -> snake_to = cell -> snake_to;
    return new_cell;
}

static void free_cell(void *data)
{
    free(data);
}

static bool is_last_cell(void *data)
{
    Cell *cell = data;
    if (cell -> number == 100)
    {
        return true;
    }
    return false;
}

static int argv_checker_snake (const int argc)
{
    if (argc != 3)
    {
        printf(NUM_ARGS_ERROR);
        return 1;
    }
    return 0;
}

static MarkovChain *create_chain_snake()
{
    MarkovChain *markov_chain = malloc(sizeof(MarkovChain));
    if (!markov_chain)
    {
        printf(ALLOCATION_ERROR_MESSAGE);
        return NULL;
    }
    LinkedList *linked_list = malloc(sizeof(LinkedList));
    if (!linked_list)
    {
        printf(ALLOCATION_ERROR_MESSAGE);
        free(markov_chain);
        return NULL;
    }
    linked_list -> first = NULL;
    linked_list -> last = NULL;
    linked_list -> size = 0;
    markov_chain -> database = linked_list;
    markov_chain->print_func = print_cell;
    markov_chain->comp_func = comp_cells;
    markov_chain->free_data = free_cell;
    markov_chain->copy_func = copy_cell;
    markov_chain->is_last = is_last_cell;
    return markov_chain;
}

/**
 * @param argc num of arguments
 * @param argv 1) Seed
 *             2) Number of sentences to generate
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int main(int argc, char *argv[])
{
    if (argv_checker_snake(argc))
    {
        return EXIT_FAILURE;
    }
    const unsigned int seed = strtol(argv[1], NULL, 10);
    const long int paths_num = strtol(argv[2], NULL, 10);
    srand(seed);
    MarkovChain *markov_chain = create_chain_snake();
    if (fill_database_snakes(markov_chain) == EXIT_FAILURE)
    {
        free_database(&markov_chain);
        return EXIT_FAILURE;
    }
    for (int i = 0 ; i < paths_num ; i++)
    {
        printf("Random Walk %d: ", i+1);
        Cell start;
        start.number = 1;
        start.ladder_to = EMPTY;
        start.snake_to = EMPTY;
        Node *start_node = get_node_from_database(markov_chain, &start);
        if (!start_node)
        {
            printf(ALLOCATION_ERROR_MESSAGE);
            free_database(&markov_chain);
            return EXIT_FAILURE;
        }
        generate_random_sequence(markov_chain, start_node->data, MAX_GENERATION_LENGTH);
        printf("\n");
    }
    free_database(&markov_chain);
    return EXIT_SUCCESS;
}
