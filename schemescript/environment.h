#pragma once

#include "namespace.h"
#include "liveness_range.h"

#include <map>
#include <stdint.h>
#include <string>

COMPILER_BEGIN

template <class TEntry>
class environment
  {
  public:
    environment(const environment<TEntry>* outer = nullptr)
      {
      if (outer)
        env = outer->env;
      }
  
    bool has(const std::string& name)
      {
      auto it = env.find(name);
      return (it != env.end());
      }
      
    bool find(TEntry& e, const std::string& name)
      {
      auto it = env.find(name);
      if (it != env.end())
        {
        e = it->second;
        return true;
        }
      return false;
      }
      
    template <class Pred>
    bool find_if(std::pair<std::string, TEntry>& out, Pred p)
      {
      auto it = std::find_if(env.begin(), env.end(), p);
      if (it != env.end())
        {
        out = *it;
        return true;
        }
      return false;
      }
      
    bool replace(const std::string& name, TEntry e)
      {
      auto it = env.find(name);
      if (it != env.end())
        {
        it->second = e;
        return true;
        }
      return false;
      }
      
    void remove(const std::string& name)
      {
      auto it = env.find(name);
      if (it != env.end())
        {
        env.erase(it);
        }
      }
      
    void push(const std::string& name, TEntry e)
      {
      env[name] = e;
      }
      
    typename std::map<std::string, TEntry>::iterator begin()
      {
      return env.begin();
      }

    typename std::map<std::string, TEntry>::iterator end()
      {
      return env.end();
      }
      
    typename std::map<std::string, TEntry>::const_iterator begin() const
      {
      return env.begin();
      }

    typename std::map<std::string, TEntry>::const_iterator end() const
      {
      return env.end();
      }
      
  private:
    std::map<std::string, TEntry> env;
  };


struct environment_entry
  {
  environment_entry() {}

  enum storage_type
    {
    st_register,
    st_local,
    st_global
    };

  storage_type st;
  uint64_t pos;
  liveness_range live_range;

  bool operator < (const environment_entry& other) const
    {
    if (st == other.st)
      return pos < other.pos;
    return st < other.st;
    }
  };
  
typedef environment<environment_entry> environment_map;

COMPILER_END
