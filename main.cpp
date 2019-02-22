#include <iostream>
#include <cstdlib>  // atoi
#include <stack>    // stack for floodfill
#include <map>      // map for floodfill
// #include <set>      // visited set for floodfill

#include "Maze.h"
#include "MazeDefinitions.h"
#include "PathFinder.h"
#include "Dir.h"

/**
 * Demo of a PathFinder implementation.
 *
 * Do not use a left/right wall following algorithm, as most
 * Micromouse mazes are designed for such algorithms to fail.
 */
// Coordinates
class Coord {
public:
    Coord(int xx, int yy) : m_x(xx), m_y(yy) {}
    Coord(const Coord &coordObj) {
        this->m_x = coordObj.m_x;
        this->m_y = coordObj.m_y;
    }
    int x() const { return m_x; }
    int y() const { return m_y; }
    bool operator< (const Coord &coordObj) const {
        if (coordObj.m_x < this->m_x) return true;
        if (coordObj.m_x == this->m_x && coordObj.m_y < this->m_y) return true;
        return false;
    }
    
private:
    int m_x;
    int m_y;
};
// Floodfill algorithm
class FloodFill : public PathFinder {
public:
    FloodFill(bool shouldPause = false) : pause(shouldPause) {
        shouldGoForward = false;
        visitedStart = false;
        int drawing[16][16] = {
            {14, 13, 12, 11, 10, 9, 8, 7, 7, 8, 9, 10, 11, 12, 13, 14},
            {13, 12, 11, 10,  9, 8, 7, 6, 6, 7, 8,  9, 10, 11, 12, 13},
            {12, 11, 10,  9,  8, 7, 6, 5, 5, 6, 7,  8,  9, 10, 11, 12},
            {11, 10,  9,  8,  7, 6, 5, 4, 4, 5, 6,  7,  8,  9, 10, 11},
            {10,  9,  8,  7,  6, 5, 4, 3, 3, 4, 5,  6,  7,  8,  9, 10},
            { 9,  8,  7,  6,  5, 4, 3, 2, 2, 3, 4,  5,  6,  7,  8,  9},
            { 8,  7,  6,  5,  4, 3, 2, 1, 1, 2, 3,  4,  5,  6,  7,  8},
            { 7,  6,  5,  4,  3, 2, 1, 0, 0, 1, 2,  3,  4,  5,  6,  7},
            { 7,  6,  5,  4,  3, 2, 1, 0, 0, 1, 2,  3,  4,  5,  6,  7},
            { 8,  7,  6,  5,  4, 3, 2, 1, 1, 2, 3,  4,  5,  6,  7,  8},
            { 9,  8,  7,  6,  5, 4, 3, 2, 2, 3, 4,  5,  6,  7,  8,  9},
            {10,  9,  8,  7,  6, 5, 4, 3, 3, 4, 5,  6,  7,  8,  9, 10},
            {11, 10,  9,  8,  7, 6, 5, 4, 4, 5, 6,  7,  8,  9, 10, 11},
            {12, 11, 10,  9,  8, 7, 6, 5, 5, 6, 7,  8,  9, 10, 11, 12},
            {13, 12, 11, 10,  9, 8, 7, 6, 6, 7, 8,  9, 10, 11, 12, 13},
            {14, 13, 12, 11, 10, 9, 8, 7, 7, 8, 9, 10, 11, 12, 13, 14}
        };
        
        for (int h = 0; h < 16; h++) {
            for (int g = 0; g < 16; g++) {
                manhattan[h][g] = drawing[h][g];
            }
        }
        
        for (int j = 0; j < 16; j++)
            for (int k = 0; k < 16; k++) {
                Coord temp(j, k);
                TopWall[temp] = false;
                RightWall[temp] = false;
            }
        
        for (int i = 0; i < 16; i++) {
            Coord virtualTop(i, 15);
            Coord virtualBot(i, -1);
            TopWall[virtualTop] = true;
            // std::cout << virtualTop.x() << ", " << virtualTop.y() << " has top wall" << std::endl;
            TopWall[virtualBot] = true;
            // std::cout << virtualBot.x() << ", " << virtualBot.y() << " has top wall" << std::endl;
            
            Coord virtualLeft(-1, i);
            Coord virtualRight(15, i);
            RightWall[virtualLeft] = true;
            // std::cout << virtualLeft.x() << ", " << virtualLeft.y() << " has right wall" << std::endl;
            RightWall[virtualRight] = true;
            // std::cout << virtualRight.x() << ", " << virtualRight.y() << " has right wall" << std::endl;
        }
        
        direction = NORTH;
    }
    
