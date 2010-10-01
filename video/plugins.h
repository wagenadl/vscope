// Plugins.H

#ifndef PLUGINS_H

#define PLUGINS_H

class Plugins {
  /*:C Plugins
   *:D Handles communication with the plugins
   */
public:
  Plugins(int renderwidth, int renderheight, char const *path=0);
  ~Plugins();
  void listNames();
  void listDetails();
  class Plugin *plugin(int n, bool init=true); // this automatically initializes. Throws error if nonexistent. This call's timing is O(N), where N is the number of plugins.
  class Plugin *first(); // this may return a non-initialized plugin. Returns 0 if nonexistent.
  class Plugin *next(); // this may return a non-initialized plugin. Only valid after first() has been called.
private:
  struct ListEl {
    struct ListEl *next;
    class Plugin *plugin;
  };
  ListEl *first_;
  ListEl *iter;
};

#endif
