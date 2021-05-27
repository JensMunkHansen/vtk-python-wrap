#include "vtkInteractorStyleGame.h"

#include <sstream>

#include <string.h>
#include <stdlib.h>

// macro for performing tests
#define TestAssert(t) \
if (!(t)) \
{ \
  cout << exename << ": Assertion Failed: " << #t << "\n"; \
  cout << __FILE__ << ":" << __LINE__ << "\n"; \
  cout.flush(); \
  rval |= 1; \
}

#ifdef VTK_IN_VTK
int TestInteractorStyleGame(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
  int rval = 0;
  const char *exename = (argc > 0 ? argv[0] : "TestInteractorStyleGame");

  // remove path portion of exename
  const char *cp = exename + strlen(exename);
  while (cp != exename && cp[-1] != '\\' && cp[-1] != '/') { --cp; }
  exename = cp;

  vtkInteractorStyleGame *meta = vtkInteractorStyleGame::New();

  // test something
  {
    TestAssert(0 == 0);
  }

  meta->Delete();
  return rval;
}
