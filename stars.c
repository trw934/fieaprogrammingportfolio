#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* This is an assignment for my Game Programming class on algorithms. We were
 * given a scenario where we had to use Breadth-First-Search to determine whether specific
 * goals were possible to reach in a game. The game is structured like a tic-tac-toe board,
 * where 1 of the squares is filled. Clicking on a filled square (a star) inverts the squares adjacent to it
 * (not diagonal to). The goal was to see if it was possible to get to a state where all squares
 * except the middle were filled, and how many moves it takes to reach that condition.
 * 
 * To accomplish this, I created a queue that would examine patterns in the order I came across them.
 * Each pattern also kept track of the distance it took to reach it, as well as what patterns were
 * used to get to them.
 */

// Structure for holding the star pattern
struct Stars {
    int star[9];
};
/* Structure that has a star pattern, a distance from
 * the beginning, and the patterns used to reach it.
 */
struct Pattern {
    struct Stars star;
    int distance;
    char pass[512];
};

struct Pattern end = {{1,1,1,1,0,1,1,1,1},0," ",};
struct Stars visit[512];
struct Pattern queue[512];
int back = -1;
int front = 0;
int count = 0;
int index = 0;

// Method that puts the pattern at the back of the queue.
void insert(struct Pattern data){
    queue[++back] = data;
    count++;
}

// Method that gets the next pattern from the front of the queue.
struct Pattern dequeue() {
    count--;
    return queue[front++];
}

// Method that compares two star patterns to see if they match.
int compStars(struct Pattern x, struct Stars y) {
    for (int i = 0; i < 9; i++) {
        if (x.star.star[i] != y.star[i]) {
            return 0;
        }
    }
    return 1;
}

// Method that checks if the star pattern has already been visited.
int visited(struct Pattern x) {
    for (int i = 0; i < index; i++) {
        if (compStars(x,visit[i])) {
            return 1;
        }
    }
    return 0;
}

// Method that produces a new star pattern based on what star was clicked.
struct Pattern starSwitch(struct Pattern y, int star) {
    struct Pattern x;
    for (int i = 0; i < 9; i++) {
        x.star.star[i] = y.star.star[i];
    }
    switch(star) {
        case 0:
            x.star.star[0] = 0;
            x.star.star[1] = x.star.star[1] ? 0 : 1;
            x.star.star[3] = x.star.star[3] ? 0 : 1;
            break;
        case 1:
            x.star.star[1] = 0;
            x.star.star[0] = x.star.star[0] ? 0 : 1;
            x.star.star[2] = x.star.star[2] ? 0 : 1;
            x.star.star[4] = x.star.star[4] ? 0 : 1;
            break;
        case 2:
            x.star.star[2] = 0;
            x.star.star[1] = x.star.star[1] ? 0 : 1;
            x.star.star[5] = x.star.star[5] ? 0 : 1;
            break;
        case 3:
            x.star.star[3] = 0;
            x.star.star[0] = x.star.star[0] ? 0 : 1;
            x.star.star[4] = x.star.star[4] ? 0 : 1;
            x.star.star[6] = x.star.star[6] ? 0 : 1;
            break;
        case 4:
            x.star.star[4] = 0;
            x.star.star[1] = x.star.star[1] ? 0 : 1;
            x.star.star[3] = x.star.star[3] ? 0 : 1;
            x.star.star[5] = x.star.star[5] ? 0 : 1;
            x.star.star[7] = x.star.star[7] ? 0 : 1;
            break;
        case 5:
            x.star.star[5] = 0;
            x.star.star[2] = x.star.star[2] ? 0 : 1;
            x.star.star[4] = x.star.star[4] ? 0 : 1;
            x.star.star[8] = x.star.star[8] ? 0 : 1;
            break;
        case 6:
            x.star.star[6] = 0;
            x.star.star[3] = x.star.star[3] ? 0 : 1;
            x.star.star[7] = x.star.star[7] ? 0 : 1;
            break;
        case 7:
            x.star.star[7] = 0;
            x.star.star[4] = x.star.star[4] ? 0 : 1;
            x.star.star[6] = x.star.star[6] ? 0 : 1;
            x.star.star[8] = x.star.star[8] ? 0 : 1;
            break;
        case 8:
            x.star.star[8] = 0;
            x.star.star[5] = x.star.star[5] ? 0 : 1;
            x.star.star[7] = x.star.star[7] ? 0 : 1;
            break;
        default:
        return y;
    }
    return x;
}

/* BFS implementation that determines optimal path to end condition and
 * prints it to console.
 */
int main() {
    struct Stars star = {{0,0,0,0,1,0,0,0,0}};
    struct Pattern start = {{0,0,0,0,1,0,0,0,0},0,""};
    visit[index] = star;
    index++;
    insert(start);
    while (count != 0) {
        struct Pattern x = dequeue();
        for (int i = 0; i < 9; i++) {
            if (x.star.star[i]) {
                struct Pattern y = starSwitch(x,i);
                y.distance = x.distance + 1;
                char c[2]; 
                c[0]= i + 49;
                c[1] = '\0';
                strcpy(y.pass, x.pass);
                strcat(y.pass, c);
                if (compStars(y,end.star)) {
                    printf("Path: %s, Distance: %d\n", y.pass, y.distance);
                    return 0;
                } else if (!(visited(y))) {
                    visit[index] = y.star;
                    index++;
                    insert(y);
                }
            }
        }
    }
    return 0;
}