    MouseMovement nextMovement(unsigned x, unsigned y, const Maze &maze) {
        const bool frontWall = maze.wallInFront();
        const bool leftWall  = maze.wallOnLeft();
        const bool rightWall = maze.wallOnRight();
        
        // std::cout << "sf1" << std::endl;
        // std::cout << x << std::endl;
        // std::cout << y << std::endl;
        
        /*switch (direction) {
            case NORTH:
                std::cout << "NORTH" << std::endl;
                break;
            case SOUTH:
                std::cout << "SOUTH" << std::endl;
                break;
            case EAST:
                std::cout << "EAST" << std::endl;
                break;
            case WEST:
                std::cout << "WEST" << std::endl;
                break;
            default:
                break;
        }*/
        
        /*std::map<Coord, bool> wowWall;
        Coord c1(1, 1);
        wowWall[c1] = true;
        Coord c2(1, 1);
        if (wowWall[c2] == true) std::cout << "hi" << std::endl;*/
        
        // Pause at each cell if the user requests it.
        // It allows for better viewing on command line.
        if (pause) {
            std::cout << "Hit enter to continue..." << std::endl;
            std::cin.ignore(10000, '\n');
            std::cin.clear();
        }
        
        std::cout << maze.draw(5) << std::endl << std::endl;
        
        // If we somehow miraculously hit the center
        // of the maze, just terminate and celebrate!
        if(isAtCenter(x, y)) {
            std::cout << "Found center! Good enough for the demo, won't try to get back." << std::endl;
            return Finish;
        }
        
        if (frontWall) {
            setWall(x, y, direction);
        }
        if (leftWall)  setWall(x, y, counterClockwise(direction));
        if (rightWall) setWall(x, y, clockwise(direction));
        
        // if there is a neighbor with lower distance to the center, move to that neighbor
        if (!frontWall && isLowerDistance(x, y, direction))
            return MoveForward;
        if (!leftWall  && isLowerDistance(x, y, counterClockwise(direction))) {
            direction = counterClockwise(direction);
            return TurnCounterClockwise;
        }
        if (!rightWall && isLowerDistance(x, y, clockwise(direction))) {
            direction = clockwise(direction);
            return TurnClockwise;
        }
        
        // otherwise, must run floodfill
        std::stack<Coord> st;
        st.push(Coord(x, y));

        while (!st.empty()) {
            Coord cur = st.top();
            st.pop();
            
            // std::cout << "x: " << cur.x() << std::endl;
            // std::cout << "y: " << cur.y() << std::endl;
            
            if (getThisDistance(cur.x(), cur.y()) == 0) continue;
            
            int min_dist = 9999;
            if (!isWall(cur.x(), cur.y(), NORTH)) {
                if (getDistance(cur.x(), cur.y(), NORTH) < min_dist)  min_dist = getDistance(cur.x(), cur.y(), NORTH);
                // std::cout << "no wall north: " << min_dist << std::endl;
            }
            if (!isWall(cur.x(), cur.y(), SOUTH)) {
                if (getDistance(cur.x(), cur.y(), SOUTH) < min_dist) min_dist = getDistance(cur.x(), cur.y(), SOUTH);
                // std::cout << "no wall south: " << min_dist << std::endl;
            }
            if (!isWall(cur.x(), cur.y(), EAST)) {
                if (getDistance(cur.x(), cur.y(), EAST) < min_dist) min_dist = getDistance(cur.x(), cur.y(), EAST);
                // std::cout << "no wall east: " << min_dist << std::endl;
            }
            if (!isWall(cur.x(), cur.y(), WEST)) {
                if (getDistance(cur.x(), cur.y(), WEST) < min_dist) min_dist = getDistance(cur.x(), cur.y(), WEST);
                // std::cout << "no wall west: " << min_dist << std::endl;
            }
            
            // std::cout << "min dist: " << min_dist << std::endl;
            // std::cout << "this distance: " << getThisDistance(cur.x(), cur.y()) << std::endl;
            
            if (getThisDistance(cur.x(), cur.y()) > min_dist) continue;
            setThisDistance(cur.x(), cur.y(), min_dist+1);
            
            // push every neighbor onto the stack
            if (cur.x()+1 < 16) st.push(Coord(cur.x()+1, cur.y()));
            if (cur.x()-1 >= 0) st.push(Coord(cur.x()-1, cur.y()));
            if (cur.y()-1 >= 0) st.push(Coord(cur.x(), cur.y()-1));
            if (cur.y()+1 < 16) st.push(Coord(cur.x(), cur.y()+1));
            
            // printMan();
            // Pause at each cell if the user requests it.
            // It allows for better viewing on command line.
            /*if (pause) {
                std::cout << "Hit enter to continue..." << std::endl;
                std::cin.ignore(10000, '\n');
                std::cin.clear();
            }*/
        }
        
        // printMan();
        direction = clockwise(direction);
        return TurnClockwise;
    }
    
