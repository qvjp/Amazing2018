//
//  main.c
//  a-start
//
//  Created by Qv Junping on 2018/4/3.
//  Copyright © 2018 Qv Junping. All rights reserved.
//

#include <stdio.h>
#include <assert.h> // assert();
#include <math.h>   // sqrt();
#include <stdlib.h> // abs();

#define HEIGHT 20
#define WIDTH 70

typedef struct nodeItem nodeItem;

struct nodeItem {
    int isWall;    // 1是墙
    int x, y;
    int G, H, F;
    nodeItem* parent;
    int isPath;
};

nodeItem openList[HEIGHT * WIDTH];
nodeItem closeList[HEIGHT * WIDTH];

void initListItem(nodeItem* list)
{
    for (int i = 0; i < HEIGHT * WIDTH; i++)
    {
        list[i].x = -1;
        list[i].y = -1;
        list[i].F = -1;
    }
}

void addListItem(nodeItem* list, nodeItem node)
{
    for (int i = 0; i < HEIGHT * WIDTH; i++)
        if (list[i].x == -1 && list[i].y == -1)
        {
            list[i] = node;
            break;
        }
}
void rmListItem(nodeItem* list, nodeItem node)
{
    for (int i = 0; i < HEIGHT * WIDTH; i++)
        if (list[i].x == node.x && list[i].y == node.y)
        {
            list[i].x = -1;
            list[i].y = -1;
            break;
        }
}

int countOfList(nodeItem* list)
{
    int i = 0;
    for (int j = 0; j < HEIGHT * WIDTH; j++)
    {
        if (list[j].x != -1)
            i++;
    }
    return i;
}

void printList(nodeItem* list)
{
    printf("list: ");
    for (int i = 0; i < WIDTH * HEIGHT; i++)
    {
        if (list[i].x == -1 && list[i].y == -1)
            continue;
        printf("(%d %d) ", list[i].x, list[i].y);
    }
    printf("\n");
}

nodeItem map[HEIGHT][WIDTH];
void initMap()
{
    for (int i = 0; i < HEIGHT; i++)
        for (int j = 0; j < WIDTH; j++)
        {
            map[i][j].isWall = 0;
            map[i][j].x = i;
            map[i][j].y = j;
            map[i][j].G = 0;
            map[i][j].H = -1;
            map[i][j].F = -1;
            map[i][j].parent = NULL;
            map[i][j].isPath = 0;
        }
}

int containsNode(nodeItem* list, nodeItem node);

nodeItem nbResult[8]; //存放当前节点的八个相邻节点
// 获取相邻节点
nodeItem* getNbhNodes(nodeItem node)
{
    for (int i = 0; i < 8; i++)
    {
        nbResult[i].x = -1;
        nbResult[i].y = -1;
    }
    int x = node.x;
    int y = node.y;
    if (y > 0 && x > 0 && map[x-1][y-1].isWall != 1 && !containsNode(closeList, map[x-1][y-1]))
    {
        map[x-1][y-1].G = 14;        
        nbResult[0] = map[x-1][y-1]; // up-left
    }
    if (y > 0 && map[x][y-1].isWall != 1 && !containsNode(closeList, map[x][y-1]))
    {
        map[x][y-1].G = 10;
        nbResult[1] = map[x][y-1]; // up
    }
    if (x < HEIGHT - 1 && y > 0 && map[x+1][y-1].isWall != 1 && !containsNode(closeList, map[x+1][y-1]))
    {
        map[x+1][y-1].G = 14;
        nbResult[2] = map[x+1][y-1]; // up-right
    }
    if (x < HEIGHT - 1 && map[x+1][y].isWall != 1 && !containsNode(closeList, map[x+1][y]))
    {
        map[x+1][y].G = 10;
        nbResult[3] = map[x+1][y]; // right
    }
    if (x < HEIGHT - 1 && x < WIDTH - 1 && map[x+1][y+1].isWall != 1 && !containsNode(closeList, map[x+1][y+1]))
    {
        map[x+1][y+1].G = 14;
        nbResult[4] = map[x+1][y+1]; // down-right
    }
    if (y < WIDTH - 1 && map[x][y+1].isWall != 1 && !containsNode(closeList, map[x][y+1]))
    {
        map[x][y+1].G = 10;
        nbResult[5] = map[x][y+1]; // down
    }
    if (x > 0 && y > 0 && map[x-1][y-1].isWall != 1 && !containsNode(closeList, map[x-1][y-1]))
    {
        map[x-1][y-1].G = 14;
        nbResult[6] = map[x-1][y-1]; // down-left
    }
    if (x > 0 && map[x-1][y].isWall != 1 && !containsNode(closeList, map[x-1][y]))
    {
        map[x-1][y].G = 10;
        nbResult[7] = map[x-1][y]; // left
    }

    return nbResult;
}

