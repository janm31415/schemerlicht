#include <iostream>

#include "life.h"
#include "window.h"

#include <atomic>
#include <memory>
#include <random>
#include <thread>

extern "C"
  {
#include "schemerlicht/schemerlicht_api.h"
  }

int width = 100;
int height = 100;

cell_map grid;
std::vector<uint8_t> image;
WindowHandle wh;
std::unique_ptr<std::thread> game_thread;
std::atomic<bool> stop_game_thread;
std::atomic<double> game_sleep_time(50.0);

void paint_grid()
  {
  const int nr_bytes = grid.width * grid.height;
  if (image.size() != nr_bytes)
    image.resize(nr_bytes);
  auto it = grid.cells.begin();
  auto it_end = grid.cells.end();
  auto im_it = image.begin();
  while (it != it_end)
    *im_it++ = (*it++ & 1) ? 0 : 255;
  paint(wh, image.data(), grid.width, grid.height, 1);
  }

void run_game_loop()
  {
  while (!stop_game_thread)
    {
    grid = next_generation(grid);
    paint_grid();
    std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(game_sleep_time));
    }
  }

void scm_stop()
  {
  if (game_thread)
    {
    stop_game_thread = true;
    game_thread->join();
    game_thread.reset(nullptr);
    }
  }

void scm_run()
  {
  scm_stop();
  stop_game_thread = false;
  game_thread.reset(new std::thread(run_game_loop));
  }

void scm_resize(schemerlicht_object* w, schemerlicht_object* h)
  {
  if (w->type != schemerlicht_object_type_fixnum)
    {
    std::cout << "First argument should be a fixnum\n";
    return;
    }
  if (h->type != schemerlicht_object_type_fixnum)
    {
    std::cout << "Second argument should be a fixnum\n";
    return;
    }
  scm_stop();
  if (w->value.fx > 0)
    width = (int)w->value.fx;
  if (h->value.fx > 0)
    height = (int)h->value.fx;
  grid = cell_map(width, height);
  paint_grid();
  }

void scm_randomize()
  {
  scm_stop();
  std::random_device rd;
  std::mt19937 gen(rd());

  grid = cell_map(width, height);

  for (int i = 0; i < width * height / 2; ++i)
    {
    int x = gen() % width;
    int y = gen() % height;
    if (grid.cell_state(x, y) == 0)
      grid.set_cell(x, y);
    }
  paint_grid();
  }

void scm_clear()
  {
  scm_stop();
  grid = cell_map(width, height);
  paint_grid();
  }

void scm_game_sleep(schemerlicht_object* s)
  {
  if (s->type == schemerlicht_object_type_fixnum)
    game_sleep_time = (double)(s->value.fx);
  else if (s->type == schemerlicht_object_type_flonum)
    game_sleep_time = (double)(s->value.fl);
  }

void scm_set_cell(schemerlicht_object* x, schemerlicht_object* y)
  {
  if (x->type != schemerlicht_object_type_fixnum)
    {
    std::cout << "First argument should be a fixnum\n";
    return;
    }
  if (y->type != schemerlicht_object_type_fixnum)
    {
    std::cout << "Second argument should be a fixnum\n";
    return;
    }
  scm_stop();
  if (x->value.fx >= 0 && y->value.fx >= 0 && x->value.fx < grid.width && y->value.fx < grid.height)
    {
    if (grid.cell_state((int)(x->value.fx), (int)(y->value.fx)) == 0)
      grid.set_cell((int)(x->value.fx), (int)(y->value.fx));
    }
  paint_grid();
  }

void scm_clear_cell(schemerlicht_object* x, schemerlicht_object* y)
  {
  if (x->type != schemerlicht_object_type_fixnum)
    {
    std::cout << "First argument should be a fixnum\n";
    return;
    }
  if (y->type != schemerlicht_object_type_fixnum)
    {
    std::cout << "Second argument should be a fixnum\n";
    return;
    }
  scm_stop();
  if (x->value.fx >= 0 && y->value.fx >= 0 && x->value.fx < grid.width && y->value.fx < grid.height)
    {
    if (grid.cell_state((int)(x->value.fx), (int)(y->value.fx)) != 0)
      grid.clear_cell((int)(x->value.fx), (int)(y->value.fx));
    }
  paint_grid();
  }

void scm_next()
  {
  scm_stop();
  grid = next_generation(grid);
  paint_grid();
  }

