#include "defs.h"

int main()
{
  int i;
  int nextCol = HEALTH_COL + 8;

  //initialization
  initRaceInfoType();

  srand((unsigned)time(NULL));
  initNcurses();

  scrPrt("Health:", HEALTH_ROW, HEALTH_COL);

  //initiates the runner's threads
  printHealthBar(nextCol, "T");
  initRunnerType("Timmy", MAX_HEALTH, "T", START_POS, PATH_1);
  nextCol += 4;
  printHealthBar(nextCol, "H");
  initRunnerType("Harold", MAX_HEALTH, "H", START_POS, PATH_2);
  nextCol += 4;

  //starting the race loop
  raceStart();

  //waits for runner threads to be done and terminates all dorc threads
  raceEnd();

  printWinner();

  //cleaning memory and Ncurses
  cleanupNcurses(START_POS + 2);

  for(i=0; i<race->numRunners; ++i)
  {
    free(race->runners[i]);
  }

  for(i=0; i<race->numDorcs; ++i)
  {
    free(race->dorcs[i]);
  }

  free(race);
}
