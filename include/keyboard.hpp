#pragma once

#include <string>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <unordered_map>
#include "../include/keys.hpp"

namespace keyboard
{
  using key_name_to_code_table_t = std::unordered_map<std::string, const int>;
  using key_code_to_name_table_t = std::unordered_map<int, const std::string>;

  typedef struct __keys_db_t
  {
    public:
      static const __keys_db_t& instance()
      {
        static __keys_db_t kh;
        return kh;
      }

      int get_key_code_from_db(const std::string& name) const
      {
        return k_name_to_cdt_.at(name);
      }

      std::string get_key_name_from_db(int key_code) const
      {
        if( is_valid_key_code(key_code) )
          return k_code_to_nt_.equal_range(key_code).first->second;

        return {};
      }

      bool is_valid_key_code(int key_code) const
      {
        return (key_code > 0) && 
          (k_code_to_nt_.count(key_code) > 0);
      }

    private:
      __keys_db_t()
      {
        for ( auto& key : keys )
        {
          k_code_to_nt_.emplace(
            std::get<1>(key), 
            std::get<0>(key)
          );
          k_name_to_cdt_.emplace(
            std::get<0>(key), 
            std::get<1>(key)
          );        
        }
      }

    private:
      key_code_to_name_table_t k_code_to_nt_;
      key_name_to_code_table_t k_name_to_cdt_;
      
  } keys_db_t;

  typedef struct __keystroker_t
  {
    enum class state_t : uint_fast8_t
    {
      up_down = 0,
      down = 1,
      up = 2,
      press = up_down
    };

    static const __keystroker_t& instance()
    {
      static __keystroker_t kw;
      return kw;
    }

    void operator()(
      const std::string& name,
      const state_t = state_t::press) const
    {
      operator()(keys_db_t::instance().get_key_code_from_db(name));
    }
    
    void operator()(
      const int code,
      const state_t state = state_t::press) const
    {
      if ( !keys_db_t::instance().is_valid_key_code(code) )
        throw std::runtime_error("invalid key code");
      INPUT input{};
      ZeroMemory(&input, sizeof(input));
      input.type = INPUT_KEYBOARD;
      input.ki.wVk = static_cast<WORD>(code);
      input.ki.wScan = static_cast<WORD>(MapVirtualKey(input.ki.wVk, 0));
      input.ki.dwFlags = KEYEVENTF_EXTENDEDKEY;
      if ( state == state_t::up )
        input.ki.dwFlags |= KEYEVENTF_KEYUP;
      input.ki.time = 0;
      input.ki.dwExtraInfo = ::GetMessageExtraInfo();
      SendInput(1, &input, sizeof(INPUT));
      if ( state == state_t::press )
      {
        input.ki.dwFlags |= KEYEVENTF_KEYUP;
        SendInput(1, &input, sizeof(INPUT));
      }
    }

  } keystroker_t; 

}