void scm_gun()
  {
  scm_stop();
  if (width < 38)
    width = 38;
  if (height < 11)
    height = 11;
  grid = cell_map(width, height);

  grid.set_cell(1, 5);
  grid.set_cell(2, 5);
  grid.set_cell(1, 6);
  grid.set_cell(2, 6);

  grid.set_cell(35, 3);
  grid.set_cell(36, 3);
  grid.set_cell(35, 4);
  grid.set_cell(36, 4);

  grid.set_cell(11, 5);
  grid.set_cell(11, 6);
  grid.set_cell(11, 7);
  grid.set_cell(12, 4);
  grid.set_cell(12, 8);
  grid.set_cell(13, 3);
  grid.set_cell(13, 9);
  grid.set_cell(14, 3);
  grid.set_cell(14, 9);
  grid.set_cell(15, 6);
  grid.set_cell(16, 4);
  grid.set_cell(16, 8);
  grid.set_cell(17, 5);
  grid.set_cell(17, 6);
  grid.set_cell(17, 7);
  grid.set_cell(18, 6);

  grid.set_cell(21, 3);
  grid.set_cell(21, 4);
  grid.set_cell(21, 5);
  grid.set_cell(22, 3);
  grid.set_cell(22, 4);
  grid.set_cell(22, 5);
  grid.set_cell(23, 2);
  grid.set_cell(23, 6);
  grid.set_cell(25, 1);
  grid.set_cell(25, 2);
  grid.set_cell(25, 6);
  grid.set_cell(25, 7);
  paint_grid();
  }


void scm_space_rake()
  {
  scm_stop();
  if (width < 26)
    width = 26;
  if (height < 23)
    height = 23;
  grid = cell_map(width, height);

  grid.set_cell(2, 17);
  grid.set_cell(2, 19);
  grid.set_cell(3, 20);
  grid.set_cell(4, 20);
  grid.set_cell(5, 17);
  grid.set_cell(5, 20);
  grid.set_cell(6, 18);
  grid.set_cell(6, 19);
  grid.set_cell(6, 20);


  grid.set_cell(19, 19);
  grid.set_cell(19, 17);
  grid.set_cell(20, 16);
  grid.set_cell(21, 16);
  grid.set_cell(22, 19);
  grid.set_cell(22, 16);
  grid.set_cell(23, 16);
  grid.set_cell(23, 17);
  grid.set_cell(23, 18);

  grid.set_cell(19, 5);
  grid.set_cell(19, 3);
  grid.set_cell(20, 2);
  grid.set_cell(21, 2);
  grid.set_cell(22, 5);
  grid.set_cell(22, 2);
  grid.set_cell(23, 2);
  grid.set_cell(23, 3);
  grid.set_cell(23, 4);

  grid.set_cell(8, 9);
  grid.set_cell(9, 8);
  grid.set_cell(9, 10);
  grid.set_cell(10, 7);
  grid.set_cell(10, 8);
  grid.set_cell(10, 10);
  grid.set_cell(10, 11);
  grid.set_cell(11, 10);
  grid.set_cell(11, 11);
  grid.set_cell(12, 10);
  grid.set_cell(12, 11);
  grid.set_cell(13, 10);
  grid.set_cell(13, 11);
  grid.set_cell(13, 12);

  grid.set_cell(11, 3);
  grid.set_cell(11, 4);
  grid.set_cell(12, 3);
  grid.set_cell(12, 4);
  grid.set_cell(12, 5);
  grid.set_cell(13, 2);
  grid.set_cell(13, 4);
  grid.set_cell(13, 5);
  grid.set_cell(14, 2);
  grid.set_cell(14, 3);
  grid.set_cell(14, 4);
  grid.set_cell(15, 3);

  grid.set_cell(17, 11);
  grid.set_cell(18, 9);
  grid.set_cell(18, 10);
  grid.set_cell(18, 11);
  grid.set_cell(18, 12);
  grid.set_cell(19, 8);
  grid.set_cell(19, 12);
  grid.set_cell(20, 8);
  grid.set_cell(20, 11);
  grid.set_cell(21, 9);
  grid.set_cell(21, 10);
  grid.set_cell(21, 11);

  paint_grid();
  }


void scm_spaceship()
  {
  scm_stop();
  if (width < 7)
    width = 7;
  if (height < 6)
    height = 6;
  grid = cell_map(width, height);

  grid.set_cell(2, 5);
  grid.set_cell(2, 3);
  grid.set_cell(3, 2);
  grid.set_cell(4, 2);
  grid.set_cell(5, 5);
  grid.set_cell(5, 2);
  grid.set_cell(6, 2);
  grid.set_cell(6, 3);
  grid.set_cell(6, 4);

  paint_grid();
  }

