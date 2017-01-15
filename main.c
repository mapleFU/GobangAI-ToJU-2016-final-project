#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#define BOARD_SIZE 20
#define EMPTY 0
#define ME 1
#define OTHER 2

#define TRUE 1
#define FALSE 0

#define MAX_BYTE 10000

#define START "START"
#define PLACE "PLACE"
#define DONE  "DONE"
#define TURN  "TURN"
#define BEGIN "BEGIN"
#define END   "END"

#define MAX_DEPTH 6
#define MAX_SEARCH 7
//#define MAX(a, b) ((a) > ((b))?(a):(b))
#define MIN(a, b) ((a) < ((b))?(a):(b))
int MAX(int a, int b)
{
    return (a > b ? a : b);
}


int finalGrade[BOARD_SIZE][BOARD_SIZE] = {0};
const int posValue[BOARD_SIZE][BOARD_SIZE]=             //the basic grade on board
        {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
         {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
         {0,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,0},
         {0,1,2,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,1,0},
         {0,1,2,3,4,4,4,4,4,4,4,4,4,4,4,4,3,2,1,0},
         {0,1,2,3,4,5,5,5,5,5,5,5,5,5,5,4,3,2,1,0},
         {0,1,2,3,4,5,6,6,6,6,6,6,6,6,5,4,3,2,1,0},
         {0,1,2,3,4,5,6,7,7,7,7,7,7,6,5,4,3,2,1,0},
         {0,1,2,3,4,5,6,7,8,8,8,8,7,6,5,4,3,2,1,0},
         {0,1,2,3,4,5,6,7,8,9,9,8,7,6,5,4,3,2,1,0},
         {0,1,2,3,4,5,6,7,8,9,9,8,7,6,5,4,3,2,1,0},
         {0,1,2,3,4,5,6,7,8,8,8,8,7,6,5,4,3,2,1,0},
         {0,1,2,3,4,5,6,7,7,7,7,7,7,6,5,4,3,2,1,0},
         {0,1,2,3,4,5,6,6,6,6,6,6,6,6,5,4,3,2,1,0},
         {0,1,2,3,4,5,5,5,5,5,5,5,5,5,5,4,3,2,1,0},
         {0,1,2,3,4,4,4,4,4,4,4,4,4,4,4,4,3,2,1,0},
         {0,1,2,3,3,3,3,3,3,3,3,3,3,3,3,3,3,2,1,0},
         {0,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,0},
         {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
         {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
        };

const int combo[5][5] =
        {{0,0,0,0,0},
         {0,0,0,0,0},
         {0,0,60,100,100},
         {0,0,100,150,200},
         {0,0,100,200,1000}};
const int groupGde[6] = {0, 10, 30, 100, 500, 10000};
struct Position aiTurn(char board[BOARD_SIZE][BOARD_SIZE], int me, int otherX, int otherY);
#define _INFINITY 12000
const float coef[2] = {1, 1};

struct Position
{
    int x;
    int y;
};

char buffer[MAX_BYTE] = {0};
char board[BOARD_SIZE][BOARD_SIZE] = {0};

void debug(const char *str)
{
    printf("DEBUG %s\n", str);
    fflush(stdout);
}

int isFive(struct Position pos);        //判断是否构成成五
typedef struct _MAXPOS{
    int x;
    int y;
    int score;
}setPos;
/*
 * YOUR CODE BEGIN
 */
bool exist(int x, int y)			//判断点上数字是否存在
{
    if(x >= 0 && x < 20 && y >= 0 && y < 20)
        return TRUE;
    else
        return FALSE;
}

int adjustGroup(int x, int y, int wx, int wy, int com[])       //返回com
{   //site为所执掌者 - 1
    int grade[2] = {0};
    int site = 0;
    for (int i = -2; i <= 2; ++i) {
        if(!exist(x + (i - 2) * wx, y + (i - 2) * wy) || !exist(x + (i + 2) * wx, y + (i + 2) * wy))
            continue;
        int cntOpp = 0, cntSig = 0;
        for (int j = -2; j <= 2; ++j)
        {
            if(board[x + (i + j) * wx][y + (i + j) * wy] == ME)
                site = ME - 1, cntSig++;
            else if(board[x + (i + j) * wx][y + (i + j) * wy] == OTHER)
                site = OTHER - 1, cntOpp++;
        }
        if(cntOpp != 0 && cntSig != 0)            //两个都有值就滚
            continue;
        int cnt = MAX(cntOpp, cntSig); //最大数
        if(cnt > com[site])
            com[site] = cnt;
        grade[site] += groupGde[cnt + 1] + groupGde[cnt];
    }
    return (grade[0] * coef[0] + grade[1] * coef[1]);
}

int adjustPoint(int depth, int x, int y)
{
    int ret = 0;
    int com[4][2] = {0};       //4方向连击
    ret += adjustGroup(x, y, 1, 0, com[0]);    //评测该点的横行
    ret += adjustGroup(x, y, 0, 1, com[1]);
    ret += adjustGroup(x, y, 1, 1, com[2]);
    ret += adjustGroup(x, y, -1, 1, com[3]);

    for (int i = 0; i < 4; ++i) {
        for (int j = i + 1; j < 4; ++j) {
            for (int k = 0; k < 2; ++k) {
                ret += combo[abs(com[i][k])][abs(com[j][k])];
            }
        }
    }
    return ret;
}
/*
 * You can define your own struct and variable here
 */


/*
 * You should init your AI here
 */
bool checkFive(struct Position pos, int wx, int wy)     //检查是否有五子连续
{
    char site = board[pos.x][pos.y];
    int combo = 0,MaxCombo = 0;
    for (int i = -4; i <= 4; ++i) {
        if(!exist(pos.x + i * wx, pos.y + i * wy))      //不存在就不继续
            continue;
        if(board[pos.x + i * wx][pos.y + i * wy] == site)
        {
            combo++;
            if(combo > MaxCombo)
                MaxCombo = combo;
        }
        else
            combo = 0;
    }
    if(MaxCombo >= 5)
        return true;
    else
        return false;
}

int isFive(struct Position pos)
{
    if(checkFive(pos,1,0))
        return 1;
    else if(checkFive(pos,0,1))
        return 1;
    else if(checkFive(pos,1,1))
        return 1;
    else if(checkFive(pos,1,-1))
        return 1;
    return 0;
}

void initAI()
{

}

/*
 * Game Start, you will put the first chess.
 * Warning: This method will only be called when after the initialize ofi the  map, it is your turn to put the chess.
 * Or this method will not be called.
 * You should return a valid Position variable.
 */
struct Position aiBegin(char board[BOARD_SIZE][BOARD_SIZE], int me)
{
    /*
     * TODO: Write your own ai here!
     * Here is a simple AI which just put chess at empty position!
     */
//    int i, j;
    struct Position preferedPos;
    preferedPos = aiTurn((char (*)[20])board, ME, 10, 10);        //全局搜索？
    return preferedPos;
}


/*
 * Game ongoing, the competitor put the chess at the position (otherX, otherY). You should put your chess.
 * You should return a valid Position variable.
 */
void swap(setPos *a, setPos *b)
{
    setPos tmp = *a;
    *a = *b;
    *b = tmp;
}

void insert(setPos best[])
{
    for (int i = MAX_SEARCH - 1; i > 0 && best[i - 1].score < best[i].score; --i) {
        swap(&best[i - 1], &best[i]);
    }
}

int deeper(setPos best, int MaxBreak, int depth)
{
    if(depth == MAX_DEPTH)
        return 0;
    int maxG = -_INFINITY;
    struct Position pos;
    pos.x = best.x,pos.y = best.y;
    char site;
    if(depth % 2 == 1)
        site = ME;
    else
        site = OTHER;
    board[best.x][best.y] = site;
    if(isFive(pos))
    {
        board[best.x][best.y] = EMPTY;
        return -_INFINITY;
        printf("*%d*\n",site);
    }
    setPos thisbest[MAX_SEARCH];
    memset(thisbest, 0, sizeof(thisbest));
    int inserts = 0;            //被插入排序的数目
    for (int k = 0; k < BOARD_SIZE; ++k)
        for (int l = 0; l < BOARD_SIZE; ++l) {
            if (board[k][l] != 0)
                continue;
            int tmp_G = 0;
            tmp_G = adjustPoint(1, k, l) + posValue[k][l];            //x,y,board;
            if(tmp_G > MaxBreak)
            {
                maxG = _INFINITY;                 //大于最大值
                board[best.x][best.y] = EMPTY;
                return maxG;
            }
            if(tmp_G > thisbest[MAX_SEARCH - 1].score)
            {
                thisbest[MAX_SEARCH - 1].x = k, thisbest[MAX_SEARCH - 1].y = l, thisbest[MAX_SEARCH - 1].score = tmp_G;
                insert(thisbest);
                inserts++;
            }
            if (tmp_G > maxG)                    //考虑以后做插入排序剪枝
            {
                maxG = tmp_G;
            }
        }
    if(depth == MAX_DEPTH - 1)
        return maxG;
    int anoScore[MAX_SEARCH];
    int maxBreak = _INFINITY;
    for (int i = 0; i < MIN(inserts,MAX_SEARCH); ++i) {
        anoScore[i] = deeper(thisbest[i], maxBreak, depth + 1);
        if(anoScore[i] < maxBreak)
            maxBreak = anoScore[i];
    }
    int max2 = -_INFINITY;
    for (int j = 0; j < MIN(inserts,MAX_SEARCH); ++j) {
        if(max2 < thisbest[j].score / 3 - anoScore[j] / 3 * 4)
        {
            max2 = thisbest[j].score / 3 - anoScore[j] / 3 * 4;
        }
    }
    board[best.x][best.y] = EMPTY;
    return max2;
}

struct Position aiTurn(char board[BOARD_SIZE][BOARD_SIZE], int me, int otherX, int otherY)
{
    /*
     * TODO: Write your own ai here!
     * Here is a simple AI which just put chess at empty position!
     */         //未设计连续冲4
    struct Position preferedPos;
//    memset(finalGrade, 0, sizeof(finalGrade));
    int maxG = -_INFINITY;
    setPos best[MAX_SEARCH];
    memset(best, 0, sizeof(best));
    preferedPos.x = preferedPos.y = 0;
    int inserts = 0;            //被插入排序的数目
    for (int k = 0; k < BOARD_SIZE; ++k)
        for (int l = 0; l < BOARD_SIZE; ++l) {
            int tmp_G = 0;
            if (board[k][l] != 0)
                continue;
            tmp_G = adjustPoint(1, k, l) + posValue[k][l];            //x,y,board;
            if(tmp_G > best[MAX_SEARCH - 1].score)
            {
                best[MAX_SEARCH - 1].x = k, best[MAX_SEARCH - 1].y = l, best[MAX_SEARCH - 1].score = tmp_G;
                insert(best);
                inserts++;
            }
            if (tmp_G > maxG)                    //考虑以后做插入排序剪枝
            {
                preferedPos.x = k;
                preferedPos.y = l;
                maxG = tmp_G;
            }
        }
    if(/*best[0].score < 1000 &&*/ inserts != 0)
    {
        int anoScore[MAX_SEARCH];
        int MaxBreak = _INFINITY;
        for (int i = 0; i < MIN(inserts,MAX_SEARCH); ++i) {
            anoScore[i] = deeper(best[i], MaxBreak, 1);     //第一层挖掘
            if(anoScore[i] < MaxBreak)
                MaxBreak = anoScore[i];
        }
        int max2 = -_INFINITY;
        for (int j = 0; j < MIN(inserts, MAX_SEARCH); ++j) {
            if(max2 <  best[j].score - anoScore[j])
            {
                max2 = best[j].score - anoScore[j];
                preferedPos.x = best[j].x;
                preferedPos.y = best[j].y;
            }
        }
    }
    if(inserts == 0)
    {
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                if(board[i][j] == EMPTY)
                {
                    preferedPos.x = i;
                    preferedPos.y = j;
                    return preferedPos;
                }
            }
        }
    }
    return preferedPos;
}

/*
 * YOUR CODE END
 */

void place(int x, int y, int z)
{
    board[x][y] = z;
}

void done()
{
    printf("OK\n");
    fflush(stdout);
}

void start()
{
    memset(board, 0, sizeof(board));
    initAI();
}

void turn(int x, int y)
{
    board[x][y] = OTHER;

    // AI
    struct Position pos = aiTurn((char (*)[20])board, ME, x, y);
    board[pos.x][pos.y] = ME;
    printf("%d %d\n", pos.x, pos.y);
    fflush(stdout);
}

void end(int x)
{

}

void loop()
{
    while (TRUE)
    {
        memset(buffer, 0, sizeof(buffer));
        gets(buffer);

        if (strstr(buffer, START))
        {
            start();
        }
        else if (strstr(buffer, PLACE))
        {
            char tmp[MAX_BYTE] = {0};
            int x,  y, z;
            sscanf(buffer, "%s %d %d %d", tmp, &x, &y, &z);
            place(x, y, z);
        }
        else if (strstr(buffer, DONE))
        {
            done();
        }
        else if (strstr(buffer, BEGIN))
        {
            // AI
            struct Position pos = aiBegin((char (*)[20])board, ME);
            board[pos.x][pos.y] = ME;
            printf("%d %d\n", pos.x, pos.y);
            fflush(stdout);
        }
        else if (strstr(buffer, TURN))
        {
            char tmp[MAX_BYTE] = {0};
            int x,  y;
            sscanf(buffer, "%s %d %d", tmp, &x, &y);
            turn(x, y);
        }
        else if (strstr(buffer, END))
        {
            char tmp[MAX_BYTE] = {0};
            int x;
            sscanf(buffer, "%s %d", tmp, &x);
            end(x);
        }
    }
}


int main(int argc, char *argv[]) {
    loop();
    return 0;
}