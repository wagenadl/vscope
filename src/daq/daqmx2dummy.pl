#!/usr/bin/perl -w
use strict;

open IN, "<dwNIDAQmx.h" or die "Cannot read NIDAQmx.h\n";
open OUT, ">daqdummy.cpp" or die "Cannot write daqdummy.cpp\n";

print OUT "// daqdummy.cpp - Dummy version of the NIDAQmx library.\n";
print OUT "// Created by daqmx2dummy.pl. Do not edit.\n";
print OUT "\n";
print OUT "#include <daq/dwNIDAQmx.h>\n";
print OUT "\n";

while (<IN>) {
  chomp;
  /CFUNC/ and /DAQmx/ or next;
  s/\s+/ /g;
  /(.*)\s*\((.*)\);/ or die "Bad function declaration: $_\n";
  my $pre = $1;
  my @args = split(/, */,$2);
  for (@args) { s/ (\**)[a-zA-Z][a-zA-Z0-9_]*(\[?\]?)$/$1$2/; }
  print OUT "$pre(",join(", ",@args),") {\n";
  print OUT "  return -1;\n";
  print OUT "}\n\n";
}