void register_functions(schemerlicht_context* ctxt)
  {
  schemerlicht_register_external_primitive(ctxt, "resize", (SCHEMERLICHT_FUNPTR())&scm_resize, schemerlicht_foreign_void, 2);
  schemerlicht_register_external_primitive(ctxt, "randomize", (SCHEMERLICHT_FUNPTR())&scm_randomize, schemerlicht_foreign_void, 0);
  schemerlicht_register_external_primitive(ctxt, "clear", (SCHEMERLICHT_FUNPTR())&scm_clear, schemerlicht_foreign_void, 0);
  schemerlicht_register_external_primitive(ctxt, "next", (SCHEMERLICHT_FUNPTR())&scm_next, schemerlicht_foreign_void, 0);
  schemerlicht_register_external_primitive(ctxt, "run", (SCHEMERLICHT_FUNPTR())&scm_run, schemerlicht_foreign_void, 0);
  schemerlicht_register_external_primitive(ctxt, "stop", (SCHEMERLICHT_FUNPTR())&scm_stop, schemerlicht_foreign_void, 0);
  schemerlicht_register_external_primitive(ctxt, "game-sleep", (SCHEMERLICHT_FUNPTR())&scm_game_sleep, schemerlicht_foreign_void, 1);
  schemerlicht_register_external_primitive(ctxt, "set-cell", (SCHEMERLICHT_FUNPTR())&scm_set_cell, schemerlicht_foreign_void, 2);
  schemerlicht_register_external_primitive(ctxt, "clear-cell", (SCHEMERLICHT_FUNPTR())&scm_clear_cell, schemerlicht_foreign_void, 2);
  schemerlicht_register_external_primitive(ctxt, "gun", (SCHEMERLICHT_FUNPTR())&scm_gun, schemerlicht_foreign_void, 0);
  schemerlicht_register_external_primitive(ctxt, "space-rake", (SCHEMERLICHT_FUNPTR())&scm_space_rake, schemerlicht_foreign_void, 0);
  schemerlicht_register_external_primitive(ctxt, "spaceship", (SCHEMERLICHT_FUNPTR())&scm_spaceship, schemerlicht_foreign_void, 0);
  }

std::string get_cleaned_command(std::string txt)
  {
  auto it = txt.find_first_not_of(' ');
  std::string cleaned = txt.substr(it);
  it = cleaned.find_first_of(' ');
  if (it == std::string::npos)
    return cleaned;
  return cleaned.substr(0, it);
  }

int main()
  {
  wh = create_window("Game of life", 512, 512); // create window for visualization
  scm_randomize(); // fill grid cells at random  

  schemerlicht_context* ctxt = schemerlicht_open(1024);
  schemerlicht_build_base(ctxt);
  register_functions(ctxt);

  bool quit = false;
  std::string input;
  std::cout << "Schemerlicht example: Game of life\n";
  std::cout << "Type exit or quit to stop.\n";
  std::cout << "The following commands can be used to modify the game of life grid:\n";
  std::cout << "(resize w h): resize the grid to w x h cells\n";
  std::cout << "(randomize): fill the grid with random cells\n";
  std::cout << "(clear): clears the grid\n";
  std::cout << "(next): show the next generation\n";
  std::cout << "(run): starts a game of life simulation\n";
  std::cout << "(stop): stops the game of life simulation\n";
  std::cout << "(game-sleep ms): waits 'ms' milliseconds between generations\n";
  std::cout << "(set-cell x y): sets cell (x,y) in the grid\n";
  std::cout << "(clear-cell x y): clears cell (x,y) in the grid\n";
  std::cout << "(gun): generates the Gosper glider gun\n";
  std::cout << "(space-rake): generates the spacer-rake\n";
  std::cout << "(spaceship): generates the spaceship or glider\n";
  std::cout << "\n";
  while (!quit)
    {
    std::cout << "> ";
    std::getline(std::cin, input);
    std::string cmd = get_cleaned_command(input);
    if (cmd == std::string("exit") || cmd == std::string("quit"))
      quit = true;
    else if (!input.empty())
      {
      schemerlicht_object* res = schemerlicht_execute(ctxt, input.c_str());
      schemerlicht_string s;
      schemerlicht_string_init(ctxt, &s, "");
      schemerlicht_show_object(ctxt, res, &s);
      std::cout << s.string_ptr << "\n";
      schemerlicht_string_destroy(ctxt, &s);
      }
    }

  scm_stop(); // stop the game thread

  schemerlicht_close(ctxt);
  close_window(wh); // close the visualization window

  return 0;
  }
