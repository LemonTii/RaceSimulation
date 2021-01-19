#include "defs.h"

/*****************************************************************************
 *
 * Function:     : initRunnerType
 *
 * Description   : initiates a RunnerType and its thread
 *
 * Type          : In parameter
 *
 * Parameters    : char*   name    (Name of the runner)
 *                 int     health  (Runner's inital health)
 *                 char*   avatar  (Avatar of the runner to be displayed)
 *                 int     currPos (Initial row for the runner to start at)
 *                 int     path    (Initial column for the runner to start)
 *
 * Return Value  : void
 *
 *****************************************************************************/
void initRunnerType(char* name, int health, char* avatar, int currPos, int path)
{
  char converted[MAX_BUF];

  race->runners[race->numRunners] = malloc(sizeof(RunnerType));
  RunnerType *runner = race->runners[race->numRunners];

  strcpy(runner->name, name);
  runner->health = health;
  runner->dead = C_FALSE;

  strcpy(runner->ent.avatar, avatar);
  runner->ent.currPos = currPos;
  runner->ent.path = path;

  race->numRunners++;

  sprintf(converted, "%d", health);

  if (sem_wait(&(race->mutex)) < 0)
  {
    printf("semaphore wait error\n");
    exit(1);
  }

  scrPrt(converted, HEALTH_ROW + 2, path + 28);

  if (sem_post(&(race->mutex)) < 0)
  {
    printf("semaphore post error\n");
    exit(1);
  }
  pthread_create(&(runner->ent.thr), NULL, goRunner, runner);
}

/*****************************************************************************
 *
 * Function:     : goRunner
 *
 * Description   : Does all functionality required for a runner as a thread
 *
 * Type          : In-out parameter
 *
 * Parameters    : void*   arg    (The RunnerType passed as a void pointer)
 *
 * Return Value  : void
 *
 *****************************************************************************/
void *goRunner(void *arg)
{
  if(arg == NULL)
  {
    printf("runner not found\n");
    return NULL;
  }

	RunnerType *runner = ((RunnerType *)arg);
  int nextPos = 0;
  int newHealth = runner->health;

  if (sem_wait(&(race->mutex)) < 0)
  {
    printf("semaphore wait error\n");
    exit(1);
  }

  scrPrt(runner->ent.avatar, runner->ent.currPos, runner->ent.path);
  usleep(250000);

  if (sem_post(&(race->mutex)) < 0)
  {
    printf("semaphore post error\n");
    exit(1);
  }

  while(!((runner->ent.currPos == END_POS) || (runner->dead == C_TRUE)))
  {
    rNextPos(&nextPos, &newHealth);
    nextPos += runner->ent.currPos;
    if(nextPos < END_POS)
    {
      nextPos = END_POS;
    }
    if(nextPos > START_POS)
    {
      nextPos = START_POS;
    }

    checkCollide(&newHealth, nextPos, runner->ent.path, runner);

    checkAlive(runner, &newHealth);;

    rUpdate(runner, nextPos);

    runner->ent.currPos = nextPos;
    usleep(250000);
  }

  if (sem_wait(&(race->mutex)) < 0)
  {
    printf("semaphore wait error\n");
    exit(1);
  }

  if(runner->dead == C_FALSE && strlen(race->winner) == 0)
  {
    strcpy(race->winner, runner->name);
  }

  if (sem_post(&(race->mutex)) < 0)
  {
    printf("semaphore post error\n");
    exit(1);
  }
}

/*****************************************************************************
 *
 * Function:     : rNextPos
 *
 * Description   : Calculates the next row in which the runner will be at
 *
 * Type          : In-out parameter
 *
 * Parameters    : int*   nextPos    (The varialbe to save the next row)
 *                 int*   newHealth  (The health runner will have after moving
 *                                    to the next row)
 *
 * Return Value  : void
 *
 *****************************************************************************/
void rNextPos(int *nextPos, int *newHealth)
{
  if(newHealth == NULL)
  {
    printf("Cannot calculate new position\n");
    return;
  }

  int choice = randm(10);
  if(choice < 3)
  {
    *nextPos = randm(3) + 1;
  }
  else
  {
    *nextPos = -(randm(4)+1);
    *newHealth += *nextPos;
  }
}

/*****************************************************************************
 *
 * Function:     : checkCollide
 *
 * Description   : Checks if the runner will collide with a dorc at the next row
 *
 * Type          : In-out parameter
 *
 * Parameters    : int*          newHealth  (The health runner will have after
 *                                           colliding)
 *                 int           pos        (Runner's next row)
 *                 int           path       (The column in which the runner is at)
 *                 RunnerType*   runner     (The runner being checked)
 *
 * Return Value  : void
 *
 *****************************************************************************/
void checkCollide(int *newHealth, int pos, int path, RunnerType *runner)
{
  if(newHealth == NULL || runner == NULL)
  {
    printf("Cannot check for collision\n");
    return;
  }

  int i;
  if (sem_wait(&(race->mutex)) < 0)
  {
    printf("semaphore wait error\n");
    exit(1);
  }

  for(i=0; i<race->numDorcs; ++i)
  {
    if(race->dorcs[i]->currPos == pos && race->dorcs[i]->path == path)
    {
      printStatus(0, runner);
      *newHealth -= 3;
    }
  }

  if (sem_post(&(race->mutex)) < 0)
  {
    printf("semaphore post error\n");
    exit(1);
  }
}

/*****************************************************************************
 *
 * Function:     : checkAlive
 *
 * Description   : Check if the runner would be alive with its new health
 *
 * Type          : In-out parameter
 *
 * Parameters    : RunnerType*  runner    (The runner being checked)
 *                 int*         newHealth (The health to check with)
 *
 * Return Value  : void
 *
 *****************************************************************************/
void checkAlive(RunnerType *runner, int *newHealth)
{
  if(newHealth == NULL || runner == NULL)
  {
    printf("failed to check if runner is alive\n");
    return;
  }

  if (sem_wait(&(race->mutex)) < 0)
  {
    printf("semaphore wait error\n");
    exit(1);
  }

  if(*newHealth <= 0)
  {
    runner->dead = C_TRUE;
    strcpy(runner->ent.avatar, "+");
    runner->health = 0;

    printStatus(1, runner);
  }
  else
  {
    runner->health = *newHealth;
  }

  if (sem_post(&(race->mutex)) < 0)
  {
    printf("semaphore post error\n");
    exit(1);
  }
}

/*****************************************************************************
 *
 * Function:     : rUpdate
 *
 * Description   : Updates the screen with the new information of the runner
 *
 * Type          : In parameter
 *
 * Parameters    : RunnerType* runner  (The runner being updated to the screen)
 *                 int         nextPos (The next row the runner will be at on
 *                                      screen)
 *
 * Return Value  : void
 *
 *****************************************************************************/
void rUpdate(RunnerType *runner, int nextPos)
{
  if(runner == NULL)
  {
    printf("failed to update screen (runner)\n");
    return;
  }

  char converted[MAX_BUF];

  if (sem_wait(&(race->mutex)) < 0)
  {
    printf("semaphore wait error\n");
    exit(1);
  }

  scrPrt(" ", runner->ent.currPos, runner->ent.path);
  scrPrt(runner->ent.avatar, nextPos, runner->ent.path);
  sprintf(converted, "%d", runner->health);
  scrPrt("  ", HEALTH_ROW + 2, runner->ent.path + 28);
  scrPrt(converted, HEALTH_ROW + 2, runner->ent.path + 28);

  if (sem_post(&(race->mutex)) < 0)
  {
    printf("semaphore post error\n");
    exit(1);
  }
}