    void printMan() {
        for (int d = 0; d < 16; d++) {
            for (int u = 0; u < 16; u++) {
                if (manhattan[d][u] < 10) std::cout << " " << manhattan[d][u];
                else                      std::cout << manhattan[d][u];
            }
            std::cout << std::endl;
        }
    }
    
    void setWall(unsigned x, unsigned y, Dir d) {
        Coord nor(x, y);
        Coord sou(x, y-1);
        Coord eas(x, y);
        Coord wes(x-1, y);
        switch (d) {
            case NORTH:
                TopWall[nor] = true;
                // std::cout << nor.x() << ", " << nor.y() << " has top wall" << std::endl;
                break;
            case SOUTH:
                TopWall[sou] = true;
                // std::cout << sou.x() << ", " << sou.y() << " has top wall" << std::endl;
                break;
            case EAST:
                RightWall[eas] = true;
                // std::cout << eas.x() << ", " << eas.y() << " has right wall" << std::endl;
                break;
            case WEST:
                RightWall[wes] = true;
                // std::cout << wes.x() << ", " << wes.y() << " has right wall" << std::endl;
                break;
            default:
                break;
        }
    }
    
    bool isWall(unsigned x, unsigned y, Dir d) {
        Coord nor(x, y);
        Coord sou(x, y-1);
        Coord eas(x, y);
        Coord wes(x-1, y);
        switch (d) {
            case NORTH:
                if (TopWall[nor]) return true;
                break;
            case SOUTH:
                if (TopWall[sou]) return true;
                break;
            case EAST:
                if (RightWall[eas]) return true;
                break;
            case WEST:
                if (RightWall[wes]) return true;
                break;
            default:
                break;
        }
        return false;
    }
    
    bool isLowerDistance(unsigned x, unsigned y, Dir d) {
        switch (d) {
            case NORTH:
                if (y+1 < 16)
                    if (manhattan[15-y][x] > manhattan[15-(y+1)][x]) return true;
                break;
            case SOUTH:
                if (y-1 >= 0)
                    if (manhattan[15-y][x] > manhattan[15-(y-1)][x]) return true;
                break;
            case EAST:
                if (x+1 < 16)
                    if (manhattan[15-y][x] > manhattan[15-y][x+1]) return true;
                break;
            case WEST:
                if (x-1 >= 0)
                    if (manhattan[15-y][x] > manhattan[15-y][x-1]) return true;
                break;
            default:
                break;
        }
        return false;
    }
    
    int getDistance(unsigned x, unsigned y, Dir d) {
        switch (d) {
            case NORTH:
                if (y+1 < 16)
                    return manhattan[15-(y+1)][x];
                break;
            case SOUTH:
                if (y-1 >= 0)
                    return manhattan[15-(y-1)][x];
                break;
            case EAST:
                if (x+1 < 16)
                    return manhattan[15-y][x+1];
                break;
            case WEST:
                if (x-1 >= 0)
                    return manhattan[15-y][x-1];
                break;
            default:
                break;
        }
        return 9999;
    }
    
    int getThisDistance(unsigned x, unsigned y) {
        return manhattan[15-y][x];
    }
    
    void setThisDistance(unsigned x, unsigned y, int val) {
        manhattan[15-y][x] = val;
    }
    
protected:
    // Manhattan Distance
    int manhattan[16][16];
    
    // Wall hash table
    std::map<Coord, bool> TopWall;
    std::map<Coord, bool> RightWall;
    
    // Direction
    Dir direction;
    
    // Helps us determine that we should go forward if we have just turned left.
    bool shouldGoForward;
    
    // Helps us determine if we've made a loop around the maze without finding the center.
    bool visitedStart;
    
    // Indicates we should pause before moving to next cell.
    // Useful for command line usage.
    const bool pause;
    
    bool isAtCenter(unsigned x, unsigned y) const {
        unsigned midpoint = MazeDefinitions::MAZE_LEN / 2;
        
        if(MazeDefinitions::MAZE_LEN % 2 != 0) {
            return x == midpoint && y == midpoint;
        }
        
        return  (x == midpoint     && y == midpoint    ) ||
        (x == midpoint - 1 && y == midpoint    ) ||
        (x == midpoint     && y == midpoint - 1) ||
        (x == midpoint - 1 && y == midpoint - 1);
    }
};