int getDistance0(nodeItem a, nodeItem b) //几何估价法
{
    int x = a.x - b.x;
    int y = a.y - b.y;
    return sqrt(x * x + y * y);
}

int getDistance1(nodeItem a, nodeItem b) // 对角线估价法
{
    int x = abs(a.x - b.x);
    int y = abs(a.y - b.y);
    if (x > y)
        return (14 * y + 10 * (x - y));
    else
        return (14 * x + 10 * (y - x));
}

int getDistance2(nodeItem a, nodeItem b) // 曼哈顿估价法
{
    int x = abs(a.x - b.x);
    int y = abs(a.y - b.y);
    return y + x;
}

nodeItem findMinNode(nodeItem* list)
{
    nodeItem result = *list;
    for (int i = 0; i < HEIGHT * WIDTH; i++)
    {
        if ((list[i].F != -1) && (list[i].G + list[i].H <= result.G + result.H) && list[i].x != -1)
            result = list[i];
    }
    return result;
}

int containsNode(nodeItem* list, nodeItem node)
{
    for (int i = 0; i < WIDTH * HEIGHT; i++)
    {
        if (list[i].x == node.x && list[i].y == node.y)
        {
            return 1;
        }
    }
    return 0;
}

int AStarSearch(nodeItem* star, nodeItem* end)
{
    star->F = getDistance1(*star, *end);
    addListItem(openList, *star);
    while(countOfList(openList) > 0)
    {
        nodeItem currentNode = findMinNode(openList);
        assert(currentNode.x != -1);
        rmListItem(openList, currentNode);
        addListItem(closeList, currentNode);
        nodeItem* nb = getNbhNodes(currentNode);
        for (int i = 0; i < 8; i++)
        {
            if (nb[i].x != -1 && nb[i].isWall != 1 && !containsNode(openList, nb[i]))
            {
                nb[i].parent = &map[currentNode.x][currentNode.y];
                nb[i].H = getDistance1(nb[i], *end);
                nb[i].F = nb[i].G + nb[i].H;
                addListItem(openList, nb[i]);
                map[nb[i].x][nb[i].y] = nb[i];
            }
        }
        if (containsNode(openList, *end))
        {
            return 1;
        }
    }
    return 0;
}

void printNode()
{
    // printf("  ");
    // for (int i = 0; i < WIDTH; i++)
    // {
    //     printf("%d", i);
    // }
    // printf("\n");
    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0; j < WIDTH; j++)
        {
            if (map[i][j].isPath)
            {
                if (map[i][j].isPath == 10) //起点紫色
                    printf("\e[34m%c\e[0m", '*');
                else if (map[i][j].isPath == 100)//终点红色
                    printf("\e[31m%c\e[0m", '*');
                else                        // 路线 绿色
                    printf("\e[32m%c\e[0m", '*');
            }
            else if(map[i][j].isWall)
                printf("\e[0;30;40m%c\e[0m", ' ');
            else
                printf("\e[0m%c", 'O');
        }
        printf("\e[0m\n"); //恢复黑色
    }
}

void setWall()
{
    for (int i = 10; i > 0; i--)
    {
        map[i-1][20].isWall = 1;
        map[i + 8][30].isWall = 1;
        map[i+10][35].isWall = 1;
        map[9][i+20].isWall = 1;
        map[i-1][50].isWall = 1;
        map[i+8][50].isWall = 1;
        map[13][50].isWall = 0;
    }
}

int main(int argc, char** argv)
{
    //printf("Welcome to A* (A-Star) Study Room.\n");
    initMap();
    setWall();
    initListItem(openList);
    initListItem(closeList);
    nodeItem* starNode = &map[0][0];
    nodeItem* endNode = &map[19][69];
    map[starNode->x][starNode->y].isPath = 10;
    map[endNode->x][endNode->y].isPath = 100;
    
    int result = AStarSearch(starNode, endNode);
    if (!result)
    {
        printf("result: %d\n", result);
        return 1;
    }
    for (nodeItem a = *endNode; a.parent!=NULL; a = *a.parent)
    {
        // printf("(%d %d) <- ", a.x, a.y);
        map[a.x][a.y].isPath = 1;
    }
    printf("\e[34m(Start)(%d %d)\e[32m -> -> -> -> \e[0m", starNode->x, starNode->y);
    printf("\e[31m(%d %d)(End)\e[0m\n", endNode->x, endNode->y);    
    map[endNode->x][endNode->y].isPath = 100;
    printNode();
    return 0;
}
