all: treasure_manager treasure_hub calculate_score

treasure_manager: treasure_manager.c treasure_manager_source.c treasure_manager.h
	gcc -Wall treasure_manager.c treasure_manager_source.c -o treasure_manager

treasure_hub: treasure_hub.c treasure_manager_source.c treasure_manager.h
	gcc -Wall treasure_hub.c treasure_manager_source.c -o treasure_hub

calculate_score: calculate_score.c 
	gcc -Wall calculate_score.c -o calculate_score

clean: 
	rm -f *.o treasure_manager treasure_hub
 
 
