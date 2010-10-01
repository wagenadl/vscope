#!/usr/bin/perl -w

use strict;

my %globalrefs;
open GREP, "grep 'Globals::' */*.h */*.cpp |" or die "Cannot grep";
while (<GREP>) {
  /^(.*?):/ or next;
  my $fn = $1;
  /Globals::([a-z0-9_]+)/ or next;
  my $var = $1;
  unless (exists $globalrefs{$var}) {
    my %foo;
    $globalrefs{$var} = \%foo;
  }
  ${$globalrefs{$var}}{$fn} = 1;
}
close GREP;

open DOT, ">/tmp/graph.dot" or die "Cannot create graph";
print DOT "digraph Globals {\n  rankdir=LR\n";
for my $to (sort keys %globalrefs) {
  for my $frm (sort keys %{$globalrefs{$to}}) {
    my $len = int(rand()*10+1);
    print DOT "  $to -> \"$frm\" [minlen=$len];\n";
  }
}
print DOT "}\n";
close DOT;

system "dot -Tps /tmp/graph.dot -o globalgraph.ps";
