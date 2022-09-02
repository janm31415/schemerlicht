#include <iostream>
#include "schemerlicht/schemerlicht.h"

int main(int argc, char** argv)
  {
  COMPILER::schemerlicht_parameters pars;
  pars.heap_size = 64 * 1024 * 1024;
  pars.local_stack = 1024;
  COMPILER::scheme_with_schemerlicht(nullptr, nullptr, pars);

  COMPILER::schemerlicht_repl(argc, argv);

  COMPILER::schemerlicht_quit();

  return 0;
  }