#!/usr/bin/perl -w

use strict;

open IN, "<pvcam/pvcam.h" or die "Cannot read pvcam.h\n";
open OUT, ">src/pvp/pvpDummy.cpp" or die "Cannot write pvpDummmy.cpp\n";

print OUT "// pvpDummy.cpp - Dummy implementation of pvcam library.\n";
print OUT "// Generated by pvcam2dummy.pl. Do not edit.\n";
print OUT "\n";
print OUT "#include <pvp/dwpvcam.h>\n";
print OUT "\n";

while (<IN>) {
  /DEPRECATED/ and next;
  /PV_DECL/ and writefunc($_);
}

sub writefunc {
  my $line = $_;
  chomp $line;
  while (!($line =~ /\);/)) {
    $line .= <IN>;
    chomp $line;
  }
  $line =~ s/\s+/ /g;
  $line =~ s/^\s+//;
  $line =~ s/\s+$//;
  $line =~ /(.*)\((.*)\);/ or die "Bad function declaration: $line\n";
  my $pre = $1;
  my @args = split(/,\s*/,$2);
  for (@args) {
    s/\s+[a-zA-Z_][a-zA-Z0-9_]*$//;
  }
  $pre =~ s/\s+$//;
  print OUT "$pre(", join(", ",@args), ") {\n";
  print OUT "  return false;\n";
  print OUT "}\n";
  print OUT "\n";
}