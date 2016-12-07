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
    $usedfuncs{$1} = 1 if /daqTry\(DAQmx([a-zA-Z]+)\(/;
    my @bits = split(/[,\s()]/, $_);
    for (@bits) {
      $usedconsts{$1} = 1 if /(DAQmx_[a-zA-Z_]+)/;
    }
  }
}

open IN, "<../NIDAQmx.h" or die "Cannot read NIDAQmx.h\n";
open OUT, ">src/daq/daqdummy.cpp" or die "Cannot write daqdummy.cpp\n";

print OUT "// daqdummy.cpp - Dummy version of the NIDAQmx library.\n";
print OUT "// Created by daqmx2dummy.pl. Do not edit.\n";
print OUT "\n";
print OUT "#include <daq/dwNIDAQmx.h>\n";
print OUT "\n";

while (<IN>) {
  chomp;
  /CFUNC/ and /DAQmx([A-Za-z]+)/ or next;
  my $foo = $1;
  next unless exists $usedfuncs{$1};
  s/\s+/ /g;
  /(.*)\s*\((.*)\);/ or die "Bad function declaration: $_\n";
  my $pre = $1;
  my @args = split(/, */,$2);
  for (@args) { s/ (\**)[a-zA-Z][a-zA-Z0-9_]*(\[?\]?)$/$1$2/; }
  print OUT "int32 DAQmx$foo(",join(", ",@args),") {\n";
  print OUT "  return -1;\n";
  print OUT "}\n\n";
}
close OUT;
close IN;


open IN, "<../NIDAQmx.h" or die "Cannot read NIDAQmx.h\n";
open OUT, ">src/daq/dwNIDAQmx.h" or die "Cannot write dwNIDAQmx.h\n";

print OUT <<"EOF";
// dwNIDAQmx.h - NIDAQ headers for VScope.
// Created by daqmx2dummy.pl. Do not edit.

/*============================================================================*/
/*                 National Instruments / Data Acquisition                    */
/*----------------------------------------------------------------------------*/
/*    Copyright (c) National Instruments 2003-2016.  All Rights Reserved.     */
/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Title:       NIDAQmx.h                                                     */
/* Purpose:     Include file for NI-DAQmx  library support.                */
/*                                                                            */
/*============================================================================*/

#ifndef ___nidaqmx_h___
#define ___nidaqmx_h___

#ifdef __cplusplus
	extern "C" {
#endif

#ifndef __linux__
#define __CFUNC         __stdcall
#define __CFUNC_C       __cdecl
#define __CFUNCPTRVAR   __cdecl
#define CVICDECL        __cdecl
#define CVICALLBACK     CVICDECL
#endif
