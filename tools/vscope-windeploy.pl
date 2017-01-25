#!/usr/bin/perl -w

use strict;
use Cwd;
use File::Copy;
use File::Path;

######################################################################
# EXTERNAL PATHS
my $qbin_path = "c:/Qt/Qt5.7.1-x86/5.7/msvc2015/bin";
my $msvc_path = "c:/Program Files (x86)/Microsoft Visual Studio 14.0/VC";

######################################################################
# INTERNAL PATHS
my $vscope_buildpath = "build-vscope-x86/release";
my $release_path = "release-vscope-x86";

######################################################################

$msvc_path =~ s/\//\\/g;
$ENV{VCINSTALLDIR} = $msvc_path;

File::Path::remove_tree($release_path) if -d $release_path;
File::Path::make_path($release_path);

system("$qbinpath/windeployqt --dir $release_path "
       . " --compiler-runtime $vscope_buildpath/vscope.exe")
  and die "Failed to get vscope deployment";

File::Copy::copy("$vscope_buildpath/vscope.exe", "$releasepath/");

print "Now run 'tools/vscope-x86.iss' using Inno Setup.\n";
