/* 
 * Author: Chen Rushan
 * E-Mail: juscodit@gmail.com
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <math.h>

#define MAX_NPOINT 1002

struct Point {
    float x;
    float y;
};

/**
 * RETURN:
 *  = 0: (p, q, r) on a line
 *  < 0: clockwise
 *  > 0: counterclockwise
 */
int
pointOrientation(struct Point *p, struct Point *q, struct Point *r)
{
    double v = (r->y - q->y) * (q->x - p->x) - (q->y - p->y) * (r->x - q->x);
    if (v < 0) {
        return -1;
    } else if (v > 0) {
        return 1;
    }
    return 0;
}

/**
 * RETURN:
 *   1: if @p is lower than @q
 *   2: if @p is higher than @q
 */
int
pointWhoIsLower(struct Point *p, struct Point *q)
{
    if (p->y > q->y) {
        return 2;
    } else if (p->y < q->y) {
        return 1;
    } else {
        if (p->x > q->x) {
            return 2;
        } else {
            return 1;
        }
    }
}

int
pointEqual(struct Point *p, struct Point *q)
{
    if (p->x == q->x && p->y == q->y) {
        return 1;
    }
    return 0;
}

float
pointDist(struct Point *p, struct Point *q)
{
    return sqrt((p->y - q->y) * (p->y - q->y) +
                (p->x - q->x) * (p->x - q->x));
}

struct Point pointSet[MAX_NPOINT];
int npoints; /* number of points in point set */

int order[MAX_NPOINT]; /* order[i] is index of @pointSet */
int path[MAX_NPOINT]; /* path[i] is index of @pointSet */
float dist[MAX_NPOINT]; /* dist[i] is distance between path[i] and path[i+1],
                           dist[npoints - 1] is the distance between
                           path[npoints - 1] and path[0] */
float dist0[MAX_NPOINT]; /* dist0[i] is the distance between path[i] and origin */
int top; /* top index of @path */

int lowest; /* index of lowest point in pointSet */

struct Point origin = {0, 0};

/**
 * Return the index of the lowest point in pointSet
 */
int
lowestPoint(void)
{
    int i = 0, lowest = 0;

    for (i = 1; i < npoints; ++i) {
        if (pointWhoIsLower(pointSet + i, pointSet + lowest) == 1) {
            lowest = i;
        }
    }
    return lowest;
}

int
cmpPoint(const void *p, const void *q)
{
    int i = *(int *)p, j = *(int *)q;

    /* make the lowest point smallest */
    if (pointEqual(pointSet + i, pointSet + lowest)) {
        return -1;
    }
    if (pointEqual(pointSet + j, pointSet + lowest)) {
        return 1;
    }

    /* check orientation, if (i, lowest, j) is not on the same line,
     * just return orientation, otherwise, check if i is the same as j,
     * I want to put the same points together so I can easily remove
     * duplication after sorting */
    int ori = pointOrientation(pointSet + i, pointSet + lowest, pointSet + j);
    if (ori != 0) {
        return ori;
    }
    if (pointEqual(pointSet + i, pointSet + j)) {
        return 0;
    }
    return 1;
}

void
removeDuplicate(void)
{
    int nps = 0, i = 0;

    for (i = 1; i < npoints; ++i) {
        if (! pointEqual(pointSet + order[i], pointSet + order[nps])) {
            order[++nps] = order[i];
        }
    }
    npoints = nps + 1;
}

/**
 * find convex hull and store into @path
 */
void
getConvexHull(void)
{
    int i = 0;

    /* init path */
    path[0] = order[0];
    path[1] = order[1];
    path[2] = order[2];
    top = 2;

    /* set path */
    for (i = 3; i < npoints; ++i) {
        int ori = pointOrientation(pointSet + path[top - 1],
                    pointSet + path[top], pointSet + order[i]);
        while (ori < 0) {
            top -= 1;
            ori = pointOrientation(pointSet + path[top - 1],
                    pointSet + path[top], pointSet + order[i]);
        }
        path[++top] = order[i];
    }

    /* set dist */
    for (i = 0; i <= top - 1; ++i) {
        dist[i] = pointDist(pointSet + path[i], pointSet + path[i + 1]);
        dist0[i] = pointDist(pointSet + path[i], &origin);
    }
    dist[i] = pointDist(pointSet + path[i], pointSet + path[0]);
    dist0[i] = pointDist(pointSet + path[i], &origin);
    dist0[i + 1] = dist0[0]; /* for efficiency */
}

float
addOrigin(void)
{
    float sum = 0, min = 0;
    int i = 0;

    for (i = 0; i <= top; ++i) {
        sum += dist[i];
    }

    /* check if origin already in path */
    for (i = 0; i <= top; ++i) {
        if (pointEqual(pointSet + path[i], &origin)) {
            break;
        }
    }
    if (i != top + 1) {
        return sum + 2;
    }

    min = sum - dist[0] + dist0[0] + dist0[1];

    for (i = 1; i <= top; ++i) {
        float v = sum - dist[i] + dist0[i] + dist0[i + 1];
        if (v < min) {
            min = v;
        }
    }

    return min + 2;
}

int
main(int argc, char **argv)
{
    int ncases = 0, i = 0;
    float x = 0, y = 0;

    scanf("%d", &ncases);

    while (ncases--) {
        /* read in point set */
        scanf("%d", &npoints);
        for (i = 0; i < npoints; ++i) {
            scanf("%f%f", &x, &y);
            pointSet[i].x = x;
            pointSet[i].y = y;
            order[i] = i;
        }
        /* append origin (the telephone post) */
        pointSet[npoints].x = 0;
        pointSet[npoints].y = 0;
        order[npoints] = npoints;
        npoints += 1;

        /* find the lowest point */
        lowest = lowestPoint();

        /* sort all points with respect to the lowest point */
        qsort(order, npoints, sizeof(*order), cmpPoint);

        removeDuplicate();

        getConvexHull();

        float len = addOrigin();
        printf("%.2f\n", len);
        if (ncases != 0) {
            printf("\n");
        }
    }

    return 0;
}

