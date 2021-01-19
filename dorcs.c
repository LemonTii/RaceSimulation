#include "defs.h"

/*****************************************************************************
 *
 * Function:     : initEntityType
 *
 * Description   : Initiates an EntityType and creates a thread for the entity
 *
 * Type          : In parameter
 *
 * Parameters    : int  path (The initial column the dorc will be spawned at)
 *
 * Return Value  : void
 *
 *****************************************************************************/
void initEntityType(int path)
{
  race->dorcs[race->numDorcs] = malloc(sizeof(EntityType));
  EntityType *dorc = race->dorcs[race->numDorcs];
  strcpy(dorc->avatar, "d");
  dorc->currPos = 2;
  dorc->path = path;
  ++race->numDorcs;

  pthread_create(&(dorc->thr), NULL, goDorc, dorc);
}

/*****************************************************************************
 *
 * Function:     : goDorc
 *
 * Description   : Executes all functionality of a dorc as a thread
 *
 * Type          : In-out parameter
 *
 * Parameters    : void*  arg (The dorc passed in as a void pointer)
 *
 * Return Value  : void
 *
 *****************************************************************************/
void *goDorc(void *arg)
{
  if(arg == NULL)
  {
    printf("cannot find dorc to run\n");
    return NULL;
  }

	EntityType *dorc = ((EntityType *)arg);
  int nextPos, nextPath;

  while(dorc->currPos != START_POS)
  {
    dNextPos(&nextPos, &nextPath);
    nextPos += dorc->currPos;
    if(nextPos > START_POS)
    {
      nextPos = START_POS;
    }

    if(dorc->path+nextPath <= PATH_1)
    {
      nextPath = PATH_1;
    }
    else if(dorc->path+nextPath >= PATH_2)
    {
      nextPath = PATH_2;
    }
    else
    {
      nextPath = (PATH_1+PATH_2)/2;
    }

    dUpdate(dorc, nextPos, nextPath);
    dorc->currPos = nextPos;
    dorc->path = nextPath;
    usleep(700000);
  }

  if (sem_wait(&(race->mutex)) < 0)
  {
    printf("semaphore wait error\n");
    exit(1);
  }

  scrPrt(" ", dorc->currPos, dorc->path);

  if (sem_post(&(race->mutex)) < 0)
  {
    printf("semaphore post error\n");
    exit(1);
  }
}

/*****************************************************************************
 *
 * Function:     : dNextPos
 *
 * Description   : Calculates the next row the dorc will be at
 *
 * Type          : Out parameter
 *
 * Parameters    : int*  nextPos  (The next row the dorc will go to)
 *                 int*  nextPath (The next column the dorc will go to)
 *
 * Return Value  : void
 *
 *****************************************************************************/
void dNextPos(int *nextPos, int *nextPath)
{
  int choice = randm(3);

  *nextPos = randm(5)+1;

  if(choice == 0)
  {
    *nextPath = 2;
  }
  else if(choice == 1)
  {
    *nextPath = -2;
  }
  else
  {
    *nextPath = 0;
  }
}

/*****************************************************************************
 *
 * Function:     : dUpdate
 *
 * Description   : Updates the dorc to the screen with its new information
 *
 * Type          : In parameter
 *
 * Parameters    : EntityType* dorc     (The dorc being updated to the screen)
 *                 int         nextPos  (The next row in which the dorc will be
 *                                       updated to)
 *                 int         nextPath (The next column in which the dorc will
 *                                       be updated to)
 *
 * Return Value  : void
 *
 *****************************************************************************/
void dUpdate(EntityType *dorc, int nextPos, int nextPath)
{
  if(dorc == NULL)
  {
    printf("Cannot update screen (dorc)\n");
    return;
  }

  if (sem_wait(&(race->mutex)) < 0)
  {
    printf("semaphore wait error\n");
    exit(1);
  }

  scrPrt(" ", dorc->currPos, dorc->path);
  scrPrt(dorc->avatar, nextPos, nextPath);

  if (sem_post(&(race->mutex)) < 0)
  {
    printf("semaphore post error\n");
    exit(1);
  }
}
