#!/usr/bin/perl -w
use strict;

my %usedfuncs;
my %usedconsts;
my @sources = qw/digitalin digitalout digitalstep
		 analogin analogout
		 daqtask daqbase/;
for my $src (@sources) {
  open IN, "<src/daq/$src.cpp" or die "Cannot scan $src\n";
  while (<IN>) {
    $usedfuncs{$1} = 1 if /daqTry\(DAQmx([a-zA-Z0-9]+)\(/;
    my @bits = split(/[,\s()]/, $_);
    for (@bits) {
      $usedconsts{$1} = 1 if /(DAQmx_[a-zA-Z_]+)/;
    }
  }
}

open IN, "<nidaq/NIDAQmx.h" or die "Cannot read NIDAQmx.h\n";
open OUT, ">src/daq/daqdummy.cpp" or die "Cannot write daqdummy.cpp\n";

print OUT "// daqdummy.cpp - Dummy version of the NIDAQmx library.\n";
print OUT "// Created by daqmx2dummy.pl. Do not edit.\n";
print OUT "\n";
print OUT "#include <../nidaq/NIDAQmx.h>\n";
print OUT "\n";

while (<IN>) {
  chomp;
  /CFUNC/ and /DAQmx([A-Za-z0-9]+)/ or next;
  my $foo = $1;
  next unless exists $usedfuncs{$1};
  s/\s+/ /g;
  /(.*)\s*\((.*)\);/ or die "Bad function declaration: $_\n";
  my $pre = $1;
  my @args = split(/, */, $2);
  for (@args) {
    my $hasconst = s/^const +//;
    s/ (\**)[a-zA-Z][a-zA-Z0-9_]*(\[?\]?)$/$1$2/;
    $_ = "const $_" if $hasconst;
  }
  print OUT "int32 DAQmx$foo(",join(", ",@args),") {\n";
  print OUT "  return -1;\n";
  print OUT "}\n\n";
}
close OUT;
close IN;

