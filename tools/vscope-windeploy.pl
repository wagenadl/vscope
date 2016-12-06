#!/usr/bin/perl -w

use strict;
use Cwd;

print "Hello world\n";

my $here = getcwd();

print "pwd: $here\n";
for my $k (sort keys %ENV) {
#print "ENV $k: $ENV{$k}\n";
}

my $qbinpath = "c:/Qt/Qt5.7.0-x86/5.7/msvc2013/bin";
$ENV{VCINSTALLDIR} = "C:\\Program Files (x86)\\Microsoft Visual Studio 12.0\\VC";
system("$qbinpath/windeployqt --dir vscope-w32 --compiler-runtime vscope.exe");
system("copy vscope.exe vscope-w32\\vscope.exe");
