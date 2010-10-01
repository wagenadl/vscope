// foo.C

#include <stdio.h>

class Foo_ {
public:
  Foo_() { printf("Foo_ %p constructor\n",this); }
  Foo_(const Foo_ &o) { printf("Foo_ %p copy constructor <- %p\n",this,&o); }
  Foo_ &operator=(const Foo_ &o) { printf("Foo_ %p assignment <- %p\n",this,&o); return *this; }
  ~Foo_() { printf("Foo_ %p destructor\n",this); }
};

class Foo: public Foo_ {
public:
  Foo() { printf("Foo %p constructor\n", this); y='*'; }
  //private:
  Foo(const Foo &o): Foo_(o) { printf("copy constructor: %p <- %p %c\n",this,&o,y=o.y); }
  Foo &operator=(Foo const &o) { *(Foo_*)this = o; printf("assignment: %p = %p %c\n",this,&o,y=o.y); }
public:
  ~Foo() { printf("Foo %p destructor. y=%c\n",this,y); }
  void set(char x) { printf("Foo %p. y=%c\n",this,y=x); }
  char y;
};

int main() {
  printf("making a\n");
  Foo a;
  a.set('a');
  printf("made a; making b\n");
  Foo b(a);
  b.set('b');
  printf("made b; making c\n");
  Foo c;
  c.set('c');
  printf("copying b into c\n");
  c=b;
  printf("copying a into c\n");
  c=a;
  printf("all done\n");
  return 0;
}


