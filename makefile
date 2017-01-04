	
CC = gcc
CFLAGS = -Wall
LIBS = -lssl -lcrypto
TEST_DIR = ./tests/

all: wiki_runner

clean:
	rm *.txt
	rm *.o
	rm *.out

tests:	tree_tests list_tests queue_tests 
	chmod -R 777 $(TEST_DIR)

wiki_runner:	terms terms_dictionary list terms_queue wiki_parser wiki_socket wiki_crawler
	$(CC) $(CFLAGS) term.o list.o terms_queue.o  terms_dictionary.o wiki_parser.o wiki_socket.o $(LIBS) wiki_crawler.o -o wiki_racer.out
	
	
terms:	term.c
	$(CC) -c $(CFLAGS) term.c  
	
terms_queue:	terms terms_queue.c
	$(CC) -c $(CFLAGS) terms_queue.c  
	
list:   list.c
	$(CC) -c $(CFLAGS) list.c  
	
wiki_parser:	wiki_parser.c
	$(CC) -c $(CFLAGS) wiki_parser.c 
	
wiki_socket:	wiki_socket.c
	$(CC) -c $(CFLAGS)  wiki_socket.c $(LIBS)

wiki_crawler:	wiki_crawler.c
	$(CC) -c $(CFLAGS)  wiki_crawler.c 
	
terms_dictionary: terms_dictionary.c list
	$(CC) -c $(CFLAGS) terms_dictionary.c  
	
	
list_tests: list
	$(CC) -c $(CFLAGS) list_tests.c  
	$(CC) $(CFLAGS) list_tests.o list.o -o $(TEST_DIR)list_tests.out

queue_tests:	terms terms_queue list
	$(CC) -c $(CFLAGS) queue_tests.c
	$(CC) $(CFLAGS) term.o terms_queue.o queue_tests.o list.o  -o $(TEST_DIR)queue_tests.out

tree_tests:	terms_dictionary terms
	$(CC) -c $(CFLAGS) tree_tests.c  
	$(CC) $(CFLAGS) list.o term.o terms_dictionary.o tree_tests.o -o $(TEST_DIR)tree_tests.out
	