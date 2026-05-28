CC = gcc
CFLAGS = -Wall -Wextra -Wvla -std=c99 -g
all: tweets_generator snakes_and_ladders
tweets_generator: tweets_generator.o markov_chain.o linked_list.o
	$(CC) $(CFLAGS) tweets_generator.o markov_chain.o linked_list.o -o tweets_generator

snakes_and_ladders: snakes_and_ladders.o markov_chain.o linked_list.o
	$(CC) $(CFLAGS) snakes_and_ladders.o markov_chain.o linked_list.o -o snakes_and_ladders

tweets_generator.o: tweets_generator.c markov_chain.h linked_list.h
	$(CC) $(CFLAGS) -c tweets_generator.c

snakes_and_ladders.o: snakes_and_ladders.c markov_chain.h linked_list.h
	$(CC) $(CFLAGS) -c snakes_and_ladders.c

markov_chain.o: markov_chain.c markov_chain.h linked_list.h
	$(CC) $(CFLAGS) -c markov_chain.c

linked_list.o: linked_list.c linked_list.h
	$(CC) $(CFLAGS) -c linked_list.c

clean:
	rm -f *.o tweets_generator snakes_and_ladders