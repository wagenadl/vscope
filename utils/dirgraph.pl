#!/usr/bin/perl -w

use strict;

my @dirs;
opendir DIR, ".";
for (readdir DIR) {
  chomp;
  /^\./ and next;
  /^utils$/ and next;
  /^test$/ and next;
  push @dirs, $_ if -d $_;
}
closedir DIR;

my %dirs;
for (@dirs) {
  $dirs{$_} = 1;
}

my %dirrefs;
for my $dir (@dirs) {
  my %hererefs;
  open GREP, "grep '#include' $dir/*.h $dir/*.cpp |" or die "Cannot grep in '$dir'";
  while (<GREP>) {
    /<([a-z]+)\// and $hererefs{$1} = 1;
  }
  close GREP;
  my @hererefs;
  for (keys %hererefs) {
    push @hererefs, $_ if exists $dirs{$_};
  }
  $dirrefs{$dir} = \@hererefs;
}

open DOT, ">/tmp/graph.dot" or die "Cannot create graph";
print DOT "digraph Directories {\n";
for my $frm (sort keys %dirrefs) {
  for my $to (@{$dirrefs{$frm}}) {
    print DOT "  $frm -> $to;\n" unless $to eq $frm;
  }
}
print DOT "}\n";
close DOT;

system "dot -Tps /tmp/graph.dot -o dirgraph.ps";
