// Plugins.C

#include "Plugins.H"
#include "Plugin.H"
#include "Error.H"
#include "Paths.H"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


Plugins::Plugins(int wid, int hei, char const *path) {
  first_ = 0;
  char defpath[] = VSDVIDEOPATH;
  if (!path)
    path = getenv("VSDVIDEOPATH");
  if (!path)
    path = defpath;
  
  ListEl **nextp = &first_;
  for (int n=0; n<100; n++) {
    Plugin *p;
    try {
      p = new Plugin(path, n, wid, hei, false);
    } catch (Error const &) {
      p = 0; // ignore errors
    }
    if (p) {
      *nextp = new ListEl;
      (*nextp)->next = 0;
      (*nextp)->plugin = p;
      nextp = &(*nextp)->next;
    }
  }
  if (first_==0)
    throw Error("Plugins: No plugins found");
}

Plugins::~Plugins() {
  ListEl *iter = first_;
  while (iter) {
    delete iter->plugin;
    ListEl *next = iter->next;
    delete iter;
    iter = next;
  }
}

Plugin *Plugins::plugin(int n, bool init) {
  ListEl *iter = first_;
  while (iter) {
    if (iter->plugin->number() == n) {
      if (init)
	iter->plugin->initialize();
      return iter->plugin;
    }
    iter = iter->next;
  }
  throw Error("Plugins: Unknown plugin requested");
}

Plugin *Plugins::first() {
  iter = first_;
  return next();
}

Plugin *Plugins::next() {
  if (iter) {
    Plugin *p = iter->plugin;
    iter = iter->next;
    return p;
  } else {
    return 0;
  }
}

void Plugins::listNames() {
  fprintf(stderr,"Plugins:\n");
  for (Plugin *p=first(); p; p=next()) 
    fprintf(stderr,"  %02i: %s\n",p->number(), p->name());
}

void Plugins::listDetails() {
  fprintf(stderr,"Plugins:\n");
  for (Plugin *p=first(); p; p=next()) {
    fprintf(stderr,"  %02i: %s\n",p->number(), p->name());
    fprintf(stderr,"    Description: %s\n",p->description());
    char const *parnames = p->parnames();
    char const *parunits = p->parunits();
    char parname[256];
    char parunit[256];
    for (int k=1; k<99; k++) {
      strncpy(parname,parnames,255);
      char *e = strchr(parname,':');
      if (e) {
	*e=0;
	parnames += (e-parname) + 1;
      } else {
	parnames += strlen(parname);
      }
      strncpy(parunit,parunits,255);
      e = strchr(parunit,':');
      if (e) {
	*e=0;
	parunits += (e-parunit) + 1;
      }
      if (*parname) {
	fprintf(stderr,"    Parameter %i: %s (%s)\n",k, parname, parunit);
	char const *dfl = p->parsuggest(k);
	fprintf(stderr,"      Suggested values: %s\n",dfl?dfl:"(none)");
      }
    }
  }      
}

