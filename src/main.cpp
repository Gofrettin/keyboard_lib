#include "../include/keyboard.hpp"

using keyboard::keys_db_t;
using keyboard::keystroker_t;

int main()
{
  auto& ks = keystroker_t::instance();

  ks("caps lock");
  ks("h");
  ks("i");

  std::cin.get();
}

