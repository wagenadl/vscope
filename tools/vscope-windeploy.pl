#!/usr/bin/perl -w

use strict;
use Cwd;
use File::Copy;
use File::Path;

######################################################################
# EXTERNAL PATHS
my $qbin_path = "c:/Qt/5.11.0/msvc2015/bin";
my $msvc_path = "c:/Program Files (x86)/Microsoft Visual Studio 14.0/VC";
my $msvc_redist_path = "$msvc_path/redist/x86/Microsoft.VC140.CRT";

######################################################################
# INTERNAL PATHS
my $vscope_buildpath = "build-vscope-x86/release";
my $dpss_buildpath = "build-mtpsd";
my $release_path = "release-vscope-x86";

######################################################################

my $msvc_dos = $msvc_path;
$msvc_dos =~ s/\//\\/g;
$ENV{VCINSTALLDIR} = $msvc_dos;

######################################################################
# Create release directory
File::Path::remove_tree($release_path) if -d $release_path;
File::Path::make_path($release_path);

######################################################################
# Copy VScope binary
system("$qbin_path/windeployqt --dir $release_path "
       . " --compiler-runtime $vscope_buildpath/vscope.exe")
  and die "Failed to get vscope deployment";

File::Copy::copy("$vscope_buildpath/vscope.exe", "$release_path/");

######################################################################
# Copy VScope dependencies
File::Copy::copy("$msvc_redist_path/msvcp140.dll", "$release_path/");
File::Copy::copy("$msvc_redist_path/concrt140.dll", "$release_path/");
File::Copy::copy("$msvc_redist_path/vccorlib140.dll", "$release_path/");
File::Copy::copy("$msvc_redist_path/vcruntime140.dll", "$release_path/");


#######################################################################
# Copy mkdpss and dependencies
File::Copy::copy("$dpss_buildpath/bin/mkdpss.exe", "$release_path/");

my @ldd = split(/\n/, `ldd $dpss_buildpath/bin/mkdpss.exe`);
for my $ld (@ldd) {
  print "Line: $ld\n";
  $ld = lc($ld);
  next unless $ld =~ /(\/.*dll)/;
  print "Dependency: $1\n";
  File::Copy::copy("$1", "$release_path/");
}


#######################################################################
print "Now run 'tools/vscope-x86.iss' using Inno Setup.\n";
