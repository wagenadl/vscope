#!/usr/bin/perl -w

use strict;

my $copyright = "(C) Daniel Wagenaar 2008-1017.";

sub usage {
  print STDERR "Usage: apply-gpl.pl filename\n";
  exit 1;
}

sub licensetext {
  my $text = << "EOF";
VScope is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

VScope is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with VScope.  If not, see <http://www.gnu.org/licenses/>.
EOF
  return $text;
}

sub badfiletype {
  my $fn = shift;
  print STDERR "Do not known how to apply GPL to $fn\n";
  exit 1;
}

sub apply_c {
  my $fn = shift;
  open IN, "<$fn~" or die "Cannot open input $fn~\n";
  open OUT, ">$fn" or die "Cannot open output $fn\n";
  print OUT "// $fn - This file is part of VScope.\n";
  print OUT "// $copyright";
  print OUT "\n";
  my $pfx = "/* ";
  my @lines = split(/\n/, licensetext());
  for (@lines) {
    chomp;
    if (/^\s*$/) {
      print OUT "\n";
    } else {
      print OUT "$pfx$_\n";
      $pfx = "   ";
    }
  }
  print OUT "*/\n\n";
  while (<IN>) {
    print OUT $_;
  }
  close IN;
  close OUT;
}

sub apply_matlab {
  my $fn = shift;
  open IN, "<$fn~" or die "Cannot open input $fn~\n";
  open OUT, ">$fn" or die "Cannot open output $fn\n";
  my @lines = split(/\n/, licensetext());
  my $carry;
  my $incomment = 0;
  while (<IN>) {
    if (/^\s*\%/) {
      $incomment = 1;
    } elsif ($incomment) {
      $carry = $_;
      last;
    }
    print OUT $_;
  }
  print OUT "\n";
  print OUT "% This file is part of VScope. $copyright\n\n";
  for (@lines) {
    if (/^\s*$/) {
      print OUT "%\n";
    } else {
      print OUT "% $_\n";
    }
  }
  print OUT $carry if defined $carry;
  while (<IN>) {
    print OUT $_;
  }
  close IN;
  close OUT;
}
  

######################################################################

my $fn = shift @ARGV or usage();
usage() if @ARGV;

system("grep -q 'This file is part of VScope' $fn") or exit 0;

badfiletype($fn) unless $fn =~ /\.(cpp|h|m)$/;

print STDERR "Applying GPL text to $fn\n";

system("mv $fn $fn~");

if ($fn =~ /\.m$/) {
  apply_matlab($fn);
} else {
  apply_c($fn);
}