class LeftWallFollower : public PathFinder {
public:
    LeftWallFollower(bool shouldPause = false) : pause(shouldPause) {
        shouldGoForward = false;
        visitedStart = false;
    }

    MouseMovement nextMovement(unsigned x, unsigned y, const Maze &maze) {
        const bool frontWall = maze.wallInFront();
        const bool leftWall  = maze.wallOnLeft();

        // Pause at each cell if the user requests it.
        // It allows for better viewing on command line.
        std::cout << x << std::endl;
        std::cout << y << std::endl;
        
        if(pause) {
            std::cout << "Hit enter to continue..." << std::endl;
            std::cin.ignore(10000, '\n');
            std::cin.clear();
        }

        std::cout << maze.draw(5) << std::endl << std::endl;

        // If we somehow miraculously hit the center
        // of the maze, just terminate and celebrate!
        if(isAtCenter(x, y)) {
            std::cout << "Found center! Good enough for the demo, won't try to get back." << std::endl;
            return Finish;
        }

        // If we hit the start of the maze a second time, then
        // we couldn't find the center and never will...
        if(x == 0 && y == 0) {
            if(visitedStart) {
                std::cout << "Unable to find center, giving up." << std::endl;
                return Finish;
            } else {
                visitedStart = true;
            }
        }

        // If we have just turned left, we should take that path!
        if(!frontWall && shouldGoForward) {
            shouldGoForward = false;
            return MoveForward;
        }

        // As long as nothing is in front and we have
        // a wall to our left, keep going forward!
        if(!frontWall && leftWall) {
            shouldGoForward = false;
            return MoveForward;
        }

        // If our forward and left paths are blocked
        // we should try going to the right!
        if(frontWall && leftWall) {
            shouldGoForward = false;
            return TurnClockwise;
        }

        // Lastly, if there is no left wall we should take that path!
        if(!leftWall) {
            shouldGoForward = true;
            return TurnCounterClockwise;
        }

        // If we get stuck somehow, just terminate.
        std::cout << "Got stuck..." << std::endl;
        return Finish;
    }

protected:
    // Helps us determine that we should go forward if we have just turned left.
    bool shouldGoForward;

    // Helps us determine if we've made a loop around the maze without finding the center.
    bool visitedStart;

    // Indicates we should pause before moving to next cell.
    // Useful for command line usage.
    const bool pause;

    bool isAtCenter(unsigned x, unsigned y) const {
        unsigned midpoint = MazeDefinitions::MAZE_LEN / 2;

        if(MazeDefinitions::MAZE_LEN % 2 != 0) {
            return x == midpoint && y == midpoint;
        }

        return  (x == midpoint     && y == midpoint    ) ||
        (x == midpoint - 1 && y == midpoint    ) ||
        (x == midpoint     && y == midpoint - 1) ||
        (x == midpoint - 1 && y == midpoint - 1);
    }
};

int main(int argc, char * argv[]) {
    MazeDefinitions::MazeEncodingName mazeName = MazeDefinitions::MAZE_CAMM_2012;
    bool pause = false;

    // Since Windows does not support getopt directly, we will
    // have to parse the command line arguments ourselves.

    // Skip the program name, start with argument index 1
    for(int i = 1; i < argc; i++) {
        if(strcmp(argv[i], "-m") == 0 && i+1 < argc) {
            int mazeOption = atoi(argv[++i]);
            if(mazeOption < MazeDefinitions::MAZE_NAME_MAX && mazeOption > 0) {
                    mazeName = (MazeDefinitions::MazeEncodingName)mazeOption;
            }
        } else if(strcmp(argv[i], "-p") == 0) {
            pause = true;
        } else {
            std::cout << "Usage: " << argv[0] << " [-m N] [-p]" << std::endl;
            std::cout << "\t-m N will load the maze corresponding to N, or 0 if invalid N or missing option" << std::endl;
            std::cout << "\t-p will wait for a newline in between cell traversals" << std::endl;
            return -1;
        }
    }

    FloodFill floodFiller(pause);
    Maze maze(mazeName, &floodFiller);
    
    // LeftWallFollower leftWallFollower(pause);
    // Maze maze(mazeName, &leftWallFollower);
    std::cout << maze.draw(5) << std::endl << std::endl;

    maze.start();
}
