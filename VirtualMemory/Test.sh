gcc -o main.o main.c

PROGRAM="main.o"

TEST_FILE="./test_simple.txt"
echo "Running $PROGRAM with $TEST_FILE"
./main.o < $TEST_FILE

TEST_FILE="./test_commands.txt"
echo -e "\n\n"
echo "Running $PROGRAM with $TEST_FILE"
./main.o < $TEST_FILE