#include "markov_chain.h"
#include <string.h>
//Don't change the macros!
#define FILE_PATH_ERROR "Error: incorrect file path"
#define NUM_ARGS_ERROR "Usage: invalid number of arguments"

#define DELIMITERS " \n\t\r"
#define MAX_LENGTH 20

static void print_string (void *data)
{
    const char *new_data = data;
    printf("%s", new_data);
}

static int comp_strings(void *data1, void *data2)
{
    const char *new_data1 = data1;
    const char *new_data2 = data2;
    int result = strcmp(new_data1, new_data2);
    if (result > 0)
    {
        return 1;
    }
    if (result < 0)
    {
        return -1;
    }
    return 0;
}

static void free_string(void *data)
{
    free(data);
}

static void* copy_string(void *data)
{
    const char *new_data = data;
    const unsigned int  len = strlen(new_data);
    char *results = malloc(len + 1);
    if (!results)
    {
        return NULL;
    }
    strcpy(results, new_data);
    return results;
}

static bool is_last_string(void *data)
{
    char *str = (char*)data;
    int len = strlen(str);
    if (len > 0 && str[len - 1] == '.')
    {
        return true;
    }
    return false;
}

static int path_checker (const char *file_path)
{
    FILE *file_ptr = fopen(file_path, "r");
    if (!file_ptr) {
        printf(FILE_PATH_ERROR);
        return 1;
    }
    fclose(file_ptr);
    return 0;
}

static int words_counter (const char *file_path)
{
    FILE *file_ptr = fopen(file_path, "r");
    char line_buf[1000];
    int counter = 0;
    while (fgets(line_buf, sizeof(line_buf), file_ptr) != NULL)
    {
        char *word = strtok(line_buf, DELIMITERS);
        while (word != NULL)
        {
            counter ++;
            word = strtok(NULL,DELIMITERS);
        }
    }
    fclose(file_ptr);
    return counter;
}

static int num_to_read_check(const int num_of_words, const int total_num_of_words)
{
    if (num_of_words > total_num_of_words)
    {
        return total_num_of_words;
    }
    return num_of_words;
}

static int argv_checker (const int argc, char *argv[])
{
    if (argc != 5 && argc != 4)
    {
        printf(NUM_ARGS_ERROR);
        return 1;
    }
    if (path_checker(argv[3]))
    {
        return 1;
    }
    return 0;
}

static int fill_database(FILE *fp, const int words_to_read, MarkovChain *markov_chain)
{
    char line_buf[1000];
    int words_read = 0;
    while (fgets(line_buf, sizeof(line_buf), fp) != NULL && words_read < words_to_read)
    {
        MarkovNode *prv_markov = NULL;
        char *word = strtok(line_buf, DELIMITERS);
        while (word != NULL && words_read < words_to_read)
        {
            const Node *node = add_to_database(markov_chain, word);
            if (node == NULL)
            {
                return 1;
            }
            MarkovNode *curr_markov = node -> data;
            if (prv_markov != NULL)
            {
                if (!markov_chain->is_last(prv_markov->data))
                    if (add_node_to_frequency_list(prv_markov, curr_markov) == 1)
                    {
                        return 1;
                    }
            }
            prv_markov = curr_markov;
            words_read ++;
            word = strtok(NULL, DELIMITERS);
        }
    }
    return 0;
}

static MarkovChain *create_chain()
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
    markov_chain->print_func = print_string;
    markov_chain->comp_func = comp_strings;
    markov_chain->free_data = free_string;
    markov_chain->copy_func = copy_string;
    markov_chain->is_last = is_last_string;
    return markov_chain;
}

int main(const int argc, char *argv[])
{
    if (argv_checker(argc, argv))
    {
        return EXIT_FAILURE;
    }
    FILE *file = fopen(argv[3], "r");
    const int total_num_of_words = words_counter(argv[3]);
    int num_to_read = 0;
    if (argc == 4)
    {
        num_to_read = total_num_of_words;
    }
    else
    {
        num_to_read = (int)strtol(argv[4], NULL, 10);
    }
    const int words_to_read = num_to_read_check(num_to_read,total_num_of_words);
    MarkovChain *markov_chain = create_chain();
    if (!markov_chain)
    {
        fclose(file);
        return EXIT_FAILURE;
    }
    if (fill_database(file, words_to_read, markov_chain) == 1)
    {
        fclose(file);
        free_database(&markov_chain);
        return EXIT_FAILURE;
    }
    const int seed = (int)strtol(argv[1], NULL, 10);
    const int tweets_count = (int)strtol(argv[2], NULL, 10);
    srand(seed);
    for (int i = 1 ; i <= tweets_count ; i++)
    {
        printf("Tweet %d: ", i);
        MarkovNode *first_node = get_first_random_node(markov_chain);
        generate_random_sequence(markov_chain, first_node, MAX_LENGTH);
    }
    fclose(file);
    free_database(&markov_chain);
    return EXIT_SUCCESS;
}