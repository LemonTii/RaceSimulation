#include "defs.h"

/*****************************************************************************
 *
 * Function:     : initRaceInfoType
 *
 * Description   : Initates a RaceInfoType (the global variable) for the threads
 *
 * Type          : No parameters
 *
 * Parameters    : none
 *
 * Return Value  : void
 *
 *****************************************************************************/
void initRaceInfoType()
{
  race = malloc(sizeof(RaceInfoType));
  race->numRunners = 0;
  race->numDorcs = 0;
  strcpy(race->winner, "");
  race->statusRow = 4;
  if (sem_init(&(race->mutex), 0, 1) < 0)
  {
    printf("semaphore initialization error, cannot start program\n");
    exit(1);
  }
}

/*****************************************************************************
 *
 * Function:     : raceStart
 *
 * Description   : Starts the race loop which randomly generates dorcs
 *
 * Type          : No parameters
 *
 * Parameters    : none
 *
 * Return Value  : void
 *
 *****************************************************************************/
void raceStart()
{
  int tempNum = 0, i = 0;

  if (sem_wait(&(race->mutex)) < 0)
  {
    printf("semaphore wait error\n");
    exit(1);
  }

  scrPrt("------------------------", START_POS+1, 0);
  scrPrt("------------------------", END_POS-1, 0);

  if (sem_post(&(race->mutex)) < 0)
  {
    printf("semaphore post error\n");
    exit(1);
  }

  while(1)
  {
    tempNum = 0;

    if (sem_wait(&(race->mutex)) < 0)
    {
      printf("semaphore wait error\n");
      exit(1);
    }

    if(strlen(race->winner) != 0)
    {
      break;
    }

    for(i=0; i<(race->numRunners); ++i)
    {
      if(race->runners[i]->dead == C_FALSE)
      {
        tempNum = 1;
      }
    }

    if(tempNum == 0)
    {
      break;
    }

    if (sem_post(&(race->mutex)) < 0)
    {
      printf("semaphore post error\n");
      exit(1);
    }

    if(randm(10)<DORC_FREQ)
    {
      tempNum = randm(3);
      if(tempNum == 0)
      {
        tempNum = PATH_1;
      }
      else if(tempNum == 1)
      {
        tempNum = (PATH_1 + PATH_2)/2;
      }
      else
      {
        tempNum = PATH_2;
      }
      initEntityType(tempNum);
    }
    usleep(250000);
  }
  if (sem_post(&(race->mutex)) < 0)
  {
    printf("semaphore post error\n");
    exit(1);
  }
}

/*****************************************************************************
 *
 * Function:     : printHealthBar
 *
 * Description   : Prints the initial health bar using a runner's avatar
 *
 * Type          : In parameter
 *
 * Parameters    : int    nextCol (Column in which the avatar will be placed)
 *                 char*  avatar  (The avatar of the runner)
 *
 * Return Value  : void
 *
 *****************************************************************************/
void printHealthBar(int nextCol, char *avatar)
{
  if (sem_wait(&(race->mutex)) < 0)
  {
    printf("semaphore wait error\n");
    exit(1);
  }

  scrPrt(avatar, HEALTH_ROW, nextCol);

  if (sem_post(&(race->mutex)) < 0)
  {
    printf("semaphore post error\n");
    exit(1);
  }
}

/*****************************************************************************
 *
 * Function:     : printStatus
 *
 * Description   : Prints the Events that happen during the race
 *
 * Type          : In parameter
 *
 * Parameters    : int          statusType (Determines the status to be printed)
 *                 RunnerType*  runner     (Runner experiencing the event)
 *
 * Return Value  : void
 *
 *****************************************************************************/
void printStatus(int statusType, RunnerType *runner)
{
  if(runner == NULL)
  {
    printf("cannot print status\n");
    return;
  }

  char str[MAX_BUF];

  if(statusType == 0)
  {
    strcpy(str, "Status: collision between ");
    strcat(str, runner->name);
    strcat(str, " and dorc");
    scrPrt(str, race->statusRow, STATUS_COL);
    ++race->statusRow;
  }
  else
  {
    strcpy(str, "Status: ");
    strcat(str, runner->name);
    strcat(str, " is dead");
    scrPrt(str, race->statusRow, STATUS_COL);
    ++race->statusRow;
  }
}

/*****************************************************************************
 *
 * Function:     : raceEnd
 *
 * Description   : Has the function to wait for the runners to end and
 *                 terminates the dorc threads
 *
 * Type          : No parameters
 *
 * Parameters    : none
 *
 * Return Value  : void
 *
 *****************************************************************************/
void raceEnd()
{
  int i;

  for(i=0; i<race->numRunners; ++i)
  {
    pthread_join(race->runners[i]->ent.thr, NULL);
  }

  for(i=0; i<race->numDorcs; ++i)
  {
    pthread_cancel(race->dorcs[i]->thr);
  }
}

/*****************************************************************************
 *
 * Function:     : printWinner
 *
 * Description   : Prints the winner of the race
 *
 * Type          : No parameters
 *
 * Parameters    : none
 *
 * Return Value  : void
 *
 *****************************************************************************/
void printWinner()
{
  char str[MAX_BUF];
  strcpy(str, "Outcome: The winner is ");

  if(strlen(race->winner) != 0)
  {
    strcat(str, race->winner);
    scrPrt(str, race->statusRow, STATUS_COL);
  }
  else
  {
    scrPrt("Nobody won", race->statusRow, STATUS_COL);
  }
}
