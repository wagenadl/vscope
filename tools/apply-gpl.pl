#!/usr/bin/perl -w

use strict;

sub usage {
  print STDERR "Usage: apply-gpl.pl filename\n";
  exit 1;
}

sub licensetext {
  return << "EOF";
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
  print OUT "// $fn - This file is part of VScope\n";
  print OUT "\n/* ";
  my @lines = split(/\n/, $licensetext);
  print OUT join(@lines, "\n   ");
  print OUT "\n*/\n\n";
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
  my @lines = split(/\n/, $licensetext);
  my $carry;
  while (<IN>) {
    if (/^\s*\%/) {
      print OUT $_;
    } else {
      $carry = $_;
      last;
    }
  }
  print OUT "\n";
  print OUT "% This file is part of VScope\n\n";
  for (@lines) {
    print OUT "% $_";
  }
  print OUT $carry;
  while (<IN>) {
    print OUT $_;
  }
  close IN;
  close OUT;
}
  

######################################################################

my $fn = shift @ARGV or usage();
usage() if @ARGV;

system("grep -q 'This file is part of VScope' $fn") and exit 0;

badfiletype($fn) unless $fn =~ /\.(cpp|h|m)$/;

print STDERR "Applying GPL text to $fn\n";

system("mv $fn $fn~");

if ($fn =~ /\.m$/) {
  apply_matlab($fn);
} else {
  apply_c($fn);
}
