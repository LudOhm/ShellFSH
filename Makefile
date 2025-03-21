.PHONY: all clean

# Variables
TARGET = fsh
SRC = src/fsh.c src/commandeInterne.c src/for.c src/execute.c src/commandeStructuree.c src/redirections.c src/pipelines.c
OBJ = $(patsubst src/%.c,.compile/%.o,$(SRC))   # Convertit chaque .c en .o 

all: $(TARGET)


$(TARGET): $(OBJ)
	gcc -g -Wall -o $(TARGET) $(OBJ) -lreadline

# pour compiler chaque fichier .c en .o (pour meilleurs orga dans un autre dossier )
.compile/%.o: src/%.c
	mkdir -p .compile
	gcc -g -Wall -c $< -o $@

# Pour nettoyer les fichiers générés
clean:
	rm -rf .compile $(TARGET)


# pour lancer le shell (il faut faire make run pour lancé le shell)
run: all
	./$(TARGET) || true

# Lancer le script de test
test: $(TARGET)
	./test.sh

testjal2: $(TARGET)
	TEST_FOLDER_FILTER=jalon-2-extra ./test.sh

testjalfinal: $(TARGET)
	TEST_FOLDER_FILTER=rendu-final-A-pipelines ./test.sh

testfinB : $(TARGET)
	TEST_FOLDER_FILTER=rendu-final-B-signaux ./test.sh

testfinC : $(TARGET)
	TEST_FOLDER_FILTER=rendu-final-C-boucles-paralleles ./test.sh